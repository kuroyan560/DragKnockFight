#pragma once
#include"CharacterInterFace.h"
#include"Singleton.h"
#include<array>

class CharacterManager : public Singleton<CharacterManager>
{
	friend class Singleton<CharacterManager>;
	CharacterManager();

	//キャラクター生成
	std::shared_ptr<CharacterInterFace>CreateCharacter(const PLAYABLE_CHARACTER_NAME& CharacterName, const WHICH_TEAM& Team);

	//キャラクター選択用
	WHICH_TEAM nowSelectTeam;
	std::array<PLAYABLE_CHARACTER_NAME, TEAM_NUM>characterName = { PLAYABLE_LUNA,PLAYABLE_BOSS_0 };

	//キャラクター情報
	std::array<std::shared_ptr<CharacterInterFace>, TEAM_NUM>characters;

public:
	//マルチ対戦モード
	bool vsMode = false;
	//選択
	void CharactersSelectInit(PLAYABLE_CHARACTER_NAME LEFT_TEAM_NAME);
	void CharactersSelectUpdate();
	void CharactersSelectDraw();

	void CharactersGenerate();
	void CharactersInit(const Vec2<float>& PlayerRespawnPos, const Vec2<float>& EnemyResponePos, const bool& Appear);
	std::shared_ptr<CharacterInterFace>& Left() { return characters[LEFT_TEAM]; }
	std::shared_ptr<CharacterInterFace>& Right() { return characters[RIGHT_TEAM]; }

	void PracticeMode();
};