#include "NEngine.hpp"

void LoadTextures()
{
	lua_State* L = GetGame()->GetLua()->GetL();
	static const luaL_Reg BaseFunctions[] = {
		{"Animation",CreateAnimation},
		{"LoadTexture",LoadTexture},
		{NULL,NULL}
	};
	lua_getglobal(L,"_G");
	luaL_register(L,NULL,BaseFunctions);
	lua_pop(L,1);
	luaL_getmetatable(L,"AnimationBase");
	if (lua_isnoneornil(L,-1))
	{
		lua_pop(L,1);
		luaL_newmetatable(L,"AnimationBase");
	}
	static const luaL_Reg AnimationMethods[] = {
		{"__index",Animation__index},
		{"__newindex",Animation__newindex},
		{NULL,NULL}
	};
	luaL_register(L,NULL,AnimationMethods);
	lua_pushstring(L,"Animation");
	lua_setfield(L,-2,"__type");
	lua_pop(L,1);
	GetGame()->GetLua()->DoFile("data/textures/runner/runner.lua");
}

NAnimation* lua_toAnimation(lua_State* L, int index)
{
	NAnimation** Animation = (NAnimation**)luaL_checkudata(L,index,"AnimationBase");
	return *Animation;
}

NAnimation* lua_checkAnimation(lua_State* L, int narg)
{
	NAnimation* Foo = lua_toAnimation(L,narg);
	if (Foo == NULL)
	{
		luaL_argerror(L,narg,"attempt to index a NULL Animation");
	}
	return Foo;
}

void lua_pushAnimation(lua_State* L, NAnimation* Animation)
{
	NAnimation** Pointer = (NAnimation**)lua_newuserdata(L,sizeof(NAnimation*));
	*Pointer = Animation;
	luaL_getmetatable(L,"AnimationBase");
	lua_setmetatable(L,-2);
}

int CreateAnimation(lua_State* L)
{
	unsigned int ArgCount = lua_gettop(L);
	if (ArgCount == 0)
	{
		return 0;
	}
	NAnimation* Animation = new NAnimation();
	for (unsigned int i=1;i<1+ArgCount;i++)
	{
		const char* Field = luaL_checkstring(L,i);
		if (Field != NULL)
		{
			Animation->AddFrame(Field);
		}
	}
	lua_newtable(L);
	Animation->Reference = luaL_ref(L,LUA_REGISTRYINDEX);
	lua_pushAnimation(L,Animation);
	return 1;
}

int Animation__index(lua_State* L)
{
	NAnimation* Animation = lua_toAnimation(L,1);
	if (Animation == NULL)
	{
		lua_Debug ar1;
		lua_getstack(L,1,&ar1);
		lua_getinfo(L,"fl",&ar1);
		lua_Debug ar2;
		lua_getinfo(L,">S",&ar2);
		lua_pushfstring(L, "%s:%d: attempt to index a NULL Animation", ar2.short_src, ar1.currentline);
		return lua_error(L);
	}
	std::string Field = luaL_checkstring(L,2);
	if (Field == "FPS")
	{
		lua_pushnumber(L,Animation->FPS);
	} else if (Field == "Name")
	{
		lua_pushstring(L,Animation->GetName().c_str());
	} else {
		lua_getmetatable(L,1);
		lua_pushvalue(L,2);
		lua_gettable(L,-2);
		if (lua_isnil(L,-1))
		{
			lua_pop(L,1);
			lua_getref(L,Animation->Reference);
			lua_pushvalue(L,2);
			lua_gettable(L,-2);
		}
	}
	return 1;
}

int Animation__newindex(lua_State* L)
{
	NAnimation* Animation = lua_toAnimation(L,1);
	if (Animation == NULL)
	{
		lua_Debug ar1;
		lua_getstack(L,1,&ar1);
		lua_getinfo(L,"fl",&ar1);
		lua_Debug ar2;
		lua_getinfo(L,">S",&ar2);
		lua_pushfstring(L, "%s:%d: attempt to index a NULL Animation", ar2.short_src, ar1.currentline);
		return lua_error(L);
	}
	const char* Field = luaL_checkstring(L,2);
	if (!strcmp(Field,"FPS"))
	{
		Animation->FPS = luaL_checknumber(L,3);
	} else if (!strcmp(Field,"Name"))
	{
		Animation->SetName(luaL_checkstring(L,3));
	} else {
		lua_getref(L,Animation->Reference);
		lua_pushvalue(L,3);
		lua_setfield(L,-2,luaL_checkstring(L,2));
	}
	return 0;
}

int LoadTexture(lua_State* L)
{
	unsigned int ArgCount = lua_gettop(L);
	NTexture* Texture = new NTexture(luaL_checkstring(L,1));
	for (unsigned int i=2;i<1+ArgCount;i++)
	{
		Texture->AddAnimation(lua_checkAnimation(L,i));
	}
	GetGame()->GetRender()->AddTexture(Texture);
	return 0;
}
	
void NAnimation::SetName(std::string i_Name)
{
	Name = i_Name;
}

std::string NAnimation::GetName()
{
	return Name;
}

NAnimation::NAnimation()
{
	Reference = 0;
	FPS = 30;
	Name = "NULL";
}

NAnimation::~NAnimation()
{
}

void NAnimation::AddFrame(std::string FileName)
{
	//Probably need to do some error checking here...
	GLuint ID = GetGame()->GetRender()->GetCachedTexture(FileName);
	Frames.push_back(ID);
}

NTexture::NTexture(std::string i_Name)
{
	Name = i_Name;
	PlayingAnimation = 0;
	TimeOffset = CurTime();
}

NTexture::NTexture(NTexture* Texture)
{
	Animations = Texture->Animations;
	Name = Texture->Name;
	PlayingAnimation = 0;
	TimeOffset = CurTime();
}

NTexture::~NTexture()
{
}

GLuint NAnimation::GetID(double Time)
{
	unsigned int IDLoc = fmod(Time*FPS,Frames.size());
	return Frames[IDLoc];
}

void NTexture::Play(std::string i_Name)
{
	for (unsigned int i=0;i<Animations.size();i++)
	{
		if (Name == Animations[i]->GetName())
		{
			PlayingAnimation = i;
			return;
		}
	}
	SetColor(Yellow);
	std::cout << "TEXTURE WARN: ";
	ClearColor();
	std::cout << "Animation " << i_Name << " not found in texture " << Name << "\n";
}

GLuint NTexture::GetID()
{
	return Animations[PlayingAnimation]->GetID(CurTime()+TimeOffset);
}

void NTexture::AddAnimation(NAnimation* Animation)
{
	Animations.push_back(Animation);
}
