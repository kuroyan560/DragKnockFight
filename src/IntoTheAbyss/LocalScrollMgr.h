#pragma once
#include "Vec.h"
#include<vector>
#include<array>
#include"ShakeMgr.h"
#include"KuroMath.h"
//#include"LocalCamera.h"

class LocalScrollMgr
{
public:
	static const float INIT_SCROLL;
	LocalScrollMgr() :zoom(INIT_SCROLL)
	{};
	void Init(const Vec2<float> POS, const Vec2<float> &MAP_MAX_SIZE, const Vec2<float> &ADJ);
	void Update(const Vec2<float> &LineCenterPos);
	void CalucurateScroll(const Vec2<float> &VEL, const Vec2<float> &PLAYER_POS);
	Vec2<float>Affect(const Vec2<float> &Pos, const Vec2<float> &AFFECT);//�X�N���[���ƃY�[����K�p������k

	void Warp(const Vec2<float> POS);
	void Reset();

	Vec2<float> honraiScrollAmount;	//�{���X�N���[����
	Vec2<float> scrollAmount;		//�X�N���[����
	float zoom;						//�Y�[���{��

	Vec2<float> lineCenterOffset;	// �R�̒��S����ʂ̒��S�Ɏ����Ă��邽�߂̃I�t�Z�b�g ���S���炸��Ă����炱�̒l���g���Ē��S�Ɏ����Ă���B

	//std::shared_ptr<LocalCamera> camera;
private:

	Vec2<int>windowSize;
	Vec2<float>windowHalfSize;
	Vec2<float>adjLine;
	Vec2<float> mapSize;
	bool warpFlag;

	Vec2<float>resetAmount;
public:
	Vec2<float> CaluStartScrollLine(const Vec2<float> &SIZE)
	{
		Vec2<float> distance(0.0f, 0.0f);
		Vec2<float> startPos;

		startPos.x = (distance.x + SIZE.x);
		startPos.y = (distance.y + SIZE.y);
		return startPos;
	};


	Vec2<float> CaluEndScrollLine(const Vec2<float> &SIZE)
	{
		Vec2<float> distance(mapSize.x, mapSize.y);
		Vec2<float> endPos;

		endPos.x = (distance.x - SIZE.x);
		endPos.y = (distance.y - SIZE.y);
		return endPos;
	};
};