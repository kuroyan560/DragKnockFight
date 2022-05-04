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

	float areaLeftPos;		// �퓬�G���A�̍��̍��W
	float areaRighPos;		// �퓬�G���A�̉E�̍��W

	int sponeTimer = 0;
	const int SPONE_TIMER = 120;	// �A�C�e���������X�|�[������^�C�}�[

	const int GENERATE_CRASH = 5;		// �N���b�V�����ɐ�������A�C�e��
	const int ADD_GENERATE_CRASH = 10;	// �N���b�V�����ɐ�������A�C�e��
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

	void SetArea(const float& LeftUpPos, const float& RightDownPos);
	void Init();
	void GenerateCrash(const Vec2<float>& GeneratePos, GENERATE_STATUS Status, Vec2<float>* CharaPos, StaminaItem::CHARA_ID CharaID, const Vec2<float>& SwingCharaPos);
	void GenerateSpone(const Vec2<float>& GeneratePos);
	void Update();
	void Draw();

	// �����蔻����s���A�X�^�~�i�̉񕜗ʂ��擾����B
	int CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, StaminaItem::CHARA_ID CharaID);

};