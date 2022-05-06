#pragma once
#include"../KuroEngine.h"
#include"StrategicLayer.h"
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

	std::vector<WayPointData> shortestData;
	std::unique_ptr<RestoreStamina> restoreStamina;
	std::unique_ptr<GoToTheField> goToTheField;

	bool initFlag = false;
	bool staminaInit = false;

	WayPointData startPoint, endPoint;
	bool startFlag;
	std::vector<WayPointData>route;
	Vec2<float>vel;
};

