#include "NEngine.hpp"

NMap::NMap(std::string i_TileSet)
{
	TileSet = GetGame()->GetRender()->GetTexture(i_TileSet);
	TileSize = Texture->GetFloat("TileSize");
	Width = Height = Depth = 0;
	Changed = true;
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
}
void NMap::Init(unsigned int i_Width, unsigned int i_Height, unsigned int i_Depth)
{
	Width = i_Width;
	Height = i_Height;
	Depth = i_Depth;
	Tiles.resize(Width,std::vector<std::vector<NTile*> Foo);
	for (unsigned int i=0;i<Width;i++)
	{
		Tiles[i].resize(Height,std::vector<NTile*> Foo);
		for (unsigned int o=0;o<Height;o++)
		{
			Tiles[i][o].resize(Depth, NTile* Foo);
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
	if (!Changed)
	{
		return;
	}
	Verts.clear();
	UVs.clear();
	if (BorderSize == 0)
	{
		Verts.push_back(glm::vec2(-.5,-.5));
		UVs.push_back(glm::vec2(0,1));
		Verts.push_back(glm::vec2(.5,-.5));
		UVs.push_back(glm::vec2(1,1));
		Verts.push_back(glm::vec2(.5,.5));
		UVs.push_back(glm::vec2(1,0));
		Verts.push_back(glm::vec2(-.5,.5));
		UVs.push_back(glm::vec2(0,0));
	} else {
		float SY = BorderSize/GetScale().y;
		float SX = BorderSize/GetScale().x;
		float UX = BorderSize/Texture->GetSize().x;
		float UY = BorderSize/Texture->GetSize().y;
		//Top Left Corner
		Verts.push_back(glm::vec2(-.5,.5));
		UVs.push_back(glm::vec2(0,0));
		Verts.push_back(glm::vec2(-.5+SX,.5));
		UVs.push_back(glm::vec2(UX,0));
		Verts.push_back(glm::vec2(-.5+SX,.5-SY));
		UVs.push_back(glm::vec2(UX,UY));
		Verts.push_back(glm::vec2(-.5,.5-SY));
		UVs.push_back(glm::vec2(0,UY));
		//Top
		Verts.push_back(glm::vec2(-.5+SX,.5));
		UVs.push_back(glm::vec2(UX,0));
		Verts.push_back(glm::vec2(.5-SX,.5));
		UVs.push_back(glm::vec2(1-UX,0));
		Verts.push_back(glm::vec2(.5-SX,.5-SY));
		UVs.push_back(glm::vec2(1-UX,UY));
		Verts.push_back(glm::vec2(-.5+SX,.5-SY));
		UVs.push_back(glm::vec2(UX,UY));
		//Top Right Corner
		Verts.push_back(glm::vec2(.5,.5));
		UVs.push_back(glm::vec2(1,0));
		Verts.push_back(glm::vec2(.5-SX,.5));
		UVs.push_back(glm::vec2(1-UX,0));
		Verts.push_back(glm::vec2(.5-SX,.5-SY));
		UVs.push_back(glm::vec2(1-UX,UY));
		Verts.push_back(glm::vec2(.5,.5-SY));
		UVs.push_back(glm::vec2(1,UY));
		//Left
		Verts.push_back(glm::vec2(-.5,.5-SY));
		UVs.push_back(glm::vec2(0,UY));
		Verts.push_back(glm::vec2(-.5+SX,.5-SY));
		UVs.push_back(glm::vec2(UX,UY));
		Verts.push_back(glm::vec2(-.5+SX,-.5+SY));
		UVs.push_back(glm::vec2(UX,1-UY));
		Verts.push_back(glm::vec2(-.5,-.5+SY));
		UVs.push_back(glm::vec2(0,1-UY));
		//Middle
		Verts.push_back(glm::vec2(-.5+SX,.5-SY));
		UVs.push_back(glm::vec2(UX,UY));
		Verts.push_back(glm::vec2(.5-SX,.5-SY));
		UVs.push_back(glm::vec2(1-UX,UY));
		Verts.push_back(glm::vec2(.5-SX,-.5+SY));
		UVs.push_back(glm::vec2(1-UX,1-UY));
		Verts.push_back(glm::vec2(-.5+SX,-.5+SY));
		UVs.push_back(glm::vec2(UX,1-UY));
		//Right
		Verts.push_back(glm::vec2(.5-SX,.5-SY));
		UVs.push_back(glm::vec2(1-UX,UY));
		Verts.push_back(glm::vec2(.5,.5-SY));
		UVs.push_back(glm::vec2(1,UY));
		Verts.push_back(glm::vec2(.5,-.5+SY));
		UVs.push_back(glm::vec2(1,1-UY));
		Verts.push_back(glm::vec2(.5-SX,-.5+SY));
		UVs.push_back(glm::vec2(1-UX,1-UY));
		//Bottom Left Corner
		Verts.push_back(glm::vec2(-.5,-.5+SY));
		UVs.push_back(glm::vec2(0,1-UY));
		Verts.push_back(glm::vec2(-.5+SX,-.5+SY));
		UVs.push_back(glm::vec2(UX,1-UY));
		Verts.push_back(glm::vec2(-.5+SX,-.5));
		UVs.push_back(glm::vec2(UX,1));
		Verts.push_back(glm::vec2(-.5,-.5));
		UVs.push_back(glm::vec2(0,1));
		//Bottom
		Verts.push_back(glm::vec2(-.5+SX,-.5+SY));
		UVs.push_back(glm::vec2(UX,1-UY));
		Verts.push_back(glm::vec2(.5-SX,-.5+SY));
		UVs.push_back(glm::vec2(1-UX,1-UY));
		Verts.push_back(glm::vec2(.5-SX,-.5));
		UVs.push_back(glm::vec2(1-UX,1));
		Verts.push_back(glm::vec2(-.5+SX,-.5));
		UVs.push_back(glm::vec2(UX,1));
		//Bottom Right Corner
		Verts.push_back(glm::vec2(.5-SX,-.5+SY));
		UVs.push_back(glm::vec2(1-UX,1-UY));
		Verts.push_back(glm::vec2(.5,-.5+SY));
		UVs.push_back(glm::vec2(1,1-UY));
		Verts.push_back(glm::vec2(.5,-.5));
		UVs.push_back(glm::vec2(1,1));
		Verts.push_back(glm::vec2(.5-SX,-.5));
		UVs.push_back(glm::vec2(1-UX,1));
	}
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
	Changed = false;
	SizeMem = GetScale();
}
}

NTile::NTile()
{
	ID = rand()%2;
}
NTile::~NTile()
{
}
