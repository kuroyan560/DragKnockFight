#pragma once
#include "Vec.h"
#include "Color.h"
#include"Angle.h"
#include<memory>

struct SphereCollision;

// �X�^�~�i�A�C�e��
class StaminaItem {

private:
	static const enum STAR_COLOR { RED, GREEN, YELLOW, BLUE, GRAPH_NUM, 
		LEFT = GREEN, RIGHT = RED, NONE = YELLOW };
	static int GRAPH_HANDLE[GRAPH_NUM];

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;		// �A�C�e���̍��W
	Vec2<float> forwardVec;	// �ړ����� ��ɃN���b�V���ȂǂŎU��΂�ۂɎg�p����B
	Vec2<float>* charaPos;	// �擾�����L�����̍��W
	int graph;
	float vel;				// �ړ��� ��ɃN���b�V���ȂǂŎU��΂�ۂɎg�p����B
	float healAmount;		// ���̃A�C�e���̉񕜗�
	bool isActive;			// �L�����t���O
	bool isAcquired;		// �擾���ꂽ���t���O
	Angle flashRad;
	std::shared_ptr<SphereCollision>collisionData;

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

	const float HIT_RADIUS = 60;	// �����蔻����s���ۂ̔��a �傫�߂ɂ��Ă���B
	const float TRACING_SPEED = 60;	// �擾����ĒǐՂ���Ƃ��̃X�s�[�h�B(���o�p�H)
	const float ACQUIRED_VEL = 20;	// �ʏ�X�|�[���̃A�C�e�����擾���ꂽ���ɍL����ړ��ʁB(���o�p)


public:

	/*===== �����o�֐� =====*/

	StaminaItem();
	void Init();
	void Generate(const Vec2<float>& GeneratePos, const Vec2<float>& ForwardVec, const float& HealAmount, const float& Vel, STAMINA_ITEM_ID ItemID, const bool& IsAcquired = false, Vec2<float>* CharaPos = nullptr, CHARA_ID CharaID = CHARA_ID::LEFT);
	void Update();
	void Draw();

	// �L�����N�^�[�Ƃ̓����蔻��
	bool CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, const float& PilotRadius, CHARA_ID CharaID, const Vec2<float>* PilotPos = nullptr);

	// �擾���ꂽ��Ԃɂ���B
	void Acquire(Vec2<float>* CharaPos, CHARA_ID CharaID);

	// �e��A�N�Z�b�^
	inline const bool& GetIsActive() { return isActive; }		// �����t���O�̃Q�b�^
	inline const bool& GetIsAcquired() { return isAcquired; }	// �擾����Ă��邩�t���O true�ɂȂ��Ă�����擾����Ă��܂��B	 �擾����Ă����ԁ��擾�����L�����ɒǔ����Ă����ԁB
	inline const float& GetHealAmount() { return healAmount; }	// �q�[���ʂ̃Q�b�^
	inline const CHARA_ID& GetCharaID() { return charaID; }		// �擾�����L������ID �E������
	inline const std::shared_ptr<SphereCollision> GetCollisionData() { return collisionData; }	// �����蔻��f�[�^�̃Q�b�^
	inline const Vec2<float>& GetPos() { return pos; }			// ���W�̃Q�b�^

};
