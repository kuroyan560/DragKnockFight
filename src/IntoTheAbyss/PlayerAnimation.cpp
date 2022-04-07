#include "PlayerAnimation.h"
#include"TexHandleMgr.h"

PlayerAnimation::PlayerAnimation()
{
	static const int DEFAULT_FRONT_NUM = 1;
	animations[DEFAULT_FRONT].graph.resize(1);
	//TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/Player/on_ground_wait.png", ON_GROUND_WAIT_NUM, { ON_GROUND_WAIT_NUM,1 }, animations[ON_GROUND_WAIT].graph.data());
	animations[DEFAULT_FRONT].graph[0] = TexHandleMgr::LoadGraph("resource/ChainCombat/player.png");
	animations[DEFAULT_FRONT].interval = 10;
	animations[DEFAULT_FRONT].loop = true;
	animations[DEFAULT_FRONT].size = { 123,123 };
	animations[DEFAULT_FRONT].handCenterOffset = { 0.0f,0.0f };

	static const int DEFAULT_BACK_NUM = 1;
	animations[DEFAULT_BACK].graph.resize(1);
	animations[DEFAULT_BACK].graph[0] = TexHandleMgr::LoadGraph("resource/ChainCombat/player_back.png");
	animations[DEFAULT_BACK].interval = 10;
	animations[DEFAULT_BACK].loop = true;
	animations[DEFAULT_BACK].size = { 117,96 };
	animations[DEFAULT_BACK].handCenterOffset = { 0.0f,0.0f };
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