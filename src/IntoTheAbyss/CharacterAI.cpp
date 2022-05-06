#include "CharacterAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"CharacterManager.h"
#include"Stamina.h"

CharacterAI::CharacterAI()
{
	pos = std::make_shared<Vec2<float>>();

	//����w�̐���--------------------------
	move = std::make_shared<OperateMove>(pos);
	//����w�̐���--------------------------

	//�U�镑���w�̐���--------------------------
	betweenPoints = std::make_shared<MovingBetweenTwoPoints>(move);
	//�U�镑���w�̐���--------------------------

	//��p�w�̐���--------------------------
	moveToGoal = std::make_shared<FollowPath>(betweenPoints);
	//��p�w�̐���--------------------------


	//�헪�w�̐���--------------------------
	
	//�헪�w�̐���--------------------------
}

void CharacterAI::Init()
{
	//restoreStamina->Init();
	//���u��
	restoreStamina = std::make_unique<RestoreStamina>
		(moveToGoal, betweenPoints, wayPoints);
	initFlag = true;


	goToTheField = std::make_unique<GoToTheField>(moveToGoal);
	goToTheField->Init();
}

void CharacterAI::Update()
{
	//�L�����N�^�[AI�ɕK�v�ȃf�[�^�W��--------------------------
	CharacterAIData::Instance()->playerData.stamineGauge = 0;
	for (int i = 0; i < CharacterManager::Instance()->Left()->staminaGauge->stamina.size(); ++i)
	{
		if (CharacterManager::Instance()->Left()->staminaGauge->stamina[i].GetIsActivate())
		{
			++CharacterAIData::Instance()->playerData.stamineGauge;
		}
	}

	CharacterAIData::Instance()->bossData.stamineGauge = 0;
	for (int i = 0; i < CharacterManager::Instance()->Right()->staminaGauge->stamina.size(); ++i)
	{
		if (CharacterManager::Instance()->Right()->staminaGauge->stamina[i].GetIsActivate())
		{
			++CharacterAIData::Instance()->bossData.stamineGauge;
		}
	}

	//�G�ƃv���C���[�Ƃ̋���
	CharacterAIData::Instance()->distance = CharacterManager::Instance()->Left()->pos.Distance(CharacterManager::Instance()->Right()->pos);
	//�L�����N�^�[AI�ɕK�v�ȃf�[�^�W��--------------------------


	if (initFlag && !staminaInit)
	{
		float value = restoreStamina->EvaluationFunction();
		restoreStamina->Init();
		staminaInit = true;
	}
	//move->Update(Vec2<float>(15.0f, 0.0f));

	if (initFlag)
	{
		//restoreStamina->route = shortestData;
		//restoreStamina->Update();
		//startPoint = restoreStamina->startPoint;
		//endPoint = restoreStamina->endPoint;
		//startFlag = restoreStamina->startFlag;

		goToTheField->moveToOnwGround->route = shortestData;
		goToTheField->Update();
		startPoint = goToTheField->startPoint;
		endPoint = goToTheField->endPoint;
		startFlag = goToTheField->startFlag;
		vel = move->vel;
	}
}

void CharacterAI::Draw()
{
	if (initFlag)
	{
		Vec2<float>drawPos = *pos;
		DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(drawPos), 10.0f, Color(128, 0, 128, 255));
	}
}
