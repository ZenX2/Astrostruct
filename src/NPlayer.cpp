#include "NEngine.hpp"

NPlayer::NPlayer()
{
	Shader = GetGame()->GetRender()->GetShader("flat");
	if (Shader)
	{
		TextureLoc = Shader->GetUniformLocation("Texture");
		MatrixLoc = Shader->GetUniformLocation("MVP");
		ColorLoc = Shader->GetUniformLocation("Color");
	}
	Velocity = glm::vec3(0);
	Speed = 200;
	glGenBuffers(2,Buffers);
	Texture = GetGame()->GetRender()->GetTexture("human");
	Changed = true;
	Moving = false;
	CurrentDirection = 0;
	Controlled = false;
	Friction = 2;
	OnGround = false;
	Gravity = 2;
}

NPlayer::~NPlayer()
{
}

void NPlayer::Move(float Direction)
{
	Texture->Play("run");
	CurrentDirection = Direction;
	Moving = true;
}

void NPlayer::StopMove()
{
	Texture->Play("idle");
	Moving = false;
}

void NPlayer::SetControl()
{
	Controlled = true;
}

void NPlayer::Tick(double DT)
{
	if (!GetGame()->GetMap()->Ready)
	{
		return;
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
		if (GetGame()->GetInput()->GetKey(GLFW_KEY_SPACE))
		{
			SetVel(glm::vec3(GetVel().x,GetVel().y,100));
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
	}
	if (Moving && OnGround)
	{
		glm::vec2 Dir = glm::vec2(sin(CurrentDirection),cos(CurrentDirection));
		Dir *= Speed*DT;
		SetVel(GetVel()+glm::vec3(Dir,0));
	}
	//Collisions Init
	glm::vec3 PosMem = GetPos();
	//Collisions
	NMap* Map = GetGame()->GetMap();
	NTile* CTile = Map->GetTile(GetPos());
	if (CTile != NULL)
	{
		if (CTile->IsOpenSpace())
		{
			OnGround = false;
			SetVel(GetVel()-glm::vec3(0,0,Gravity));
		}
	} else {
		OnGround = false;
		SetVel(GetVel()-glm::vec3(0,0,Gravity));
	}
	for (float x=-GetScale().x*32;x<=GetScale().x*32;x+=Map->GetTileSize())
	{
		for (float y=-GetScale().y*32;y<=GetScale().y*32;y+=Map->GetTileSize())
		{
			NTile* Tile = Map->GetTile(GetPos()+glm::vec3(x,y,0));
			if (!Tile || !Tile->IsSolid())
			{
				continue;
			}
			glm::vec3 TilePos = Map->TilePos(GetPos()+glm::vec3(x,y,0));
			if (Intersects(glm::vec4(TilePos.x,TilePos.y,Map->GetTileSize(),Map->GetTileSize()),glm::vec4(GetPos().x,GetPos().y,GetScale().x*32,GetScale().y*32)))
			{
				glm::vec2 Move = MinimumTranslation(glm::vec4(TilePos.x,TilePos.y,Map->GetTileSize(),Map->GetTileSize()),glm::vec4(GetPos().x,GetPos().y,GetScale().x*32,GetScale().y*32))/2.f;
				SetPos(GetPos()+glm::vec3(Move.x,Move.y,0));
				if (Move.y <= 1)
				{
					Move.y = GetVel().y;
				}
				if (Move.x <= 1)
				{
					Move.x = GetVel().x;
				}
				SetVel(glm::vec3(Move.x,Move.y,GetVel().z));
			}
		}
	}
	//Update Position
	glm::vec3 Vel = glm::vec3(Velocity.x*DT,Velocity.y*DT,Velocity.z*DT);
	SetPos(GetPos()+Vel);
	if (OnGround)
	{
		Velocity = glm::vec3(Velocity.x*(1-DT*Friction),Velocity.y*(1-DT*Friction),Velocity.z);
	}
	//Tracer Collisions
	glm::vec3 Normal = glm::normalize(GetPos()-PosMem);
	for (unsigned int i=0;i<glm::length(GetPos()-PosMem);i+=2)
	{
		glm::vec3 Pos = PosMem+(Normal*float(i));
		NMap* Map = GetGame()->GetMap();
		NTile* CTile = Map->GetTile(Pos);
		glm::vec3 CTilePos = Map->TilePos(Pos);
		if (CTile != NULL)
		{
			if (!CTile->IsOpenSpace())
			{
				if (fabs(CTilePos.z-Pos.z)<2)
				{
					OnGround = true;
					SetPos(glm::vec3(GetPos().x,GetPos().y,CTilePos.z+2));
					SetVel(glm::vec3(GetVel().x,GetVel().y,0));
					break; //Break the tracer because we hit something!
				}
			}
		}
	}
	if (!OnGround)
	{
		SetVel(GetVel()-glm::vec3(0,0,Gravity));
	}
	//Misc
	Texture->Tick(DT);
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

glm::vec3 NPlayer::GetVel()
{
	return Velocity;
}

void NPlayer::SetVel(glm::vec3 i_Velocity)
{
	Velocity = i_Velocity;
}

void NPlayer::Remove()
{
	delete (NPlayer*)this;
}
