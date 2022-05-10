#include "StrategicLayer.h"
#include"DebugKeyManager.h"
#include"StaminaItemMgr.h"
#include"BulletCollision.h"
#include"../IntoTheAbyss/CharacterManager.h"
#include"Stamina.h"
#include"ScrollMgr.h"
#include"StageMgr.h"

const float IStrategicLayer::SEARCH_RADIUS = 500.0f;
const float RestoreStamina::SUCCEED_GAIN_STAMINA_VALUE = 0.4f;
const float AcquireASuperiorityGauge::SUCCEED_GAUGE_VALUE = 0.3f;

IStrategicLayer::SearchData IStrategicLayer::SearchItem(const SphereCollision &DATA)
{
	std::array<StaminaItem, 100>item = StaminaItemMgr::Instance()->GetItemArray();

	float mapX = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum())[0].size() * 50.0f;
	float position = CharacterManager::Instance()->Right()->pos.x / mapX;
	float rate = 1.0f;
	//�|�W�V�����Q�[�W��0.5�ȉ��̏ꍇ�A�G�w�ɋ߂Â��Ă��鎖�̂Ȃ̂ō��W�𓮂�������������
	if (position <= 0.5f)
	{
		float adjPosition = position - 0.3f;
		rate = adjPosition / 0.2f;
	}
	Vec2<float> adjPos = Vec2<float>((SEARCH_RADIUS / 2.0f) * (1.0 - rate), 0.0f);
	searchCollision.center = &adjPos;
	searchCollision.radius = SEARCH_RADIUS;



	std::vector<float>distance;
	std::vector<int>itemId;
	//�T���͈͓��ɃA�C�e��������̂����ׂ�
	for (int i = 0; i < item.size(); ++i)
	{
		//�A�C�e������ȏ㌩������T������������
		//�����ċ����𑪂�
		bool canGetFlag = item[i].GetIsActive() && !item[i].GetIsAcquired();
		if (canGetFlag && BulletCollision::Instance()->CheckSphereAndSphere(*item[i].GetCollisionData(), searchCollision))
		{
			distance.push_back(DATA.center->Distance(*item[i].GetCollisionData()->center));
			itemId.push_back(i);
		}
	}

	//�T���͈͓������ԋ߂��A�C�e��������
	SearchData result;
	result.distance = 10000.0f;
	result.itemIndex = -1;
	for (int i = 0; i < distance.size(); ++i)
	{
		if (distance[i] < result.distance)
		{
			result.distance = distance[i];
			result.itemIndex = itemId[i];
		}
	}

	return result;
}

RestoreStamina::RestoreStamina()
{
	initRouteFlag = true;

	seachItemFlag = true;
	searchItemIndex = -1;
	prevItemIndex = -1;
	prevStartHandle = startPoint.handle;

	searchArea.center = &CharacterManager::Instance()->Right()->pos;
	searchArea.radius = SEARCH_RADIUS;

	initFlag = true;
	getFlag = false;

	startFlag = true;
}

void RestoreStamina::Init()
{
	staminaGauge = CharacterAIData::Instance()->bossData.stamineGauge;
	timer = 0;
	timeOver = 60 * 10;
}

