#pragma once
#include "Vec.h"
#include "Color.h"

// �X�^�~�i�A�C�e��
class StaminaItem {

private:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;		// �A�C�e���̍��W
	Vec2<float> forwardVec;	// �ړ����� ��ɃN���b�V���ȂǂŎU��΂�ۂɎg�p����B
	Vec2<float>* charaPos;	// �擾�����L�����̍��W
	Color itemColor;		// �擾���ꂽ�ۂ̃A�C�e���̐F ������͉摜�n���h���ɂȂ�\��B
	float vel;				// �ړ��� ��ɃN���b�V���ȂǂŎU��΂�ۂɎg�p����B
	float healAmount;		// ���̃A�C�e���̉񕜗�
	int stopTimer;			// �N���b�V���Ȃǂŏo���A�C�e������U�L�������ۂɂ�����Ƃ����~�߂邽�߂̕ϐ��B
	bool isActive;			// �L�����t���O
	bool isAcquired;		// �擾���ꂽ���t���O

public:
	enum class STAMINA_ITEM_ID {

		SPONE_ITEM,		// �����X�|�[���̃A�C�e��
		CRASH_ITEM,		// �N���b�V�����ɎU��΂�A�C�e��

	};
	enum class CHARA_ID {
		LEFT,
		RIGHT
	};
private:
	STAMINA_ITEM_ID itemID;
	CHARA_ID charaID;


private:

	/*===== �萔 =====*/

	const float HIT_RADIUS = 30;	// �����蔻����s���ۂ̔��a �傫�߂ɂ��Ă���B
	const float DRAW_RADIUS = 10;	// �A�C�e����`�悷��T�C�Y ������͉摜�ɂȂ�̂ŕs�v�ɂȂ�B
	const float TRACING_SPEED = 30;	// �擾����ĒǐՂ���Ƃ��̃X�s�[�h
	const int STOP_TIMER = 30;		// �N���b�V���Ȃǂŏo���A�C�e������U�L�������ۂɂ�����Ƃ����~�߂邽�߂̕ϐ��B


public:

	/*===== �����o�֐� =====*/

	StaminaItem();
	void Init();
	void Generate(const Vec2<float>& GeneratePos, const Vec2<float>& ForwardVec, const float& HealAmount, const float& Vel, STAMINA_ITEM_ID ItemID, const bool& IsAcquired = false, Vec2<float>* CharaPos = nullptr, CHARA_ID CharaID = CHARA_ID::LEFT);
	void Update();
	void Draw();

	// �L�����N�^�[�Ƃ̓����蔻��
	bool CheckHit(const Vec2<float>& CharaPos, const float& CharaRadius, CHARA_ID CharaID);

	// �e��A�N�Z�b�^
	inline const bool& GetIsActive() { return isActive; }
	inline const bool& GetIsAcquired() { return isAcquired; }
	inline const float& GetHealAmount() { return healAmount; }
	inline const CHARA_ID& GetCharaID() { return charaID; }

};
