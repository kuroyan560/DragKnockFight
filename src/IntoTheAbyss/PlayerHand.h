#pragma once
#include "Vec.h"
#include "Beacon.h"
#include <vector>
using namespace std;
#include"WinApp.h"

class LightManager;
#include"AfterImg.h"

// �v���C���[�̎�N���X
class PlayerHand {

public:

	/*-- �����o�ϐ� --*/

	Vec2<float> handPos;			// �r�̕`����W
	Vec2<float> vel;				// �e��ł������̈ړ���
	Vec2<float> sightPos;			// �Ə����W
	Vec2<float> drawPos;			// Draw You
	float armDistance;		// �v���C���[�̒��S����̋��� �E��ƍ���ŕς��邽��
	float inputAngle;		// ���͂��ꂽ�p�x
	bool isFirstShot;		// �ŏ��̈ꔭ�͋��������t���O
	bool isNoInputTimer;	// Input Sitenai Tokiha Honrai Amount Wo Herasu

	Beacon teleportPike;	// �u�Ԉړ��̒Z��
	Beacon timeStopPike;	// ���Ԓ�~�̒Z��
	int pikeCooltime;		// �Z���S�ʂ̂̃N�[���^�C��

	AfterImg afterImg;

	/*-- �萔 --*/

public:

	const float ARM_RANGE_OF_MOTION = 15.0f;	// ��̉���
	const float SHOT_VEL = 15.0f;				// �e�����������̔���
	const float FIRST_SHOT_VEL = 20.0f;			// �ŏ��̈ꔭ�ڂ����������̔���
	const float SIGHT_SIZE = 5.0f;				// �Ə��̃T�C�Y
	const int PIKE_COOL_TIME = 60;				// �r�[�R���̃N�[���^�C��


	/*-- �����o�֐� --*/

public:

	// �R���X�g���N�^
	PlayerHand();

	// ����������
	void Init(const float& armDistance);

	// �X�V����
	void Update(const Vec2<float>& playerCenterPos);

	// �`�揈��
	void Draw(LightManager& LigManager, const Vec2<float>& ExtRate, const int& GraphHandle, const float& InitAngle, const Vec2<float>& RotaCenterUV, const bool &DRAW_CURSOR);

	// �e��ł������̏���
	void Shot(const Vec2<float>& forwardVec, const bool& isFirstShot);

	// �����蔻��֐�
	void CheckHit(const vector<vector<int>>& mapData);

	// �Ə���`�悷��ŒZ���������߂�B
	void CheckShortestPoint(const vector<vector<int>>& mapData);

	bool IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2);

	Vec2<float> CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2);

	// Angle wo Tikadukeru
	inline void PutCloseAngle(const float& defAngle) { inputAngle += (defAngle - inputAngle) / 10.0f; }

	// isNoInputTimer no Setter
	inline void SetIsNoInputTimer(const bool& set) { isNoInputTimer = set; }

	// �ǂɂ����������ɍŏ��ɓ��������t���O�����������鏈��.
	inline void Hit() { isFirstShot = false; }

	// �ŏ��Ɍ��������̏����B
	inline void FirstShot() { isFirstShot = true; }

	// �p�x�̃Z�b�^
	inline void SetAngle(const float& angle) { inputAngle = angle; }
	// �p�x�̃Q�b�^
	inline const float& GetAngle() { return inputAngle; }
	// ���W�̃Q�b�^
	inline const Vec2<float>& GetPos() { return handPos; }
	// �ŏ��ɓ��������t���O�̃Q�b�^
	inline const bool& GetIsFirstShot() { return isFirstShot; }

	//�c�����o��
	void EmitAfterImg(const Vec2<float>& TeleAmount, const int& GraphHandle, const Vec2<float>& GraphSize, const Vec2<bool>& Miror);

private:

	// �؂�グ
	int RoundUp(int size, int align) {
		return UINT(size + align - 1) & ~(align - 1);
	}

	//�X�N���[���Ȃǂɂ��l���������S���W
	Vec2<float> GetCenterDrawPos();
};