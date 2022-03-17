#pragma once
#include "Vec.h"
#include "Singleton.h"
#include "Intersected.h"

// �����蔻��N���X
class Collider : public Singleton<Collider> {

public:

	/*===== �����o�֐� =====*/

	// �I�u�W�F�N�g�ƃI�u�W�F�N�g�̈ړ��ʐ������g���������蔻��B
	//INTERSECTED_LINE CheckHitVel(Vec2<float> pushBackPos, const Vec2<float> prevFramePos)


private:

	// �����Ɛ����̓����蔻��
	bool IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2);

	// �����Ɛ����̌�_�����߂鏈��
	Vec2<float> CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2);

};