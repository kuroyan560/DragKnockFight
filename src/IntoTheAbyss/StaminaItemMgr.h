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
	const int SPONE_TIMER = 60;	// �A�C�e���������X�|�[������^�C�}�[
	const int SPONE_COUNT = 3;	// ���̃X�|�[���ŃA�C�e�������ہ[�񂷂鐔

	const int GENERATE_CRASH = 3;		// �N���b�V�����ɐ�������A�C�e��
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
	void Update(const Vec2<float>& LeftUp, const Vec2<float>& RightDown);
	void Draw();

	// �����蔻����s���A�X�^�~�i�̉񕜗ʂ��擾����B
	int CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, const float& PilotRadius, StaminaItem::CHARA_ID CharaID, const Vec2<float>* PilotPos = nullptr);

	// �A�C�e���z��̃Q�b�^
	inline const std::array<StaminaItem, ITEM_COUNT> GetItemArray() { return item; }
	inline const Vec2<float> GetPos(const int& Index){return item[Index].GetPos();}

};