void RestoreStamina::Update()
{
	std::array<StaminaItem, 100>item = StaminaItemMgr::Instance()->GetItemArray();


	//�A�C�e���T�����J�n
	if (seachItemFlag)
	{
		SearchData result = SearchItem(searchArea);

		if (result.itemIndex != -1 && result.itemIndex != prevItemIndex)
		{
			searchItemIndex = result.itemIndex;
			seachItemFlag = false;
			initFlag = false;
			//�S�[���n�_����ł��߂��E�F�C�|�C���g���S�[���n�_�Ƃ���--------------------------
			searchGoalPoint.Init(*item[searchItemIndex].GetCollisionData()->center);
			endPoint = searchGoalPoint.Update();
			//�S�[���n�_����ł��߂��E�F�C�|�C���g���S�[���n�_�Ƃ���--------------------------

			prevItemIndex = result.itemIndex;
			initRouteFlag = false;
			getFlag = true;
		}
		else
		{
			seachItemFlag = false;
			getFlag = false;
		}
	}
	else
	{
		//�T���͈͓��Ɏw��̃A�C�e�������邩�ǂ�������
		//�����Ȃ�����Č�����������
		if (searchItemIndex != -1 && !BulletCollision::Instance()->CheckSphereAndSphere(*item[searchItemIndex].GetCollisionData(), searchArea))
		{
			seachItemFlag = true;
		}
	}


	//�A�C�e���������ꍇ�͊�{�w�n�Ɍ�����
	if (!getFlag)
	{
		moveToOnwGround.route = route;
		moveToOnwGround.Update();
		startPoint = moveToOnwGround.startPoint;
		endPoint = moveToOnwGround.endPoint;
	}
	//�A�C�e�������������炻�̕����Ɍ�����
	else
	{
		//�{�X����ł��߂��E�F�C�|�C���g���X�^�[�g�n�_�Ƃ���------------------------
		if (startPoint.handle != endPoint.handle)
		{
			searchStartPoint.Init(*searchArea.center);
			startPoint = searchStartPoint.Update();
		}
		//�{�X����ł��߂��E�F�C�|�C���g���X�^�[�g�n�_�Ƃ���------------------------

		//�T�����J�n���A������--------------------------
		if (route.size() != 0 && (!initRouteFlag || prevStartHandle != startPoint.handle))
		{
			followPath.Init(route);
			initRouteFlag = true;
		}
		prevStartHandle = startPoint.handle;

		//�E�F�C�|�C���g�ɉ����Ĉړ�����
		followPath.Update();
		//�T�����J�n���A������--------------------------


		//���ԓ��Ɋl��������܂��߂��̃A�C�e����T���B��莞�ԓ��ɂ��ǂ蒅���Ȃ���Ύ��s
		if (!seachItemFlag && item[searchItemIndex].GetIsAcquired())
		{
			seachItemFlag = true;
			getFlag = false;
		}
	}

	//�헪���s����̌o�ߎ���
	++timer;
}

void RestoreStamina::Draw()
{
	float mapX = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum())[0].size() * 50.0f;

	float position = CharacterManager::Instance()->Right()->pos.x / mapX;
	float rate = 1.0f;
	//�|�W�V�����Q�[�W��0.5�ȉ��̏ꍇ�A�G�w�ɋ߂Â��Ă��鎖�̂Ȃ̂ō��W�𓮂�������������
	if (position <= 0.5f)
	{
		float adjPosition = position - 0.3f;
		rate = adjPosition / 0.2f;
	}
	Vec2<float> adjPos = Vec2<float>((SEARCH_RADIUS / 2.0f) * (1.0 - rate), 0.0f);
	DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(CharacterManager::Instance()->Right()->pos + adjPos), ScrollMgr::Instance()->zoom * searchCollision.radius, Color(255, 255, 255, 255));
}

AiResult RestoreStamina::CurrentProgress()
{
	//��莞�ԓ��Ɉ��ʉ񕜂����琬���A�o���Ȃ���Ύ��s
	float sub = CharacterAIData::Instance()->bossData.stamineGauge - staminaGauge;
	if (SUCCEED_GAIN_STAMINA_VALUE <= sub && timer < timeOver)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else if (timeOver <= timer)
	{
		return AiResult::OPERATE_FAIL;
	}
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
}

