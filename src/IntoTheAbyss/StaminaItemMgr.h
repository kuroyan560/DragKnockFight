#pragma once
#include "StaminaItem.h"
#include "Singleton.h"
#include "Vec.h"
#include <array>

class StaminaItemMgr : public Singleton<StaminaItemMgr> {

private:

	/*===== �����o�ϐ� =====*/

	static const int ITEM_COUNT = 100;
	std::array<StaminaItem, ITEM_COUNT> item;

	int sponeTimer = 0;
	const int SPONE_TIMER = 120;	// �A�C�e���������X�|�[������^�C�}�[

	const int GENERATE_CRASH = 5;	// �N���b�V�����ɐ�������A�C�e��
	const float HEAL_AMOUNT = 20;	// �A�C�e��1�̃q�[����
	const float CRASH_VEL = 20;		// �N���b�V�������ۂɃA�C�e�����L�����(���o�p)

public:

	// �A�C�e���𐶐�����ۂɁA�ǂ̎�ނ̐������s�����̎��ʗp
	enum class GENERATE_STATUS
	{
		CRASH,
		SPONE,
	};


public:

	/*===== �����o�ϐ� =====*/

	void Init();
	void Generate(const Vec2<float>& GeneratePos, GENERATE_STATUS Status, Vec2<float>* CharaPos = nullptr, Color CharaColor = {}, StaminaItem::CHARA_ID CharaID = StaminaItem::CHARA_ID::LEFT);
	void Update();
	void Draw();

	// �����蔻����s���A�X�^�~�i�̉񕜗ʂ��擾����B
	int CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, StaminaItem::CHARA_ID CharaID, Color CharaColor);

};