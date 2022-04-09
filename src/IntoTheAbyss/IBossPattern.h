#pragma once
#include"../Common/Vec.h"

/// <summary>
/// �{�X�̍s���p�^�[���ɕK�v�ȏ���n��
/// </summary>
struct BossPatternData
{
	Vec2<float>*bossPos;
	int *stuckWindowTimer;
};

/// <summary>
/// �{�X�̃p�^�[���̒��ۃN���X
/// </summary>
class IBossPattern
{
public:
	virtual ~IBossPattern() {};
	virtual void Init() = 0;
	virtual void Update(BossPatternData *POS) = 0;
};

