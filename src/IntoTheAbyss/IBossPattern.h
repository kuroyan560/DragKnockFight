#pragma once
#include"../Common/Vec.h"
#include<vector>
#include<memory>
#include<array>

struct BossBulletData
{
	Vec2<float>pos;
	float speed;
	float dir;
	bool initFlag;
	BossBulletData() :initFlag(false)
	{
	};
	void Reset()
	{
		initFlag = false;
		dir = 0.0f;
		speed = 0.0f;
		pos = { 0.0f,0.0f };
	}
};

struct BossLimitMoveData
{
	Vec2<float>startPos;
	Vec2<float>endPos;
	bool hitFlag;
};

/// <summary>
/// �{�X�̍s���p�^�[���ɕK�v�ȏ���n��
/// </summary>
class CharacterInterFace;
struct BossPatternData
{
	Vec2<float>*moveVel;
	std::array<BossBulletData, 100>bulltData;
	bool swingFlag;
	std::array<BossLimitMoveData, 8> nearLimmitLine;//�{�X�ƕǂƂ̋����𒲂ׂ�ׂ̏��(�ߋ���)
	std::array<BossLimitMoveData, 4> farLimmitLine; //�{�X�ƕǂƂ̋����𒲂ׂ�ׂ̏��(������)

	BossPatternData() :swingFlag(false)
	{

	}
};

/// <summary>
/// �{�X�̃p�^�[���̒��ۃN���X
/// </summary>
class IBossPattern
{
public:
	virtual ~IBossPattern() {};
	virtual void Init() = 0;
	virtual void Update(BossPatternData* Boss) = 0;
};