float RestoreStamina::EvaluationFunction()
{
	CharacterAIData *data = CharacterAIData::Instance();
	//�]���l
	int evaluationValue = 0;

	//�D���Q�[�W n6~7
	if (0.6f <= data->bossData.gaugeValue && data->bossData.gaugeValue <= 0.7f)
	{
		evaluationValue += 2;
	}

	const float LESS_OF_MY_OWN_STAMINA = 0.5f;
	//�X�^�~�i�����Ȃ��Ȃ���
	if (data->bossData.stamineGauge < LESS_OF_MY_OWN_STAMINA)
	{
		evaluationValue += 2;
	}


	//�E�F�C�|�C���g�T��
	SearchWayPoint search;

	//�{�X����ߏ�̃E�F�C�|�C���g�T��
	search.Init(CharacterManager::Instance()->Right()->pos);
	WayPointData bossNearWayPoint = search.Update();

	const float nearDistance = 200.0f;
	//�����ƕǂƂ̋������߂�
	if (!bossNearWayPoint.isWall)
	{
		bool nearTopFlag = bossNearWayPoint.wallDistanceTop < nearDistance;
		bool nearBottomFlag = bossNearWayPoint.wallDistanceBottom < nearDistance;
		bool nearLeftFlag = bossNearWayPoint.wallDistanceLeft < nearDistance;
		bool nearRightFlag = bossNearWayPoint.wallDistanceRight < nearDistance;

		if (nearTopFlag || nearBottomFlag || nearLeftFlag || nearRightFlag)
		{
			evaluationValue += 2;
		}
	}


	search.Init(CharacterManager::Instance()->Left()->pos);
	WayPointData playerNearWayPoint = search.Update();

	//�G�ƕǂƂ̋������߂��Ȃ�
	if (!playerNearWayPoint.isWall)
	{
		bool farTopFlag = nearDistance < playerNearWayPoint.wallDistanceTop;
		bool farBottomFlag = nearDistance < playerNearWayPoint.wallDistanceBottom;
		bool farLeftFlag = nearDistance < playerNearWayPoint.wallDistanceLeft;
		bool farRightFlag = nearDistance < playerNearWayPoint.wallDistanceRight;

		if (farTopFlag || farBottomFlag || farLeftFlag || farRightFlag)
		{
			evaluationValue += 1;
		}
	}

	//�����̕����A�C�e�����߂�
	SearchData bossResult = SearchItem(searchArea);
	if (bossResult.distance < SEARCH_RADIUS)
	{
		evaluationValue += 2;
	}

	//�G�̐U��񂵓��͂�������
	if (data->swingFlag)
	{
		evaluationValue += 2;
	}

	//�����Ǝ��w�Ƃ̋������߂�
	const float NEAR_LINE_RATE = 0.7f;
	bool nearFlag = NEAR_LINE_RATE < CharacterAIData::Instance()->position;
	if (nearFlag)
	{
		evaluationValue += 2;
	}

	//�G�ƓG�w�Ƃ̋������߂�
	const float FAR_LINE_RATE = 0.3f;
	bool farFlag = 1.0f - CharacterAIData::Instance()->position < FAR_LINE_RATE;
	if (farFlag)
	{
		evaluationValue += 2;
	}

	//���w�ւ̃E�F�C�|�C���g
	return static_cast<float>(evaluationValue) / static_cast<float>(data->EVALUATION_MAX_VALUE);
}


GoToTheField::GoToTheField()
{
}

void GoToTheField::Init()
{
	timer = 0;
	timeOver = 60 * 5;
	startFlag = false;
	goToTheFieldFlag = true;
	swingCoolTime = 0;
	CharacterAIOrder::Instance()->swingClockWiseFlag = false;
	CharacterAIOrder::Instance()->swingCounterClockWiseFlag = false;
}

