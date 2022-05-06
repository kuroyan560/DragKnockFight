#include "StrategicLayer.h"
#include"DebugKeyManager.h"
#include"StaminaItemMgr.h"
#include"BulletCollision.h"
#include"../IntoTheAbyss/CharacterManager.h"

const float RestoreStamina::SEARCH_RADIUS = 500.0f;
const int RestoreStamina::SUCCEED_GAIN_STAMINA_VALUE = 3;

RestoreStamina::RestoreStamina(const std::shared_ptr<FollowPath> &FOLLOW_PATH, const std::shared_ptr<MovingBetweenTwoPoints> &MOVING_BETWEEN_TOW_POINTS, const std::vector<std::vector<std::shared_ptr<WayPointData>>> &WAYPOINTS) :followPath(FOLLOW_PATH)
{
	searchStartPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	searchGoalPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	initRouteFlag = true;

	seachItemFlag = true;
	searchItemIndex = -1;
	prevItemIndex = -1;
	prevStartHandle = startPoint.handle;

	searchArea.center = &CharacterManager::Instance()->Right()->pos;
	searchArea.radius = SEARCH_RADIUS;

	initFlag = true;
	getFlag = false;

	moveToOnwGround = std::make_unique<MoveToOwnGround>(followPath);
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
			searchGoalPoint->Init(*item[searchItemIndex].GetCollisionData()->center);
			endPoint = searchGoalPoint->Update();
			endPos = endPoint.pos;
			//�S�[���n�_����ł��߂��E�F�C�|�C���g���S�[���n�_�Ƃ���--------------------------

			prevItemIndex = result.itemIndex;
			initRouteFlag = false;
			getFlag = true;
		}
	}
	else
	{
		//�T���͈͓��Ɏw��̃A�C�e�������邩�ǂ�������
		//�����Ȃ�����Č�����������
		if (!BulletCollision::Instance()->CheckSphereAndSphere(*item[searchItemIndex].GetCollisionData(), searchArea))
		{
			seachItemFlag = true;
		}
	}


	//�A�C�e���������ꍇ�͊�{�w�n�Ɍ�����
	if (!getFlag)
	{
		moveToOnwGround->route = route;
		moveToOnwGround->Update();
		startPoint = moveToOnwGround->startPoint;
		endPoint = moveToOnwGround->endPoint;
	}
	//�A�C�e�������������炻�̕����Ɍ�����
	else
	{
		//�{�X����ł��߂��E�F�C�|�C���g���X�^�[�g�n�_�Ƃ���------------------------
		if (startPoint.handle != endPoint.handle)
		{
			searchStartPoint->Init(*searchArea.center);
			startPoint = searchStartPoint->Update();
			startPos = startPoint.pos;
		}
		//�{�X����ł��߂��E�F�C�|�C���g���X�^�[�g�n�_�Ƃ���------------------------

		//�T�����J�n���A������--------------------------
		if (route.size() != 0 && (!initRouteFlag || prevStartHandle != startPoint.handle))
		{
			followPath->Init(route);
			initRouteFlag = true;
		}
		prevStartHandle = startPoint.handle;

		//�E�F�C�|�C���g�ɉ����Ĉړ�����
		followPath->Update();
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

AiResult RestoreStamina::CurrentProgress()
{
	//��莞�ԓ��Ɉ��ʉ񕜂����琬���A�o���Ȃ���Ύ��s
	int sub = staminaGauge - CharacterAIData::Instance()->bossData.stamineGauge;
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
	//�}�b�v�`�b�v�ԍ�
	Vec2<int>bossHandle;
	bossHandle.x = static_cast<int>(CharacterManager::Instance()->Right()->pos.x / 50.0f);
	bossHandle.y = static_cast<int>(CharacterManager::Instance()->Right()->pos.y / 50.0f);
	Vec2<int>playerHandle;
	playerHandle.x = static_cast<int>(CharacterManager::Instance()->Left()->pos.x / 50.0f);
	playerHandle.y = static_cast<int>(CharacterManager::Instance()->Left()->pos.y / 50.0f);



	//�D���Q�[�W n6~7
	if (0.6f <= data->bossData.gaugeValue && data->bossData.gaugeValue <= 0.7f)
	{
		evaluationValue += 2;
	}
	//�X�^�~�i���������G
	if (data->playerData.stamineGauge < data->bossData.stamineGauge)
	{
		evaluationValue += 2;
	}

	const float nearDistance = 200.0f;
	//�����ƕǂƂ̋������߂�
	if (!data->wayPoints[bossHandle.y][bossHandle.x]->isWall)
	{
		bool nearTopFlag = data->wayPoints[bossHandle.y][bossHandle.x]->wallDistanceTop < nearDistance;
		bool nearBottomFlag = data->wayPoints[bossHandle.y][bossHandle.x]->wallDistanceBottom < nearDistance;
		bool nearLeftFlag = data->wayPoints[bossHandle.y][bossHandle.x]->wallDistanceLeft < nearDistance;
		bool nearRightFlag = data->wayPoints[bossHandle.y][bossHandle.x]->wallDistanceRight < nearDistance;

		if (nearTopFlag || nearBottomFlag || nearLeftFlag || nearRightFlag)
		{
			evaluationValue += 2;
		}
	}
	//�G�ƕǂƂ̋������߂��Ȃ�
	if (!data->wayPoints[playerHandle.y][playerHandle.x]->isWall)
	{
		bool farTopFlag = nearDistance < data->wayPoints[playerHandle.y][playerHandle.x]->wallDistanceTop;
		bool farBottomFlag = nearDistance < data->wayPoints[playerHandle.y][playerHandle.x]->wallDistanceBottom;
		bool farLeftFlag = nearDistance < data->wayPoints[playerHandle.y][playerHandle.x]->wallDistanceLeft;
		bool farRightFlag = nearDistance < data->wayPoints[playerHandle.y][playerHandle.x]->wallDistanceRight;

		if (farTopFlag || farBottomFlag || farLeftFlag || farRightFlag)
		{
			evaluationValue += 1;
		}
	}

	//�G��莩���̕����A�C�e�����߂�
	//������
	SearchData bossResult = SearchItem(searchArea);
	//�G��
	SphereCollision hitBox;
	hitBox.center = &CharacterManager::Instance()->Left()->pos;
	hitBox.radius = SEARCH_RADIUS;
	SearchData playerResult = SearchItem(hitBox);
	if (playerResult.distance < bossResult.distance)
	{
		evaluationValue += 2;
	}

	//�G�̐U��񂵓��͂�������
	if (data->swingFlag)
	{
		evaluationValue += 2;
	}


	//���w�ւ̃E�F�C�|�C���g�����Ȃ�


	return static_cast<float>(evaluationValue / data->EVALUATION_MAX_VALUE);
}

RestoreStamina::SearchData RestoreStamina::SearchItem(const SphereCollision &DATA)
{
	std::array<StaminaItem, 100>item = StaminaItemMgr::Instance()->GetItemArray();

	std::vector<float>distance;
	std::vector<int>itemId;
	//�T���͈͓��ɃA�C�e��������̂����ׂ�
	for (int i = 0; i < item.size(); ++i)
	{
		//�A�C�e������ȏ㌩������T������������
		//�����ċ����𑪂�
		if (BulletCollision::Instance()->CheckSphereAndSphere(*item[i].GetCollisionData(), DATA))
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

const float AcquireASuperiorityGauge::SUCCEED_GAUGE_VALUE = 0.3f;

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
}

void AcquireASuperiorityGauge::Update()
{
	//�E�F�C�|�C���g
	const std::vector<std::vector<std::shared_ptr<WayPointData>>> wayPoints = CharacterAIData::Instance()->wayPoints;

	//�������L���s�������f����
	//�N���b�V�������ʒu������Ȃ��ʒu���Ŕ��f����


	//�������L���Ȃ�N���b�V�����ɍs��
	if (crashEnemyFlag)
	{

	}

	//�������s���Ȃ�N���b�V������Ȃ��悤�ɗ������
	if (dontCrashFlag)
	{

	}


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

GoToTheField::GoToTheField(const std::shared_ptr<FollowPath> &OPERATE)
{
	moveToOnwGround = std::make_unique<MoveToOwnGround>(OPERATE);
}

void GoToTheField::Init()
{
	timer = 0;
	timeOver = 60 * 10;
	startFlag = false;
	goToTheFieldFlag = true;
}

void GoToTheField::Update()
{
	//���������w�ɋ߂Â�
	if (goToTheFieldFlag)
	{
		moveToOnwGround->Update();
		startPoint = moveToOnwGround->startPoint;
		endPoint = moveToOnwGround->endPoint;
		startFlag = true;
	}
	//�������G�w�ɋ߂Â��Ȃ�
	else
	{

	}

	//�G��U��񂵂ňړ�������
	if (!CharacterManager::Instance()->Right()->IsHitWall())
	{
	}
	//�G���_�b�V���ňړ�������
	if (CharacterAIData::Instance()->dashFlag)
	{
	}
}

AiResult GoToTheField::CurrentProgress()
{
	if (timer < timeOver)
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
