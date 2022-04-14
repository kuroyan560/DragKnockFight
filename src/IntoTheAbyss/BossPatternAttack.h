#pragma once
#include"../IntoTheAbyss/IBossPattern.h"

/// <summary>
/// �e�������p�^�[��
/// </summary>
class BossPatternAttack:public IBossPattern
{
public:
	BossPatternAttack();
	~BossPatternAttack() {};

	void Init()override;
	void Update(BossPatternData *DATA)override;

private:
	int shotInterval;
	bool changeAngleFlag;
	const float SHOT_BULLET_MAX;//��x�Ɍ��e�̑���
	const float MAX_SPEED;//��x�Ɍ��e�̑���
	const int SHOT_INTERVAL_MAX;//�e�����Ԋu

};

