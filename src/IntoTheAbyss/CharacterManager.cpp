#include "CharacterManager.h"
#include"Player.h"
#include"Boss.h"
#include"Tutorial.h"

CharacterManager::CharacterManager()
{
}

std::shared_ptr<CharacterInterFace> CharacterManager::CreateCharacter(const  PLAYABLE_CHARACTER_NAME &CharacterName, const WHICH_TEAM &Team)
{
	switch (CharacterName)
	{
	case PLAYABLE_LUNA:
		return std::make_shared<Player>(CharacterName, Team);
	case PLAYABLE_LACY:
		return std::make_shared<Player>(CharacterName, Team);
	case PLAYABLE_BOSS_0:
		return std::make_shared<Boss>();
	}
	assert(0);
}

void CharacterManager::CharactersSelectInit()
{
	nowSelectTeam = LEFT_TEAM;

	characterName[LEFT_TEAM] = PLAYABLE_LUNA;
	if (vsMode)
	{
		characterName[RIGHT_TEAM] = PLAYABLE_LACY;
	}
	else
	{
		characterName[RIGHT_TEAM] = PLAYABLE_BOSS_0;
	}
}

void CharacterManager::CharactersSelectUpdate()
{
	//左選択
	if (nowSelectTeam == LEFT_TEAM)
	{
		if (UsersInput::Instance()->ControllerOnTrigger(0, L_DOWN))
		{
			nowSelectTeam = RIGHT_TEAM;
		}
	}
	//右選択
	else
	{
		if (UsersInput::Instance()->ControllerOnTrigger(0, L_UP))
		{
			nowSelectTeam = LEFT_TEAM;
		}
	}

	//キャラクター選択
	if (UsersInput::Instance()->ControllerOnTrigger(0, L_LEFT) && 0 < characterName[nowSelectTeam])
	{
		characterName[nowSelectTeam] = (PLAYABLE_CHARACTER_NAME)(characterName[nowSelectTeam] - 1);
	}
	else if (UsersInput::Instance()->ControllerOnTrigger(0, L_RIGHT) && characterName[nowSelectTeam] < PLAYABLE_CHARACTER_NUM - 1)
	{
		characterName[nowSelectTeam] = (PLAYABLE_CHARACTER_NAME)(characterName[nowSelectTeam] + 1);
	}
}

void CharacterManager::CharactersSelectDraw()
{
	ImGui::Begin("CharacterSelect");

	ImGui::Text("Stick Up   / Down  : Select Left/Right");
	ImGui::Text("Stick Left / Right : Select Character");
	ImGui::Text("Bbutton : Return To StageSelect");
	ImGui::Text("Abutton : GameStart");

	ImGui::Separator();

	static const std::string NOW_SELECT = "  - Now Select -";
	static const std::string NAME[PLAYABLE_CHARACTER_NUM] =
	{
		"Luca",
		"Lacy",
		"Boss_0"
	};
	static const std::string CHARACTER[TEAM_NUM] =
	{
		"Left Character",
		"RightCharacter"
	};

	for (int i = 0; i < TEAM_NUM; ++i)
	{
		std::string text = CHARACTER[i] + NAME[characterName[i]] + (nowSelectTeam == i ? NOW_SELECT : "");
		ImGui::Text("%s", text.c_str());
	}

	ImGui::End();
}

void CharacterManager::CharactersGenerate()
{
	//キャラクター生成
	for (int i = 0; i < TEAM_NUM; ++i)
	{
		characters[i].reset();

		switch (characterName[i])
		{
		case PLAYABLE_LUNA:
			characters[i] = std::make_shared<Player>(characterName[i], (WHICH_TEAM)i);
			break;
		case PLAYABLE_LACY:
			characters[i] = std::make_shared<Player>(characterName[i], (WHICH_TEAM)i);
			break;
		case PLAYABLE_BOSS_0:
			characters[i] = std::make_shared<Boss>();
			break;
		}
	}
	//どちらのキャラクターも生成完了してからでないと、情報登録出来ない
	for (int i = 0; i < TEAM_NUM; ++i)
	{
		characters[i]->RegisterCharacterInfo(characters[TEAM_NUM - i - 1], (WHICH_TEAM)i, characterName[i]);
	}
}

void CharacterManager::CharactersInit(const Vec2<float>& PlayerRespawnPos, const Vec2<float>& EnemyResponePos, const bool &Appear)
{
	characters[LEFT_TEAM]->Init(PlayerRespawnPos, Appear);
	characters[RIGHT_TEAM]->Init(EnemyResponePos, Appear);
}

void CharacterManager::PracticeMode()
{
	characterName[LEFT_TEAM] = PLAYABLE_LUNA;
	characterName[RIGHT_TEAM] = PLAYABLE_LACY;
}