void GoToTheField::Update()
{
	//���������w�ɋ߂Â�
	if (goToTheFieldFlag)
	{
		moveToOnwGround.route = route;
		moveToOnwGround.Update();
		startPoint = moveToOnwGround.startPoint;
		endPoint = moveToOnwGround.endPoint;
		startFlag = true;
	}
	//�������G�w�ɋ߂Â��Ȃ�
	else
	{
	}


	if (CharacterManager::Instance()->Right()->GetNowSwing())
	{
		swingCoolTime = 0;
	}

	//�U��񂵂��ۂɈ�苗���ȏ㗣���邩
	const float CERTAIN_DISTANCE = 100.0f;
	bool haveAdvantageToSwingClockWiseFlag = CERTAIN_DISTANCE <= CharacterAIData::Instance()->cDistance;
	bool haveAdvantageToSwingCClockWiseFlag = CERTAIN_DISTANCE <= CharacterAIData::Instance()->cCDistance;

	//�X�^�~�i������
	const float STAMINA_VALUE = 0.5f;
	bool useSwingFlag = STAMINA_VALUE <= CharacterAIData::Instance()->bossData.stamineGauge;
	//�U��񂵂̃N�[���^�C�����I�����
	bool timeToSiwngFlag = SWING_MAX_COOL_TIME <= swingCoolTime;


	//�X�^�~�i�������U��񂵂̃N�[���^�C�����I������ہA��苗������Ă�������s
	if (useSwingFlag && timeToSiwngFlag && (haveAdvantageToSwingClockWiseFlag || haveAdvantageToSwingCClockWiseFlag))
	{
		//�G��U��񂵂ňړ������鋗�����傫�����ɐU���
		if (CharacterAIData::Instance()->cCDistance < CharacterAIData::Instance()->cDistance)
		{
			CharacterAIOrder::Instance()->swingClockWiseFlag = true;
		}
		else
		{
			CharacterAIOrder::Instance()->swingCounterClockWiseFlag = true;
		}
	}

	++swingCoolTime;
	++timer;
}

AiResult GoToTheField::CurrentProgress()
{
	if (timeOver <= timer)
	{
		return AiResult::OPERATE_FAIL;
	}
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
}

float GoToTheField::EvaluationFunction()
{
	CharacterAIData *data = CharacterAIData::Instance();
	//�]���l
	int evaluationValue = 0;

	//�D���Q�[�W n6~7
	if (0.6f <= data->bossData.gaugeValue && data->bossData.gaugeValue <= 0.7f)
	{
		evaluationValue += 2;
	}

	const float MANY_OF_MY_OWN_STAMINA = 0.5f;
	//�X�^�~�i������
	if (MANY_OF_MY_OWN_STAMINA <= data->bossData.stamineGauge)
	{
		evaluationValue += 1;
	}

	//�E�F�C�|�C���g�T��
	SearchWayPoint search;

	//�{�X����ߏ�̃E�F�C�|�C���g�T��
	search.Init(CharacterManager::Instance()->Right()->pos);
	WayPointData bossNearWayPoint = search.Update();

	const float nearDistance = 200.0f;
	//�����ƕǂƂ̋������߂�
	if (!bossNearWayPoint.isWall)
	{
		bool farTopFlag = nearDistance < bossNearWayPoint.wallDistanceTop;
		bool farBottomFlag = nearDistance < bossNearWayPoint.wallDistanceBottom;
		bool farLeftFlag = nearDistance < bossNearWayPoint.wallDistanceLeft;
		bool farRightFlag = nearDistance < bossNearWayPoint.wallDistanceRight;

		if (farTopFlag || farBottomFlag || farLeftFlag || farRightFlag)
		{
			evaluationValue += 2;
		}
	}

	search.Init(CharacterManager::Instance()->Right()->pos);
	WayPointData playerNearWayPoint = search.Update();

	//�G�ƕǂƂ̋������߂��Ȃ�
	if (!playerNearWayPoint.isWall)
	{
		bool farTopFlag = nearDistance < playerNearWayPoint.wallDistanceTop;
		bool farBottomFlag = nearDistance < playerNearWayPoint.wallDistanceBottom;
		bool farLeftFlag = nearDistance < playerNearWayPoint.wallDistanceLeft;
		bool farRightFlag = nearDistance < playerNearWayPoint.wallDistanceRight;

		if (farTopFlag || farBottomFlag || farLeftFlag || farRightFlag)
		{
			evaluationValue += 1;
		}
	}

	SphereCollision searchArea;
	searchArea.center = &CharacterManager::Instance()->Right()->pos;
	searchArea.radius = SEARCH_RADIUS;

	//�G��莩���̕����A�C�e�����߂�
	//������
	SearchData bossResult = SearchItem(searchArea);
	//�G��
	SphereCollision hitBox;
	hitBox.center = &CharacterManager::Instance()->Left()->pos;
	hitBox.radius = SEARCH_RADIUS;
	SearchData playerResult = SearchItem(hitBox);
	const float distance = 200.0f;
	//�G�����ȏ�A�C�e�����痣��Ă�����A�G���ړ������邱�Ƃ�D���ł���悤�ɒl������
	if (distance < playerResult.distance)
	{
		evaluationValue += 2;
	}

	//�����Ǝ��w�Ƃ̋������߂�
	const float NEAR_LINE_RATE = 0.7f;
	bool nearFlag = NEAR_LINE_RATE < CharacterAIData::Instance()->position;
	if (nearFlag)
	{
		evaluationValue += 3;
	}

	//�G�ƓG�w�Ƃ̋������߂�
	const float FAR_LINE_RATE = 0.3f;
	bool farFlag = CharacterAIData::Instance()->position < FAR_LINE_RATE;
	if (farFlag)
	{
		evaluationValue += 3;
	}

	//���w�ւ̃E�F�C�|�C���g�����Ȃ�


	return static_cast<float>(evaluationValue) / static_cast<float>(data->EVALUATION_MAX_VALUE);
}


