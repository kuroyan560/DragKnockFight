#include "CharacterAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"CharacterManager.h"
#include"Stamina.h"

CharacterAI::CharacterAI()
{
	startTimer = 0;
	initFlag = false;
	useAiFlag = false;

	std::vector<BehaviorGraphData>data;
	data.resize(5);

	std::string pass = "resource/ChainCombat/boss/behaviorPrediction/";
	std::string pass2 = "resource/ChainCombat/boss/0/icon/";
	int dataArrayNum = 0;
	int roopNum = 10;
	//ダッシュ
	{
		const int GRAPH_NUM = 3;
		data[STRATEGY_SWING_DASH].handle.graph.resize(GRAPH_NUM);
		data[STRATEGY_SWING_DASH].handle.interval = roopNum;
		data[STRATEGY_SWING_DASH].handle.loop = true;
		data[STRATEGY_SWING_DASH].cautionFlag = false;
		TexHandleMgr::LoadDivGraph(pass + "dash.png", GRAPH_NUM, { GRAPH_NUM,1 }, data[dataArrayNum].handle.graph.data());
	}

	++dataArrayNum;
	//時計回り振り回し
	{
		const int GRAPH_NUM = 3;
		data[STRATEGY_SWING_CLOCKWISE].handle.graph.resize(GRAPH_NUM);
		data[STRATEGY_SWING_CLOCKWISE].handle.interval = roopNum;
		data[STRATEGY_SWING_CLOCKWISE].handle.loop = true;
		data[STRATEGY_SWING_CLOCKWISE].cautionFlag = false;
		TexHandleMgr::LoadDivGraph(pass + "clock_swing.png", GRAPH_NUM, { GRAPH_NUM,1 }, data[dataArrayNum].handle.graph.data());
	}

	++dataArrayNum;
	//反時計周り振り回し
	{
		const int GRAPH_NUM = 3;
		data[STRATEGY_SWING_COUNTERCLOCKWISE].handle.graph.resize(GRAPH_NUM);
		data[STRATEGY_SWING_COUNTERCLOCKWISE].handle.interval = roopNum;
		data[STRATEGY_SWING_COUNTERCLOCKWISE].handle.loop = true;
		data[STRATEGY_SWING_COUNTERCLOCKWISE].cautionFlag = false;
		TexHandleMgr::LoadDivGraph(pass + "un_clock_swing.png", GRAPH_NUM, { GRAPH_NUM,1 }, data[dataArrayNum].handle.graph.data());
	}


	++dataArrayNum;
	//三連撃時計振り回し
	{
		const int GRAPH_NUM = 6;
		data[STRATEGY_SWING_3TIMES_CLOCKWISE].handle.graph.resize(GRAPH_NUM);
		data[STRATEGY_SWING_3TIMES_CLOCKWISE].handle.interval = roopNum;
		data[STRATEGY_SWING_3TIMES_CLOCKWISE].handle.loop = true;
		data[STRATEGY_SWING_3TIMES_CLOCKWISE].cautionFlag = true;
		TexHandleMgr::LoadDivGraph(pass2 + "clock_swing_triple.png", GRAPH_NUM, { GRAPH_NUM,1 }, data[dataArrayNum].handle.graph.data());
	}

	++dataArrayNum;
	//三連撃反時計回り振り回し
	{
		const int GRAPH_NUM = 6;
		data[STRATEGY_SWING_3TIMES_COUNTERCLOCKWISE].handle.graph.resize(GRAPH_NUM);
		data[STRATEGY_SWING_3TIMES_COUNTERCLOCKWISE].handle.interval = roopNum;
		data[STRATEGY_SWING_3TIMES_COUNTERCLOCKWISE].handle.loop = true;
		data[STRATEGY_SWING_3TIMES_COUNTERCLOCKWISE].cautionFlag = true;
		TexHandleMgr::LoadDivGraph(pass2 + "un_clock_swing_triple.png", GRAPH_NUM, { GRAPH_NUM,1 }, data[dataArrayNum].handle.graph.data());
	}
	behaviorGauge = std::make_unique<BehaviorPredection>(data);
}

void CharacterAI::Init()
{
	if (!initFlag)
	{
		//戦略層の生成--------------------------
		//strategyArray[STRATEGY_RESTORE_STAMINA] = std::make_unique<RestoreStamina>();
		strategyArray[STRATEGY_SWING_3TIMES_COUNTERCLOCKWISE] = std::make_unique<SwingThreeTimesCounterClockWise>();
		strategyArray[STRATEGY_SWING_3TIMES_CLOCKWISE] = std::make_unique<SwingClockWiseThreeTimes>();
		strategyArray[STRATEGY_SWING_COUNTERCLOCKWISE] = std::make_unique<SwingCounterClockWise>();
		strategyArray[STRATEGY_SWING_CLOCKWISE] = std::make_unique<SwingClockWise>();
		strategyArray[STRATEGY_SWING_DASH] = std::make_unique<Dash>();
		//戦略層の生成--------------------------
	}
	initFlag = true;
	useAiFlag = true;
	strategyOfChoice = STRATEGY_SWING_DASH;
	startFlag = false;
	CharacterAIData::Instance()->dashTimer = 0;
	startDashFlag = false;

	for (int i = 0; i < strategyArray.size(); ++i)
	{
		strategyArray[i]->Init();
	}

	CharacterAIOrder::Instance()->Init();
}

