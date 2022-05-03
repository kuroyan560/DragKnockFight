#pragma once
#include"../KuroEngine.h"
#include"TacticalLayer.h"
#include"BehavioralLayer.h"
#include"OperatingLayer.h"
#include<memory>

/// <summary>
/// �L�����N�^�[AI�̈ӎv����
/// �K�w�^�S�[���w���v�����j���O�Ɋ�Â��Đ݌v����
/// </summary>
class CharacterAI
{
public:
	CharacterAI();

	void Init();
	void Update();
	void Draw();
	std::unique_ptr<FollowPath> moveToGoal;
	std::shared_ptr<MovingBetweenTwoPoints> betweenPoints;
	std::shared_ptr<OperateMove>move;

	std::shared_ptr<Vec2<float>>pos;
	std::vector<WayPointData> shortestData;

	bool initFlag = false;
};

