#include "NEngine.hpp"

NLight::NLight(std::string i_Texture) : NNode(NodeLight)
{
    Changed = true;
    PositionMemory = glm::vec3(0,0,0);
    ScaleMemory = glm::vec3(0,0,0);
    Texture = GetGame()->GetRender()->GetTexture(i_Texture);
    glGenBuffers(3,Buffers);
    Shader = GetGame()->GetRender()->GetShader("flat3D");
    if (Shader != NULL)
    {
        MatrixLoc = Shader->GetUniformLocation("Model");
        TextureLoc = Shader->GetUniformLocation("Texture");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    ShadowShader = GetGame()->GetRender()->GetShader("normal_textureless");
    if (ShadowShader != NULL)
    {
        SColorLoc = Shader->GetUniformLocation("Color");
    }
}

NLight::~NLight()
{
    if (Texture)
    {
        delete Texture;
    }
    glDeleteBuffers(3,Buffers);
}

void NLight::GenerateLightBuffers()
{
    if (!Texture)
    {
        return;
    }
    if (!Changed)
    {
        return;
    }
    Verts.clear();
    UVs.clear();
    Verts.push_back(glm::vec2(-.5,-.5));
    UVs.push_back(glm::vec2(0,1));
    Verts.push_back(glm::vec2(.5,-.5));
    UVs.push_back(glm::vec2(1,1));
    Verts.push_back(glm::vec2(.5,.5));
    UVs.push_back(glm::vec2(1,0));
    Verts.push_back(glm::vec2(-.5,.5));
    UVs.push_back(glm::vec2(0,0));
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
    Changed = false;
}

void NLight::GenerateShadowBuffers()
{
    //oh boy why didn't i comment this while i coded it; it's pretty damn complicated. I'll try my best to reinterpret and explain what i was thinking.
    if (!Texture)
    {
        return;
    }
    //Compares it's current position and scale from memory, if it hasn't changed from last generation then don't regenerate it!
    if (PositionMemory == GetRealPos() && ScaleMemory == GetScale())
    {
        return;
    }
    glm::vec3 Pos = GetRealPos();
    Shadows.clear();
    //Figures out what size of a box of map tiles we should loop through. FIXME: Make it more accurately decide which map tiles to loop through.
    float Max = std::max(GetScale().x, GetScale().y);
    //For every map tile in a massive area around the light...
    for (float x = -Max;x<Max;x+=GetGame()->GetMap()->GetTileSize())
    {
        for (float y = -Max;y<Max;y+=GetGame()->GetMap()->GetTileSize())
        {
            //Get the tile object at the current loop position
            NTile* Tile = GetGame()->GetMap()->GetTile(Pos+glm::vec3(x,y,0));
            //If the tile doesn't block light or doesn't exist, skip it!
            if (Tile == NULL || !Tile->IsOpaque())
            {
                continue;
            }
            float TS = GetGame()->GetMap()->GetTileSize()/2.f;
            //Generate points in the four corners of the tile.
            glm::vec3 TPos = GetGame()->GetMap()->TilePos(Pos+glm::vec3(x,y,0));
            glm::vec3 Points[4];
            Points[0] = TPos+glm::vec3(TS,TS,0);
            Points[1] = TPos+glm::vec3(TS,-TS,0);
            Points[2] = TPos+glm::vec3(-TS,-TS,0);
            Points[3] = TPos+glm::vec3(-TS,TS,0);
            std::vector<glm::vec4> Faces;
            //Generate faces (two points = face, each tile has 4 faces)
            NTile* CheckTile = GetGame()->GetMap()->GetTile(Tile->X,Tile->Y-1,Tile->Z);
            if ((CheckTile && !CheckTile->IsOpaque()) || !CheckTile)
            {
                Faces.push_back(glm::vec4(Points[1].x,Points[1].y,Points[2].x,Points[2].y));
            }
            CheckTile = GetGame()->GetMap()->GetTile(Tile->X,Tile->Y+1,Tile->Z);
            if ((CheckTile && !CheckTile->IsOpaque()) || !CheckTile)
            {
                Faces.push_back(glm::vec4(Points[3].x,Points[3].y,Points[0].x,Points[0].y));
            }
            CheckTile = GetGame()->GetMap()->GetTile(Tile->X-1,Tile->Y,Tile->Z);
            if ((CheckTile && !CheckTile->IsOpaque()) || !CheckTile)
            {
                Faces.push_back(glm::vec4(Points[2].x,Points[2].y,Points[3].x,Points[3].y));
            }
            CheckTile = GetGame()->GetMap()->GetTile(Tile->X+1,Tile->Y,Tile->Z);
            if ((CheckTile && !CheckTile->IsOpaque()) || !CheckTile)
            {
                Faces.push_back(glm::vec4(Points[0].x,Points[0].y,Points[1].x,Points[1].y));
            }
            for (unsigned int i=0;i<Faces.size();i++)
            {
                //Remove front faces
                if (!Facing(glm::vec2(Pos.x,Pos.y),Faces[i]))
                {
                    continue;
                }
                //Generate shadow mesh by extruding back faces.
                float Radians = -atan2(Pos.x-Faces[i].x,Pos.y-Faces[i].y)-PI/2.f;
                float BRadians = -atan2(Pos.x-Faces[i].z,Pos.y-Faces[i].w)-PI/2.f;
                Shadows.push_back(glm::vec3(Faces[i].x,Faces[i].y,TPos.z));
                Shadows.push_back(glm::vec3(Faces[i].z,Faces[i].w,TPos.z));
                Shadows.push_back(glm::vec3(Faces[i].z+cos(BRadians)*GetScale().x,Faces[i].w+sin(BRadians)*GetScale().y,TPos.z));
                Shadows.push_back(glm::vec3(Faces[i].x+cos(Radians)*GetScale().x,Faces[i].y+sin(Radians)*GetScale().y,TPos.z));
            }
        }
    }

    //3d lighting time, honestly the 2d doesn't work when there's any openings between floors.
    /*for (float x = -Max;x<Max;x+=GetGame()->GetMap()->GetTileSize())
    {
        for (float y = -Max;y<Max;y+=GetGame()->GetMap()->GetTileSize())
        {
            for (float z = -Max;z<Max;z+=GetGame()->GetMap()->GetTileSize())
            {
                //Get the tile object at the current loop position
                NTile* Tile = GetGame()->GetMap()->GetTile(Pos+glm::vec3(x,y,z));
                //If the tile doesn't block light or doesn't exist, skip it!
                if (Tile == NULL || !Tile->IsOpaque())
                {
                    continue;
                }
                float TS = GetGame()->GetMap()->GetTileSize()/2.f;
                //Generate points in the eight verts of the tile.
                glm::vec3 TPos = GetGame()->GetMap()->TilePos(Pos+glm::vec3(x,y,z));
                glm::vec3 Points[8];
                Points[0] = TPos+glm::vec3(TS,TS,0);
                Points[1] = TPos+glm::vec3(TS,-TS,0);
                Points[2] = TPos+glm::vec3(-TS,-TS,0);
                Points[3] = TPos+glm::vec3(-TS,TS,0);
                Points[4] = TPos+glm::vec3(TS,TS,TS*2.f);
                Points[5] = TPos+glm::vec3(TS,-TS,TS*2.f);
                Points[6] = TPos+glm::vec3(-TS,-TS,TS*2.f);
                Points[7] = TPos+glm::vec3(-TS,TS,TS*2.f);
                std::vector< std::vector<glm::vec3> > Faces;
                //Generate faces (four points = face, each tile has 6 faces)
                NTile* CheckTile = GetGame()->GetMap()->GetTile(Tile->X,Tile->Y-1,Tile->Z);
                if ((CheckTile && !CheckTile->IsOpaque()) || !CheckTile)
                {
                    //Faces.push_back(glm::vec4(Points[1].x,Points[1].y,Points[2].x,Points[2].y));
                    std::vector<glm::vec3> Face;
                    Face.push_back(Points[1]);
                    Face.push_back(Points[2]);
                    Face.push_back(Points[6]);
                    Face.push_back(Points[5]);
                    Faces.push_back(Face);
                }
                CheckTile = GetGame()->GetMap()->GetTile(Tile->X,Tile->Y+1,Tile->Z);
                if ((CheckTile && !CheckTile->IsOpaque()) || !CheckTile)
                {
                    //Faces.push_back(glm::vec4(Points[3].x,Points[3].y,Points[0].x,Points[0].y));
                    std::vector<glm::vec3> Face;
                    Face.push_back(Points[3]);
                    Face.push_back(Points[0]);
                    Face.push_back(Points[4]);
                    Face.push_back(Points[7]);
                    Faces.push_back(Face);
                }
                CheckTile = GetGame()->GetMap()->GetTile(Tile->X-1,Tile->Y,Tile->Z);
                if ((CheckTile && !CheckTile->IsOpaque()) || !CheckTile)
                {
                    //Faces.push_back(glm::vec4(Points[2].x,Points[2].y,Points[3].x,Points[3].y));
                    std::vector<glm::vec3> Face;
                    Face.push_back(Points[2]);
                    Face.push_back(Points[3]);
                    Face.push_back(Points[7]);
                    Face.push_back(Points[6]);
                    Faces.push_back(Face);
                }
                CheckTile = GetGame()->GetMap()->GetTile(Tile->X+1,Tile->Y,Tile->Z);
                if ((CheckTile && !CheckTile->IsOpaque()) || !CheckTile)
                {
                    //Faces.push_back(glm::vec4(Points[0].x,Points[0].y,Points[1].x,Points[1].y));
                    std::vector<glm::vec3> Face;
                    Face.push_back(Points[0]);
                    Face.push_back(Points[1]);
                    Face.push_back(Points[5]);
                    Face.push_back(Points[4]);
                    Faces.push_back(Face);
                }
                CheckTile = GetGame()->GetMap()->GetTile(Tile->X,Tile->Y,Tile->Z-1);
                if ((CheckTile && !CheckTile->IsOpaque()) || !CheckTile)
                {
                    std::vector<glm::vec3> Face;
                    Face.push_back(Points[0]);
                    Face.push_back(Points[1]);
                    Face.push_back(Points[3]);
                    Face.push_back(Points[2]);
                    Faces.push_back(Face);
                }
                CheckTile = GetGame()->GetMap()->GetTile(Tile->X,Tile->Y,Tile->Z+1);
                if ((CheckTile && !CheckTile->IsOpaque()) || !CheckTile)
                {
                    std::vector<glm::vec3> Face;
                    Face.push_back(Points[4]);
                    Face.push_back(Points[5]);
                    Face.push_back(Points[7]);
                    Face.push_back(Points[6]);
                    Faces.push_back(Face);
                }
                for (unsigned int i=0;i<Faces.size();i++)
                {
                    //Remove front faces
                    if (Facing(glm::vec3(Pos.x,Pos.y,Pos.z),Faces[i]))
                    {
                        continue;
                    }
                    //Generate shadow mesh by extruding back faces.
                    //float Radians = -atan2(Pos.x-Faces[i].x,Pos.y-Faces[i].y)-PI/2.f;
                    //float BRadians = -atan2(Pos.x-Faces[i].z,Pos.y-Faces[i].w)-PI/2.f;
                    glm::vec3 ExtrudeA = glm::normalize(Faces[i][0]-Pos);
                    glm::vec3 ExtrudeB = glm::normalize(Faces[i][1]-Pos);
                    Shadows.push_back(Faces[i][0]);
                    Shadows.push_back(Faces[i][1]);
                    Shadows.push_back(Faces[i][0]+ExtrudeA*GetScale()); //these go very far away so shadows are long
                    Shadows.push_back(Faces[i][1]+ExtrudeB*GetScale());

                    glm::vec3 ExtrudeA = glm::normalize(Faces[i][1]-Pos);
                    glm::vec3 ExtrudeB = glm::normalize(Faces[i][2]-Pos);
                    Shadows.push_back(Faces[i][1]);
                    Shadows.push_back(Faces[i][2]);
                    Shadows.push_back(Faces[i][1]+ExtrudeA*GetScale());
                    Shadows.push_back(Faces[i][2]+ExtrudeB*GetScale());

                    ExtrudeA = glm::normalize(Faces[i][2]-Pos);
                    ExtrudeB = glm::normalize(Faces[i][3]-Pos);
                    Shadows.push_back(Faces[i][2]);
                    Shadows.push_back(Faces[i][3]);
                    Shadows.push_back(Faces[i][2]+ExtrudeA*GetScale());
                    Shadows.push_back(Faces[i][3]+ExtrudeB*GetScale());

                    ExtrudeA = glm::normalize(Faces[i][3]-Pos);
                    ExtrudeB = glm::normalize(Faces[i][0]-Pos);
                    Shadows.push_back(Faces[i][3]);
                    Shadows.push_back(Faces[i][0]);
                    Shadows.push_back(Faces[i][3]+ExtrudeA*GetScale());
                    Shadows.push_back(Faces[i][0]+ExtrudeB*GetScale());
                }
            }
        }
    }*/

    glBindBuffer(GL_ARRAY_BUFFER,Buffers[2]);
    glBufferData(GL_ARRAY_BUFFER,Shadows.size()*sizeof(glm::vec3),&Shadows[0],GL_STATIC_DRAW);
    PositionMemory = GetRealPos();
    ScaleMemory = GetScale();
}

void NLight::Draw(NCamera* View)
{
    //If the light isn't on the current level of the map, don't draw it! This is required because 2d lighting!
    if (GetGame()->GetMap()->GetLevel() != GetGame()->GetMap()->GetLevel(GetRealPos()))
    {
        return;
    }
    //Clear out the stencil buffer with 0's so we have a clean slate to work with.
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    //Make it so whatever we draw replaces everything it touches in the stencil to 1.
    glStencilFunc(GL_ALWAYS,0x1,0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //Draw all our shadow volumes.
    DrawShadow(View);
    //Now we make it so we can only draw on 0's, we also don't want to replace anything in the stencil buffer so we lock it up.
    glStencilFunc(GL_EQUAL,0x0,0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    //Finally draw the light into whatever's not shadow.
    DrawLight(View);
    glDisable(GL_STENCIL_TEST);
}
void NLight::DrawLight(NCamera* View)
{
    //Pretty standard drawing procedures, should make sense for most people.
    GenerateLightBuffers();
    if (Texture == NULL || GetColor().w == 0 || Shader == NULL)
    {
        return;
    }
    glUseProgram(Shader->GetID());
    glActiveTexture(GL_TEXTURE0);
    if (Texture != NULL)
    {
        glBindTexture(GL_TEXTURE_2D,Texture->GetID());
    }
    glUniform1i(TextureLoc,0);
    glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,glm::value_ptr(GetModelMatrix()));
    glUniform4fv(ColorLoc,1,&(GetColor()[0]));
    glEnableVertexAttribArray(Shader->GetVertexAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glVertexAttribPointer(Shader->GetVertexAttribute(),2,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(Shader->GetUVAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glVertexAttribPointer(Shader->GetUVAttribute(),2,GL_FLOAT,GL_FALSE,0,NULL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_QUADS,0,Verts.size());
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(Shader->GetVertexAttribute());
    glDisableVertexAttribArray(Shader->GetUVAttribute());
    glUseProgram(0);
}
void NLight::DrawShadow(NCamera* View)
{
    //asdlkfhwler
    GenerateShadowBuffers();
    if (Texture == NULL || GetColor().w == 0 || ShadowShader == NULL)
    {
        return;
    }
    glUseProgram(ShadowShader->GetID());
    glUniform4f(SColorLoc,0,0,0,0);
    glEnableVertexAttribArray(ShadowShader->GetVertexAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[2]);
    glVertexAttribPointer(ShadowShader->GetVertexAttribute(),3,GL_FLOAT,GL_FALSE,0,NULL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_QUADS,0,Shadows.size());
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(ShadowShader->GetVertexAttribute());
    glUseProgram(0);
}

NLightSystem::NLightSystem()
{
    FrameBuffer = new NFramebuffer(NFramebufferFlags(NColorBuffer | NStencilBuffer), GetGame()->GetWindowWidth(), GetGame()->GetWindowHeight());
    //Generate screen quad
    Shader = GetGame()->GetRender()->GetShader("flat_colorless");
    if (Shader != NULL)
    {
        TextureLoc = Shader->GetUniformLocation("Texture");
    }
    glGenBuffers(2,VertexBuffers);
    Verts.clear();
    UVs.clear();
    Verts.push_back(glm::vec2(-1,-1));
    UVs.push_back(glm::vec2(0,0));
    Verts.push_back(glm::vec2(1,-1));
    UVs.push_back(glm::vec2(1,0));
    Verts.push_back(glm::vec2(1,1));
    UVs.push_back(glm::vec2(1,1));
    Verts.push_back(glm::vec2(-1,1));
    UVs.push_back(glm::vec2(0,1));
    glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
}

NLightSystem::~NLightSystem()
{
    delete FrameBuffer;
    glDeleteBuffers(2,VertexBuffers);
}

NFramebuffer* NLightSystem::GetFramebuffer()
{
    FrameBuffer->Resize(GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight());
    return FrameBuffer;
}

void NLightSystem::Draw()
{
    if (Shader == NULL)
    {
        return;
    }
    glEnable(GL_TEXTURE_2D);
    glUseProgram(Shader->GetID());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,FrameBuffer->GetTexture());
    glUniform1i(TextureLoc,0);
    glEnableVertexAttribArray(Shader->GetVertexAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[0]);
    glVertexAttribPointer(Shader->GetVertexAttribute(),2,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(Shader->GetUVAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[1]);
    glVertexAttribPointer(Shader->GetUVAttribute(),2,GL_FLOAT,GL_FALSE,0,NULL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_QUADS,0,Verts.size());
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(Shader->GetVertexAttribute());
    glDisableVertexAttribArray(Shader->GetUVAttribute());
    glUseProgram(0);
}

void NLight::Unallocate()
{
    delete (NLight*)this;
}

void NLight::Tick(double DT)
{
}
void NLight::UpdateShadows()
{
    PositionMemory += glm::vec3(0,1,0);
}
