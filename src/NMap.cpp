#include "NEngine.hpp"

NMap::NMap(std::string i_TileSet)
{
	Ready = false;
	Shader = GetGame()->GetRender()->GetShader("map");
	OutlineShader = GetGame()->GetRender()->GetShader("normal");
	Texture = GetGame()->GetRender()->GetTexture(i_TileSet);
	if (Texture)
	{
		TileSize = Texture->GetFloat("TileSize");
		TextureWidth = Texture->GetSize().x;
		TextureHeight = Texture->GetSize().y;
	}
	Width = Height = Depth = 0;
	if (Shader != NULL)
	{
		TextureLoc = Shader->GetUniformLocation("Texture");
		MatrixLoc = Shader->GetUniformLocation("MVP");
		ColorLoc = Shader->GetUniformLocation("Color");
	}
	if (OutlineShader != NULL)
	{
		OutlineColorLoc = OutlineShader->GetUniformLocation("Color");
		OutlineMatrixLoc = OutlineShader->GetUniformLocation("MVP");
	}
	ViewingLevel = 1;
}
unsigned int NMap::GetDepth()
{
	return Depth;
}
NMap::~NMap()
{
	for (unsigned int x=0;x<Width;x++)
	{
		for (unsigned int y=0;y<Height;y++)
		{
			for (unsigned int z=0;z<Depth;z++)
			{
				delete Tiles[x][y][z];
			}
		}
	}
	for (unsigned int i=0;i<Depth;i++)
	{
		delete[] Buffers[i];
	}
	if (Texture)
	{
		delete Texture;
	}
}
void NMap::Init(unsigned int i_Width, unsigned int i_Height, unsigned int i_Depth)
{
	Width = i_Width;
	Height = i_Height;
	Depth = i_Depth;
	Changed.clear();
	Buffers.clear();
	Verts.clear();
	UVs.clear();
	Outline.clear();
	Tiles.clear();
	Changed.resize(Depth,true);
	Buffers.resize(Depth,NULL);
	std::vector<glm::vec3> Foo2;
	Verts.resize(Depth, Foo2);
	std::vector<glm::vec2> Bar;
	UVs.resize(Depth, Bar);
	Outline.resize(Depth,Foo2);
	for (unsigned int i=0;i<Depth;i++)
	{
		Buffers[i] = new GLuint[3];
		glGenBuffers(3,Buffers[i]);
	}
	std::vector<std::vector<NTile* > > Foo;
	Tiles.resize(Width,Foo);
	for (unsigned int i=0;i<Width;i++)
	{
		std::vector<NTile* > Bar;
		Tiles[i].resize(Height,Bar);
		for (unsigned int o=0;o<Height;o++)
		{
			NTile* Asdf;
			Tiles[i][o].resize(Depth, Asdf);
		}
	}
	for (unsigned int x=0;x<Width;x++)
	{
		for (unsigned int y=0;y<Height;y++)
		{
			for (unsigned int z=0;z<Depth;z++)
			{
				Tiles[x][y][z] = new NTile();
			}
		}
	}
	Ready = true;
}
void NMap::ViewLevel(int Level)
{
	if (Level > Depth - 1)
	{
		ViewingLevel = Depth-1;
		return;
	} else if (Level < 1)
	{
		ViewingLevel = 1;
		return;
	}
	ViewingLevel = Level;
}
int NMap::GetLevel()
{
	return ViewingLevel;
}
float NMap::GetTileSize()
{
	return TileSize;
}
void NMap::GenerateBuffers()
{
	if (!Texture || !Ready)
	{
		return;
	}
	for (unsigned int i=ViewingLevel;i>0;i--)
	{
		if (!Changed[i])
		{
			continue;
		}
		Verts[i].clear();
		UVs[i].clear();
		Outline[i].clear();
		for (unsigned int x=0;x<Width;x++)
		{
			for (unsigned int y=0;y<Height;y++)
			{
				unsigned int ID = Tiles[x][y][i]->ID;
				if (ID == 0)
				{
					continue;
				}
				float TS = TileSize;
				float X = x*TS;
				float Y = y*TS;
				float Z = i*TS;
				float UTW = TS/(float)TextureWidth;
				float UTH = TS/(float)TextureHeight;
				float UTWS = UTW*((ID-1)%((unsigned int)(TextureWidth/TS)));
				float UTHS = UTH*((ID-1)/((unsigned int)(TextureHeight/TS)));
				Verts[i].push_back(glm::vec3(X,Y,Z));
				UVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
				Verts[i].push_back(glm::vec3(X+TS,Y,Z));
				UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
				Verts[i].push_back(glm::vec3(X+TS,Y+TS,Z));
				UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
				Verts[i].push_back(glm::vec3(X,Y+TS,Z));
				UVs[i].push_back(glm::vec2(UTWS,UTHS));

				if (!Tiles[x][y][i]->IsSolid()) //If we are open-space or a solid object, outline it!
				{
					continue;
				}
				int left = int(x)-1;
				int right = x+1;
				int bottom = int(y)-1;
				int top = y+1;
				if (left < 0 || (!Tiles[left][y][i]->IsSolid()))
				{
					//outline left
					Outline[i].push_back(glm::vec3(X,Y,Z));
					Outline[i].push_back(glm::vec3(X,Y+TS,Z));
				}
				if (right >= Width || (!Tiles[right][y][i]->IsSolid()))
				{
					//outline right
					Outline[i].push_back(glm::vec3(X+TS,Y,Z));
					Outline[i].push_back(glm::vec3(X+TS,Y+TS,Z));
				}
				if (bottom < 0 || (!Tiles[x][bottom][i]->IsSolid()))
				{
					//outline bottom
					Outline[i].push_back(glm::vec3(X,Y,Z));
					Outline[i].push_back(glm::vec3(X+TS,Y,Z));
				}
				if (top >= Height || (!Tiles[x][top][i]->IsSolid()))
				{
					//outline top
					Outline[i].push_back(glm::vec3(X,Y+TS,Z));
					Outline[i].push_back(glm::vec3(X+TS,Y+TS,Z));
				}
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][0]);
		glBufferData(GL_ARRAY_BUFFER,Verts[i].size()*sizeof(glm::vec3),&Verts[i][0],GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][1]);
		glBufferData(GL_ARRAY_BUFFER,UVs[i].size()*sizeof(glm::vec2),&UVs[i][0],GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][2]);
		glBufferData(GL_ARRAY_BUFFER,Outline[i].size()*sizeof(glm::vec3),&Outline[i][0],GL_STATIC_DRAW);
		Changed[i] = false;
	}
}
void NMap::Draw(NCamera* View)
{
	if (!Texture || !Ready)
	{
		return;
	}
	GenerateBuffers();
	if (Shader == NULL)
	{

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&View->GetPerspMatrix()[0][0]);
		glMatrixMode(GL_MODELVIEW);
		glm::mat4 MVP = View->GetPerspViewMatrix()*GetModelMatrix();
		glLoadMatrixf(&MVP[0][0]);

		glColor4fv(&(GetColor()[0]));
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D,Texture->GetID());
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		int Check = ViewingLevel - 2;
		if (Check < 0)
		{
			Check = 0;
		}
		for (unsigned int i=Check;i<=ViewingLevel;i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][0]);
			glVertexPointer(3,GL_FLOAT,0,NULL);

			glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][1]);
			glTexCoordPointer(2,GL_FLOAT,0,NULL);

			glDrawArrays(GL_QUADS,0,Verts[i].size());
		}
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glColor4f(0,0,0,1);
		for (unsigned int i=0;i<=ViewingLevel;i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][2]);
			glVertexPointer(3,GL_FLOAT,0,NULL);
			
			glDrawArrays(GL_LINES,0,Outline[i].size());
		}
		glDisableClientState(GL_VERTEX_ARRAY);
		return;
	}
	glUseProgram(Shader->GetID());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Texture->GetID());
	glUniform1i(TextureLoc,0);
	glm::mat4 MVP = View->GetPerspMatrix()*View->GetPerspViewMatrix()*GetModelMatrix();
	glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,&MVP[0][0]);
	glUniform4fv(ColorLoc,1,&(GetColor()[0]));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	int Check = ViewingLevel - 2;
	if (Check < 0)
	{
		Check = 0;
	}
	for (unsigned int i=Check;i<=ViewingLevel;i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][0]);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][1]);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
		
		glDrawArrays(GL_QUADS,0,Verts[i].size());
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(1);

	glUseProgram(OutlineShader->GetID());
	glUniform4f(OutlineColorLoc,0,0,0,1);
	glUniformMatrix4fv(OutlineMatrixLoc,1,GL_FALSE,&MVP[0][0]);
	glLineWidth(3);
	for (unsigned int i=0;i<=ViewingLevel;i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][2]);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
		
		glDrawArrays(GL_LINES,0,Outline[i].size());
	}
	glDisableVertexAttribArray(0);
}
void NMap::Tick(double DT)
{
	if (Texture)
	{
		Texture->Tick(DT);
	}
}
void NMap::Remove()
{
	delete (NMap*)this;
}

NTile::NTile()
{
	ID = rand()%3;
}
NTile::~NTile()
{
}

bool NTile::IsSolid()
{
	if (ID == 2)
	{
		return true;
	}
	return false;
}

bool NTile::IsOpenSpace()
{
	if (!ID)
	{
		return true;
	}
	return false;
}