void CharacterAI::Finalize()
{
	useAiFlag = false;
}

void CharacterAI::Update()
{
	//キャラクターAIに必要なデータ集め--------------------------
	{
		CharacterAIData::Instance()->playerData.stamineGauge = 0;
		float staminaMaxGauge = 0;
		for (int i = 0; i < CharacterManager::Instance()->Left()->staminaGauge->stamina.size(); ++i)
		{
			if (CharacterManager::Instance()->Left()->staminaGauge->stamina[i].GetIsActivate())
			{
				++staminaMaxGauge;
			}
		}
		CharacterAIData::Instance()->playerData.stamineGauge = staminaMaxGauge / static_cast<float>(CharacterManager::Instance()->Left()->staminaGauge->stamina.size());
	}

	{
		CharacterAIData::Instance()->bossData.stamineGauge = 0;
		float staminaMaxGauge = 0;
		for (int i = 0; i < CharacterManager::Instance()->Right()->staminaGauge->stamina.size(); ++i)
		{
			if (CharacterManager::Instance()->Right()->staminaGauge->stamina[i].GetIsActivate())
			{
				++staminaMaxGauge;
			}
		}
		CharacterAIData::Instance()->bossData.stamineGauge = staminaMaxGauge / static_cast<float>(CharacterManager::Instance()->Left()->staminaGauge->stamina.size());
	}
	//敵とプレイヤーとの距離
	CharacterAIData::Instance()->distance = CharacterManager::Instance()->Left()->pos.Distance(CharacterManager::Instance()->Right()->pos);



	if (CharacterAIData::Instance()->dashFlag)
	{
		startDashFlag = true;
	}
	if (startDashFlag)
	{
		++CharacterAIData::Instance()->dashTimer;
		if (CharacterAIData::Instance()->dashFlag)
		{
			++CharacterAIData::Instance()->dashCount;
		}
	}
	//20Flame以上入力が無かったらリセットする
	if (20 <= CharacterAIData::Instance()->dashTimer)
	{
		CharacterAIData::Instance()->dashCount = 0;
		CharacterAIData::Instance()->dashTimer = 0;
		startDashFlag = false;
	}



	//キャラクターAIに必要なデータ集め--------------------------

	if (useAiFlag)
	{
		//意思決定--------------------------
		//戦略が成功又は失敗した際
		if (strategyArray[strategyOfChoice]->CurrentProgress() == AiResult::OPERATE_SUCCESS ||
			strategyArray[strategyOfChoice]->CurrentProgress() == AiResult::OPERATE_FAIL)
		{
			float max = 0.0f;
			int selecting = 0;
			//評価値の最大値から戦略を決定する
			for (int i = 0; i < strategyArray.size(); ++i)
			{
				float strategyEvaluationValue = strategyArray[i]->EvaluationFunction();
				if (max < strategyEvaluationValue)
				{
					selecting = i;
					max = strategyEvaluationValue;
				}
			}
			strategyOfChoice = selecting;
			//strategyOfChoice = STRATEGY_SWING_DASH;
			strategyArray[strategyOfChoice]->Init();
		}
		//意思決定--------------------------

		strategyArray[strategyOfChoice]->route = shortestData;
		if (!CharacterManager::Instance()->Right()->GetNowBreak())
		{
			strategyArray[strategyOfChoice]->Update();
		}
		if (CharacterManager::Instance()->Right()->GetNowBreak() || CharacterAIOrder::Instance()->stopFlag)
		{
			CharacterAIOrder::Instance()->vel = { 0.0f,0.0f };
		}


		//受け渡し-----------------------------------------------------
		startPoint = strategyArray[strategyOfChoice]->startPoint;
		endPoint = strategyArray[strategyOfChoice]->endPoint;
		startFlag = strategyArray[strategyOfChoice]->startFlag;
		CharacterManager::Instance()->Right()->vel = CharacterAIOrder::Instance()->vel;


		float rate = strategyArray[strategyOfChoice]->GetGaugeStatus();

		Vec2<float>adjPos = { 0.0f,-120.0f };
		behaviorGauge->Update(CharacterManager::Instance()->Right()->pos + adjPos, strategyOfChoice, rate);
	}



}

void CharacterAI::Draw()
{
#ifdef DEBUG
	if (initFlag)
	{
		for (int i = 0; i < strategyArray.size(); ++i)
		{
			strategyArray[i]->Draw();
		}
	}
#endif
	if (useAiFlag)
	{
		behaviorGauge->Draw(false);
	}
}