AcquireASuperiorityGauge::AcquireASuperiorityGauge()
{
}

void AcquireASuperiorityGauge::Init()
{
	nowGauge = CharacterAIData::Instance()->bossData.gaugeValue;
	timer = 0;
	timeOver = 60 * 10;

	crashEnemyFlag = false;
	dontCrashFlag = false;

	swingCoolTime = 0;
	CharacterAIOrder::Instance()->swingClockWiseFlag = false;
	CharacterAIOrder::Instance()->swingCounterClockWiseFlag = false;
}

void AcquireASuperiorityGauge::Update()
{
	////�E�F�C�|�C���g
	//const std::vector<std::vector<std::shared_ptr<WayPointData>>> wayPoints = CharacterAIData::Instance()->wayPoints;

	////�������L���s�������f����
	////�N���b�V�������ʒu������Ȃ��ʒu���Ŕ��f����

	////�������L���Ȃ�N���b�V�����ɍs��
	//if (crashEnemyFlag)
	//{

	//}

	////�������s���Ȃ�N���b�V������Ȃ��悤�ɗ������
	//if (dontCrashFlag)
	//{
	//}

	moveToOnwGround.route = route;
	moveToOnwGround.Update();
	startPoint = moveToOnwGround.startPoint;
	endPoint = moveToOnwGround.endPoint;
	startFlag = true;


	if (CharacterManager::Instance()->Right()->GetNowSwing() || CharacterManager::Instance()->Right()->GetNowBreak())
	{
		swingCoolTime = 0;
	}

	//�U��񂵉\��
	bool canSwingClockWiseFlag = CharacterManager::Instance()->Right()->ClockwiseHitsTheWall() && !CharacterManager::Instance()->Right()->GetNowSwing();
	bool canSwingCClockWiseFlag = CharacterManager::Instance()->Right()->CounterClockwiseHitsTheWall() && !CharacterManager::Instance()->Right()->GetNowSwing();

	const float STAMINA_VALUE = 0.5f;
	//�X�^�~�i������
	bool useSwingFlag = STAMINA_VALUE <= CharacterAIData::Instance()->bossData.stamineGauge && SWING_MAX_COOL_TIME <= swingCoolTime;

	//�G��U��񂵂ňړ�������
	if (canSwingClockWiseFlag && useSwingFlag && !CharacterManager::Instance()->Right()->GetNowBreak())
	{
		CharacterAIOrder::Instance()->swingClockWiseFlag = true;
		swingCoolTime = 0;
	}
	else if (canSwingCClockWiseFlag && useSwingFlag && !CharacterManager::Instance()->Right()->GetNowBreak())
	{
		CharacterAIOrder::Instance()->swingCounterClockWiseFlag = true;
		swingCoolTime = 0;
	}

	++swingCoolTime;


	//�헪���s��
	++timer;
}

