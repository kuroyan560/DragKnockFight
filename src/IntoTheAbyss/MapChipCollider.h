#pragma once
#include "Vec.h"
#include "Singleton.h"
#include "ScrollMgr.h"
#include "Intersected.h"
#include <vector>

using namespace std;

// �}�b�v�T�C�Y
const float MAP_CHIP_HALF_SIZE = 25.0f;
const float MAP_CHIP_SIZE = 50.0f;

class MapChipCollider : public Singleton<MapChipCollider> {


	/*===== �����o�֐� =====*/

private:
	// �����Ɛ����̓����蔻��
	bool IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2);

	// �����Ɛ����̌�_�����߂鏈��
	Vec2<float> CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2);


public:
	// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂Ɋђʂ����Ȃ��o�[�W����
	INTERSECTED_LINE CheckHitMapChipBasedOnTheVel(Vec2<float>& pos, const Vec2<float>& prevFramePos, const Vec2<float>& vel, const Vec2<float>& size, bool& onGround, const vector<vector<int>>& mapChipData);

	// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂ɂ߂荞�܂��Ȃ��o�[�W����
	INTERSECTED_LINE CheckHitMapChipBasedOnTheScale(Vec2<float>& pos, const Vec2<float>& size, const vector<vector<int>>& mapChipData, const INTERSECTED_LINE& direction, const bool& onGimmick = false);


};