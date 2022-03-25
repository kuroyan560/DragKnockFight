#pragma once
#include "Vec.h"
#include "Singleton.h"
#include "Intersected.h"

// �����߂������Ȃ������蔻��N���X
class EventCpllider : public Singleton<EventCpllider> {

public:

	/*===== �����o�֐� =====*/

	// �I�u�W�F�N�g�ƃI�u�W�F�N�g�̈ړ��ʐ������g���������蔻��B
	// �߂�l�� returnValue != INTERSECTED_NONE �������瓖�����Ă��锻��ł��B
	INTERSECTED_LINE CheckHitVel(Vec2<float> pushBackPosA, const Vec2<float> prevFramePosA, const Vec2<float>& velA, const Vec2<float>& sizeA, const Vec2<float>& posB, const Vec2<float>& sizeB);

	// �I�u�W�F�N�g�ƃI�u�W�F�N�g�̃T�C�Y���g���������蔻��B
	// �ђʂ��Ȃ��ׂ̕ی��̓����蔻��ł��B��̔���͈ړ��ʂ��g���Ĕ�����s���܂����A���̔���̓T�C�Y���g���Ĕ�����s���܂��B
	// �߂�l�� returnValue != INTERSECTED_NONE �������瓖�����Ă��锻��ł��B
	// �ڂ����g������ Player.cpp �� CheckHit() �� �v���C���[�ƃh�b�X���u���b�N�̓����蔻�� �̂Ƃ�����Q�Ƃ��Ă��������I
	INTERSECTED_LINE CheckHitSize(Vec2<float> posA, const Vec2<float>& sizeA, const Vec2<float>& posB, const Vec2<float>& sizeB, const INTERSECTED_LINE& dir);


private:

	// �����Ɛ����̓����蔻��
	bool IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2);

	// �����Ɛ����̌�_�����߂鏈��
	Vec2<float> CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2);

};