AiResult AcquireASuperiorityGauge::CurrentProgress()
{
	//��莞�ԓ��Ɉ��ʉ񕜂����琬���A�o���Ȃ���Ύ��s
	int sub = nowGauge - CharacterAIData::Instance()->bossData.gaugeValue;
	if (SUCCEED_GAUGE_VALUE <= sub && timer < timeOver)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else if (timeOver <= timer)
	{
		return AiResult::OPERATE_FAIL;
	}
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
}

float AcquireASuperiorityGauge::EvaluationFunction()
{
	CharacterAIData *data = CharacterAIData::Instance();
	//�]���l
	int evaluationValue = 0;

	//�D���Q�[�W n6~7
	float sub = data->bossData.gaugeValue - data->bossData.gaugeValue;
	const float BIG_DIFFERENCE_IN_GAUGE = 0.3f;
	if (BIG_DIFFERENCE_IN_GAUGE <= fabs(sub))
	{
		evaluationValue += 3;
	}

	const float MANY_OF_MY_OWN_STAMINA = 0.5f;
	//�X�^�~�i������
	if (MANY_OF_MY_OWN_STAMINA <= data->bossData.stamineGauge)
	{
		evaluationValue += 2;
	}

	//�E�F�C�|�C���g�T��
	SearchWayPoint search;

	//�{�X����ߏ�̃E�F�C�|�C���g�T��
	search.Init(CharacterManager::Instance()->Right()->pos);
	WayPointData playerNearWayPoint = search.Update();

	const float nearDistance = 200.0f;
	//�G�ƕǂƂ̋������߂�
	if (!playerNearWayPoint.isWall)
	{
		bool nearTopFlag = playerNearWayPoint.wallDistanceTop < nearDistance;
		bool nearBottomFlag = playerNearWayPoint.wallDistanceBottom < nearDistance;
		bool nearLeftFlag = playerNearWayPoint.wallDistanceLeft < nearDistance;
		bool nearRightFlag = playerNearWayPoint.wallDistanceRight < nearDistance;

		if (nearTopFlag || nearBottomFlag || nearLeftFlag || nearRightFlag)
		{
			evaluationValue += 2;
		}
	}


	SphereCollision searchArea;
	searchArea.center = &CharacterManager::Instance()->Right()->pos;
	searchArea.radius = SEARCH_RADIUS;

	//�G��莩���̕����A�C�e�����߂�
	//������
	SearchData bossResult = SearchItem(searchArea);
	//�G��
	SphereCollision hitBox;
	hitBox.center = &CharacterManager::Instance()->Left()->pos;
	hitBox.radius = SEARCH_RADIUS;
	SearchData playerResult = SearchItem(hitBox);
	const float distance = 200.0f;
	//�G�����ȏ�A�C�e�����痣��Ă�����A�G���ړ������邱�Ƃ�D���ł���悤�ɒl������
	if (distance < playerResult.distance)
	{
		evaluationValue += 2;
	}


	{
		//�����Ǝ��w�Ƃ̋������߂�&&�D���Q�[�W���D��
		const float NEAR_LINE_RATE = 0.7f;
		bool nearFlag = NEAR_LINE_RATE < CharacterAIData::Instance()->position;
		const float ADVANTAGE_VALUE = 0.7f;
		bool advantageFlag = ADVANTAGE_VALUE <= data->bossData.gaugeValue;
		if (nearFlag)
		{
			evaluationValue += 3;
		}
	}

	{
		//�����ƓG�w�Ƃ̋������߂�&&�D���Q�[�W����
		const float NEAR_LINE_RATE = 0.3f;
		bool nearFlag = CharacterAIData::Instance()->position < NEAR_LINE_RATE;
		const float ADVANTAGE_VALUE = 0.3f;
		bool advantageFlag = data->bossData.gaugeValue <= ADVANTAGE_VALUE;
		if (nearFlag)
		{
			evaluationValue += 3;
		}
	}

	//�G�̐U��񂵓��͂�������
	if (data->swingFlag)
	{
		evaluationValue += 2;
	}

	return static_cast<float>(evaluationValue) / static_cast<float>(data->EVALUATION_MAX_VALUE);
}
