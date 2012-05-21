#include "NEngine.hpp"

NMap::NMap(std::string i_TileSet)
{
	Shader = GetGame()->GetRender()->GetShader("map");
	Texture = GetGame()->GetRender()->GetTexture(i_TileSet);
	TileSize = Texture->GetFloat("TileSize");
	TextureWidth = Texture->GetSize().x;
	TextureHeight = Texture->GetSize().y;
	Width = Height = Depth = 0;
	if (Shader != NULL)
	{
		TextureLoc = Shader->GetUniformLocation("Texture");
		MatrixLoc = Shader->GetUniformLocation("MVP");
		ColorLoc = Shader->GetUniformLocation("Color");
	}
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
}
void NMap::Init(unsigned int i_Width, unsigned int i_Height, unsigned int i_Depth)
{
	Width = i_Width;
	Height = i_Height;
	Depth = i_Depth;
	Changed.resize(Depth,true);
	Buffers.resize(Depth,NULL);
	std::vector<glm::vec3> Foo2;
	Verts.resize(Depth, Foo2);
	std::vector<glm::vec2> Bar;
	UVs.resize(Depth, Bar);
	for (unsigned int i=0;i<Depth;i++)
	{
		Buffers[i] = new GLuint[2];
		glGenBuffers(2,Buffers[i]);
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
}
void NMap::GenerateBuffers()
{
	for (unsigned int i=0;i<Depth;i++)
	{
		if (!Changed[i])
		{
			continue;
		}
		Verts[i].clear();
		UVs[i].clear();
		for (unsigned int x=0;x<Width;x++)
		{
			for (unsigned int y=0;y<Height;y++)
			{
				float TS = TileSize;
				float X = x*TS;
				float Y = y*TS;
				float Z = i*TS;
				float UTW = TS/(float)TextureWidth;
				float UTH = TS/(float)TextureHeight;
				float UTWS = UTW*(Tiles[x][y][i]->ID%((unsigned int)(TextureWidth/TS)));
				float UTHS = UTH*(Tiles[x][y][i]->ID/((unsigned int)(TextureHeight/TS)));
				Verts[i].push_back(glm::vec3(X,Y,Z));
				UVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
				Verts[i].push_back(glm::vec3(X+TS,Y,Z));
				UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
				Verts[i].push_back(glm::vec3(X+TS,Y+TS,Z));
				UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
				Verts[i].push_back(glm::vec3(X,Y+TS,Z));
				UVs[i].push_back(glm::vec2(UTWS,UTHS));
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][0]);
		glBufferData(GL_ARRAY_BUFFER,Verts[i].size()*sizeof(glm::vec3),&Verts[i][0],GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][1]);
		glBufferData(GL_ARRAY_BUFFER,UVs[i].size()*sizeof(glm::vec2),&UVs[i][0],GL_STATIC_DRAW);
		Changed[i] = false;
	}
}
void NMap::Draw(NCamera* View)
{
	GenerateBuffers();
	if (Shader == NULL)
	{
		glPushMatrix();
		glColor4fv(&(GetColor()[0]));
		glTranslatef(GetPos().x,GetPos().y,0);
		glRotatef(GetAng().z,0,0,1);
		glRotatef(GetAng().y,0,1,0);
		glRotatef(GetAng().x,1,0,0);
		glScalef(GetScale().x,GetScale().y,0);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D,Texture->GetID());
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		for (unsigned int i=0;i<Depth;i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][0]);
			glVertexPointer(3,GL_FLOAT,0,NULL);

			glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][1]);
			glTexCoordPointer(2,GL_FLOAT,0,NULL);

			glDrawArrays(GL_QUADS,0,Verts[i].size());
		}
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		return;
	}
	glUseProgram(Shader->GetID());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Texture->GetID());
	glUniform1i(TextureLoc,0);
	glm::mat4 MVP = View->GetOrthoMatrix()*View->GetViewMatrix()*GetModelMatrix();
	glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,&MVP[0][0]);
	glUniform4fv(ColorLoc,1,&(GetColor()[0]));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	for (unsigned int i=0;i<Depth;i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][0]);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][1]);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
		
		glDrawArrays(GL_QUADS,0,Verts[i].size());
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);
}
void NMap::Tick(double DT)
{
	Texture->Tick(DT);
}
void NMap::Remove()
{
	delete (NMap*)this;
}

NTile::NTile()
{
	ID = rand()%2;
}
NTile::~NTile()
{
}
