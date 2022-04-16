#pragma once
#include"Vec.h"
#include<memory>
#include<vector>
#include"StagingInterFace.h"
#include"AreaCollider.h"
#include"../IntoTheAbyss/BulletCollision.h"
#include<array>
#include "Intersected.h"

class CharacterInterFace
{
	void SwingUpdate();
	void Crash(const Vec2<float>& MyVec);
	void CrashUpdate();

	//��ʋ��݃N���b�V������
	const int STACK_WINDOW_TIMER = 120.0f;// �E�B���h�E�ɋ��܂����^�C�}�[
	int stackWindowTimer;		// �E�B���h�E�ɋ��܂����^�C�}�[

	//�w�n�Ƃ̔���
	Square areaHitBox;

	//�e�Ƃ̓����蔻��
	SphereCollision bulletHitSphere;

	const int AFTER_SWING_DELAY = 15;
	Vec2<float> swingInertiaVec;// �U��񂵂̊����̈ړ�����
	float swingInertia;			// �U��񂵂̊���
	int afterSwingDelay;		// �U��񂵂̂��ƂɃ{�X�����������Ȃ���Ԃɂ��邽�߂̃^�C�}�[

	//���o�Ȃǂ̓����̊֌W�œ������~�߂�
	bool canMove;

protected:

	bool nowSwing;
	Vec2<float> swingStartVec;
	Vec2<float> swingEndVec;
	float swingEaseRate;


protected:
	static const enum HIT_DIR { LEFT, RIGHT, TOP, BOTTOM, HIT_DIR_NUM };
	std::array<int, HIT_DIR_NUM>mapChipHit;
	bool stackMapChip;

	//�����J�n���ɌĂяo�����
	CharacterInterFace(const Vec2<float>& HonraiSize) :size(HonraiSize)
	{
		areaHitBox.center = &pos;
		areaHitBox.size = size;
		bulletHitSphere.center = &pos;
		bulletHitSphere.radius = size.x;
	}

	std::weak_ptr<CharacterInterFace>partner;
	StagingInterFace stagingDevice;
	Vec2<float>size;	//�T�C�Y

	//[�L�������ƂɈႤ�֐�]
	virtual void OnInit() = 0;
	virtual void OnUpdate(const std::vector<std::vector<int>>& MapData) = 0;
	virtual void OnDraw() = 0;
	virtual void OnCheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos) = 0;
	virtual void OnHitMapChip(const HIT_DIR& Dir) = 0;

	//[���ʊ֐�]
	//�U���
	void SwingPartner();
	//�Q�b�^��
	const Vec2<float>& GetPartnerPos()
	{
		return partner.lock()->pos;
	}
	//�U��񂵂ɂ�銵��
	Vec2<float>GetSwingInertia() { return swingInertiaVec * swingInertia; }
	//�U��񂵒���̍d������
	bool GetSwingRigor() { return 0 < afterSwingDelay; }
	// �����蔻����ۑ��B
	void SaveHitInfo(bool& isHitTop, bool& isHitBottom, bool& isHitLeft, bool& isHitRight, const INTERSECTED_LINE& intersectedLine);

public:
	//�o�ꉉ�o������������
	virtual bool Appear() = 0;

public:
	static const int LINE_LENGTH = 150;
	float addLineLength;	//�R
	Vec2<float> pos;			// ���W
	Vec2<float>vel;
	Vec2<float> prevPos;		// �O�t���[���̍��W

	void RegisterSetPartner(const std::shared_ptr<CharacterInterFace>Partner) { partner = Partner; }
	void Init(const Vec2<float>& GeneratePos);	//���E���h�J�n���ɌĂяo�����
	void Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos);
	void Draw();
	//�����蔻��
	void CheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos);

	const Square& GetAreaHitBox() { return areaHitBox; }
	const SphereCollision& GetBulletHitSphere() { return bulletHitSphere; }

	const int& GetStackWinTimer() { return stackWindowTimer; }
	const bool& GetNowSwing() { return nowSwing; }
	const bool& GetNowStuckWin() { return 0 < stackWindowTimer; }
	const bool& GetCanMove() { return canMove; }
	const bool& GetStackFlag() { return stackMapChip; }

	void SetCanMove(const bool& Flg) { canMove = Flg; }

	inline void FinishSwing() { nowSwing = false; }
};
