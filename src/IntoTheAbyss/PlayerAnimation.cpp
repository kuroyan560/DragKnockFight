#include "PlayerAnimation.h"
#include"TexHandleMgr.h"

PlayerAnimation::PlayerAnimation()
{
	//ON_GROUND_WAIT
	static const int ON_GROUND_WAIT_NUM = 13;
	animations[ON_GROUND_WAIT].graph.resize(ON_GROUND_WAIT_NUM);
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/Player/on_ground_wait.png", ON_GROUND_WAIT_NUM, { ON_GROUND_WAIT_NUM,1 }, animations[ON_GROUND_WAIT].graph.data());
	animations[ON_GROUND_WAIT].interval = 10;
	animations[ON_GROUND_WAIT].loop = true;
	animations[ON_GROUND_WAIT].size = { 56,144 };
	animations[ON_GROUND_DASH].handOffsetLeft = { 0.0f,2.0f };
	animations[ON_GROUND_DASH].handOffsetRight = { 0.0f,2.0f };

	static const int ON_GROUND_DASH_NUM = 9;
	animations[ON_GROUND_DASH].graph.resize(ON_GROUND_DASH_NUM);
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/Player/on_ground_dash.png", ON_GROUND_DASH_NUM, { ON_GROUND_DASH_NUM,1 }, animations[ON_GROUND_DASH].graph.data());
	animations[ON_GROUND_DASH].interval = 10;
	animations[ON_GROUND_DASH].loop = true;
	animations[ON_GROUND_DASH].size = { 96,144 };
	animations[ON_GROUND_DASH].handOffsetLeft = { 0.0f,7.0f };
	animations[ON_GROUND_DASH].handOffsetRight = { 0.0f,7.0f };

	static const int ON_AIR_DASH_X_NUM = 9;
	animations[ON_AIR_DASH_X].graph.resize(ON_AIR_DASH_X_NUM);
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/Player/on_air_dash_x.png", ON_AIR_DASH_X_NUM, { ON_AIR_DASH_X_NUM,1 }, animations[ON_AIR_DASH_X].graph.data());
	animations[ON_AIR_DASH_X].interval = 10;
	animations[ON_AIR_DASH_X].loop = true;
	animations[ON_AIR_DASH_X].size = { 72,144 };
	//animations[ON_AIR_DASH_X].size = { 56,144 };
	animations[ON_AIR_DASH_X].handOffsetLeft = { 0.0f,4.0f };
	animations[ON_AIR_DASH_X].handOffsetRight = { 0.0f,4.0f };

	static const int ON_AIR_DASH_Y_NUM = 9;
	animations[ON_AIR_DASH_Y].graph.resize(ON_AIR_DASH_Y_NUM);
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/Player/on_air_dash_y.png", ON_AIR_DASH_Y_NUM, { ON_AIR_DASH_Y_NUM,1 }, animations[ON_AIR_DASH_Y].graph.data());
	animations[ON_AIR_DASH_Y].interval = 10;
	animations[ON_AIR_DASH_Y].loop = true;
	animations[ON_AIR_DASH_Y].size = { 64,144 };
	//animations[ON_AIR_DASH_Y].handOffsetLeft = { 0.0f,4.0f };
	//animations[ON_AIR_DASH_Y].handOffsetRight = { 0.0f,4.0f };

	static const int ON_WALL_WAIT_NUM = 8;
	animations[ON_WALL_WAIT].graph.resize(ON_WALL_WAIT_NUM);
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/Player/on_wall_wait.png", ON_WALL_WAIT_NUM, { ON_WALL_WAIT_NUM,1 }, animations[ON_WALL_WAIT].graph.data());
	animations[ON_WALL_WAIT].interval = 10;
	animations[ON_WALL_WAIT].loop = true;
	animations[ON_WALL_WAIT].size = { 56,136 };
	animations[ON_WALL_WAIT].handOffsetLeft = { 0.0f,5.0f };
	animations[ON_WALL_WAIT].handOffsetRight = { 0.0f,5.0f };

	static const int ON_WALL_DASH_NUM = 6;
	animations[ON_WALL_DASH].graph.resize(ON_WALL_DASH_NUM);
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/Player/on_wall_dash.png", ON_WALL_DASH_NUM, { ON_WALL_DASH_NUM,1 }, animations[ON_WALL_DASH].graph.data());
	animations[ON_WALL_DASH].interval = 10;
	animations[ON_WALL_DASH].loop = true;
	animations[ON_WALL_DASH].size = { 56,136 };
	animations[ON_WALL_DASH].handOffsetLeft = { 0.0f,5.0f };
	animations[ON_WALL_DASH].handOffsetRight = { 0.0f,5.0f };
}

void PlayerAnimation::Update()
{
	auto& anim = animations[status];
	timer++;
	if (anim.interval == timer)
	{
		idx++;
		if (anim.graph.size() == idx)
		{
			if (anim.loop)
			{
				idx = 0;
			}
			else
			{
				idx--;
			}
		}
		timer = 0;
	}
}