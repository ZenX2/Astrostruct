#include "NEngine.hpp"

NPlayer::NPlayer(std::wstring i_Name) : NNode(NodePlayer)
{
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_newtable(L);
    SelfReference = luaL_ref(L,LUA_REGISTRYINDEX);
    Speed = 4;
    Changed = true;
    Moving = false;
    CurrentDirection = 0;
    Controlled = false;
    Name = i_Name;
    HasSetWantedPosition = false;
    if (GetGame()->IsServer())
    {
        return;
    }
    glGenBuffers(2,Buffers);
    Texture = GetGame()->GetRender()->GetTexture("human");
    Shader = GetGame()->GetRender()->GetShader("flat");
    if (Shader)
    {
        TextureLoc = Shader->GetUniformLocation("Texture");
        MatrixLoc = Shader->GetUniformLocation("MVP");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    NameText = new NText("didactgothic",Name);
    NameText->SetPos(Position+glm::vec3(0,20,0));
    NameText->SwapView();
    NameText->SetLayer(1); // Put it into the world
    NameText->SetMode(1);
    NameText->SetSize(13);
    NameText->SetParent(this);
    float TS = GetGame()->GetMap()->GetTileSize();
    //btCollisionShape* Shape = new btBoxShape(btVector3(TS/3.f,TS/3.f,TS/3.f));
    Shape = new btSphereShape(TS/4.f);
    //btCollisionShape* Shape = new btCapsuleShapeZ(TS/4.f,TS/4.f);
    btVector3 FallInertia(0,0,0);
    Shape->calculateLocalInertia(80,FallInertia);
    btDefaultMotionState* StaticMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
    btRigidBody::btRigidBodyConstructionInfo PlaneBody(80,StaticMotionState,Shape,FallInertia);
    //PlaneBody.m_friction = 30;
    Body = new btRigidBody(PlaneBody);
    Body->setSleepingThresholds(0,0);
    //Body->setAngularFactor(0);
    GetGame()->GetPhysics()->GetWorld()->addRigidBody(Body);
    GetGame()->GetMap()->CallMethod("OnPlayerSpawn","n",this);
}

void NPlayer::SetPos(glm::vec3 i_Position)
{
    Position = i_Position;
    UpdateMatrix();

    GetGame()->GetPhysics()->GetWorld()->removeRigidBody(Body);
    btTransform Trans = Body->getWorldTransform();
    Trans.setOrigin(btVector3(Position.x,Position.y,Position.z));
    Body->setWorldTransform(Trans);
    GetGame()->GetPhysics()->GetWorld()->addRigidBody(Body);
}

NPlayer::~NPlayer()
{
    GetGame()->GetPhysics()->GetWorld()->removeRigidBody(Body);
    delete Body->getMotionState();
    delete Body;
    delete Shape;
    if (GetGame()->IsServer())
    {
        return;
    }
    if (Texture)
    {
        delete Texture;
    }
}

std::string NPlayer::GetName()
{
    return ToMBS(Name);
}

void NPlayer::Move(float Direction)
{
    if (!GetGame()->IsServer())
    {
        if (Texture)
        {
            Texture->Play("run");
        }
    }
    CurrentDirection = Direction;
    Moving = true;
}

void NPlayer::StopMove()
{
    if (!GetGame()->IsServer())
    {
        if (Texture)
        {
            Texture->Play("idle");
        }
    }
    Moving = false;
}

void NPlayer::SetControl()
{
    Controlled = true;
    NLight* Light = new NLight("point");
    Light->SetScale(glm::vec3(256,256,1));
    Light->SetParent(this);
}

void NPlayer::Tick(double DT)
{
    if (!GetGame()->GetMap()->Ready)
    {
        return;
    }
    // Networking
    if (HasSetWantedPosition)
    {
        if (glm::distance(WantedPosition,GetPos()) > GetGame()->GetMap()->GetTileSize())
        {
            SetPos(GetPos()+(WantedPosition-GetPos())*(1.f-float(DT*50)));
        }
    }
    // Movement
    if (Controlled)
    {
        unsigned int MoveFlags = GetGame()->GetInput()->GetKey('W');
        MoveFlags = (MoveFlags<<1 | GetGame()->GetInput()->GetKey('A'));
        MoveFlags = (MoveFlags<<1 | GetGame()->GetInput()->GetKey('S'));
        MoveFlags = (MoveFlags<<1 | GetGame()->GetInput()->GetKey('D'));
        switch (MoveFlags)
        {
            case 1: //A
            {
                Move(PI/2);
                break;
            }
            case 2: //S
            {
                Move(PI);
                break;
            }
            case 3: //A D
            {
                Move((3*PI)/4);
                break;
            }
            case 4: //D
            {
                Move(PI+PI/2);
                break;
            }
            case 6: //D S
            {
                Move(PI+PI/4);
                break;
            }
            case 8: // W
            {
                Move(0);
                break;
            }
            case 9: // W D
            {
                Move(PI/4);
                break;
            }
            case 12: //A W
            {
                Move(PI+(3*PI)/4);
                break;
            }
            case 0:
            {
                StopMove();
                break;
            }
        }
    }
    if (!Moving)
    {
        float* Pos;
        Pos = new float[2];
        GetGame()->GetInput()->GetJoyStick(1,Pos,2);
        if (fabs(Pos[0])+fabs(Pos[1]) > 0.25)
        {
            Move(atan2(Pos[0],Pos[1]));
        } else {
            StopMove();
        }
        delete[] Pos;
    }
    //Collision Movement
    if (Moving)
    {
        btVector3 Vel = Body->getLinearVelocity();
        Body->setLinearVelocity(btVector3(Vel.x()+sin(CurrentDirection)*Speed,Vel.y()+cos(CurrentDirection)*Speed,Vel.z()));
    }
    //Collisions
    btTransform Trans; 
    Body->getMotionState()->getWorldTransform(Trans);
    btVector3 WPos = Trans.getOrigin();
    Position = glm::vec3(WPos.x(),WPos.y(),WPos.z());
    UpdateMatrix();
    //Misc
    if (!GetGame()->IsServer())
    {
        if (Texture)
        {
            Texture->Tick(DT);
        }
    }
}

void NPlayer::GenerateBuffers()
{
    if (!Changed || !Texture)
    {
        return;
    }
    Verts.clear();
    UVs.clear();
    Verts.push_back(glm::vec2(-16,-16));
    UVs.push_back(glm::vec2(0,1));
    Verts.push_back(glm::vec2(-16,16));
    UVs.push_back(glm::vec2(0,0));
    Verts.push_back(glm::vec2(16,16));
    UVs.push_back(glm::vec2(1,0));
    Verts.push_back(glm::vec2(16,-16));
    UVs.push_back(glm::vec2(1,1));
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
    Changed = false;
}

void NPlayer::Draw(NCamera* View)
{
    GenerateBuffers();
    if (Texture == NULL || GetColor().w == 0)
    {
        return;
    }
    if (Shader == NULL)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
        glVertexPointer(2,GL_FLOAT,0,NULL);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
        glTexCoordPointer(2,GL_FLOAT,0,NULL);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (Texture != NULL)
        {
            glBindTexture(GL_TEXTURE_2D,Texture->GetID());
        }

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(&View->GetOrthoMatrix()[0][0]);
        glMatrixMode(GL_MODELVIEW);
        glm::mat4 MVP = View->GetViewMatrix()*GetModelMatrix();
        glLoadMatrixf(&MVP[0][0]);

        glColor4fv(&(GetColor()[0]));
        glDrawArrays(GL_QUADS,0,Verts.size());
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        return;
    }
    glUseProgram(Shader->GetID());
    glActiveTexture(GL_TEXTURE0);
    if (Texture != NULL)
    {
        glBindTexture(GL_TEXTURE_2D,Texture->GetID());
    }
    glUniform1i(TextureLoc,0);
    glm::mat4 MVP = View->GetPerspMatrix()*View->GetPerspViewMatrix()*GetModelMatrix();
    glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,&MVP[0][0]);
    glUniform4fv(ColorLoc,1,&(GetColor()[0]));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_QUADS,0,Verts.size());
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glUseProgram(0);
}

void NPlayer::Unallocate()
{
    delete (NPlayer*)this;
}

float NPlayer::GetDirection()
{
    return CurrentDirection;
}

bool NPlayer::GetMoving()
{
    return Moving;
}

void NPlayer::SetWantedPosition(glm::vec3 Pos)
{
    HasSetWantedPosition = true;
    WantedPosition = Pos;
}

void NPlayer::SetDirection(float Direction)
{
    CurrentDirection = Direction;
}

void NPlayer::SetMoving(bool i_Moving)
{
    Moving = i_Moving;
}
