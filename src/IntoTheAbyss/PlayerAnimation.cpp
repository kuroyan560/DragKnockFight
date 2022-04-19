#include "PlayerAnimation.h"
#include"TexHandleMgr.h"

PlayerAnimation::PlayerAnimation(const PLAYABLE_CHARACTER_NAME& CharacterName)
{
	if (PLAYER_CHARACTER_NUM <= CharacterName)assert(0);
	static const std::string NAME_DIR[PLAYER_CHARACTER_NUM] =
	{
		"luna",
		"lacy"
	};
	static const std::string DIR = "resource/ChainCombat/player/";

	static const int DEFAULT_FRONT_NUM = 1;
	animations[DEFAULT_FRONT].graph.resize(DEFAULT_FRONT_NUM);
	//TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/Player/on_ground_wait.png", ON_GROUND_WAIT_NUM, { ON_GROUND_WAIT_NUM,1 }, animations[ON_GROUND_WAIT].graph.data());
	animations[DEFAULT_FRONT].graph[0] = TexHandleMgr::LoadGraph(DIR + NAME_DIR[CharacterName] + "/player.png");
	animations[DEFAULT_FRONT].interval = 10;
	animations[DEFAULT_FRONT].loop = true;
	animations[DEFAULT_FRONT].size = { 123,123 };
	animations[DEFAULT_FRONT].handCenterOffset = { 0.0f,0.0f };

	static const int DEFAULT_BACK_NUM = 1;
	animations[DEFAULT_BACK].graph.resize(DEFAULT_BACK_NUM);
	animations[DEFAULT_BACK].graph[0] = TexHandleMgr::LoadGraph(DIR + NAME_DIR[CharacterName] + "/player_back.png");
	animations[DEFAULT_BACK].interval = 10;
	animations[DEFAULT_BACK].loop = true;
	animations[DEFAULT_BACK].size = { 117,96 };
	animations[DEFAULT_BACK].handCenterOffset = { 0.0f,0.0f };

	static const int HOLD_NUM = 3;
	animations[HOLD].graph.resize(HOLD_NUM);
	TexHandleMgr::LoadDivGraph(DIR + NAME_DIR[CharacterName] + "/hold.png", HOLD_NUM, { HOLD_NUM,1 }, animations[HOLD].graph.data());
	animations[HOLD].interval = 6;
	animations[HOLD].loop = true;
	animations[HOLD].size = { 230,220 };
	animations[HOLD].handCenterOffset = { 0.0f,0.0f };

	static const int SWINGED_NUM = 1;
	animations[SWINGED].graph.resize(SWINGED_NUM);
	animations[SWINGED].graph[0] = TexHandleMgr::LoadGraph(DIR + NAME_DIR[CharacterName] + "/swinged.png");
	animations[SWINGED].interval = 10;
	animations[SWINGED].loop = true;
	animations[SWINGED].size = { 102,123 };
	animations[SWINGED].handCenterOffset = { 0.0f,0.0f };
}

#include"SlowMgr.h"
void PlayerAnimation::Update()
{
	auto& anim = animations[status];
	timer += SlowMgr::Instance()->slowAmount;
	if (anim.interval <= timer)
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