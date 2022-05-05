#include "StrategicLayer.h"
#include"DebugKeyManager.h"
#include"StaminaItemMgr.h"
#include"BulletCollision.h"
#include"../IntoTheAbyss/CharacterManager.h"

const float RestoreStamina::SEARCH_RADIUS = 500.0f;

RestoreStamina::RestoreStamina(const std::shared_ptr<FollowPath> &FOLLOW_PATH, const std::shared_ptr<MovingBetweenTwoPoints> &MOVING_BETWEEN_TOW_POINTS, const std::vector<std::vector<std::shared_ptr<WayPointData>>> &WAYPOINTS) :followPath(FOLLOW_PATH), moveToOhterPlace(MOVING_BETWEEN_TOW_POINTS)
{
	searchStartPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	searchGoalPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	readyToFollowPathFlag = false;
	readyToGoToGoalFlag = false;
	initRouteFlag = false;

	seachItemFlag = true;
	searchItemIndex = -1;
	prevStartHandle = startPoint.handle;

	searchArea.center = &CharacterManager::Instance()->Right()->pos;
	searchArea.radius = SEARCH_RADIUS;

	initFlag = true;
	getFlag = false;
}

void RestoreStamina::Update()
{
	std::array<StaminaItem, 100>item = StaminaItemMgr::Instance()->GetItemArray();

	if (!getFlag)
	{
		//�A�C�e���T�����J�n
		if (seachItemFlag)
		{
			SearchData result = SearchItem(searchArea);

			if (result.itemIndex != -1)
			{
				seachItemFlag = false;
				initFlag = false;
				//�{�X����ł��߂��E�F�C�|�C���g���X�^�[�g�n�_�Ƃ���------------------------
				searchStartPoint->Init(*searchArea.center);
				startPoint = searchStartPoint->Update();
				startPos = startPoint.pos;
				//�{�X����ł��߂��E�F�C�|�C���g���X�^�[�g�n�_�Ƃ���------------------------
				//�S�[���n�_����ł��߂��E�F�C�|�C���g���S�[���n�_�Ƃ���--------------------------
				searchGoalPoint->Init(*item[searchItemIndex].GetCollisionData()->center);
				endPoint = searchGoalPoint->Update();
				endPos = endPoint.pos;
				//�S�[���n�_����ł��߂��E�F�C�|�C���g���S�[���n�_�Ƃ���--------------------------
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

		//�X�^�[�g�n�_���ς�����炻�̏ꏊ�Ɍ������悤����������
		if (!initFlag)
		{
			//�{�X�̌��݂̍��W�ƃX�^�[�g�n�_�̈ړ�
			moveToOhterPlace->Init(*searchArea.center, startPos);
			initFlag = true;
		}
		prevStartHandle = startPoint.handle;
		moveToOhterPlace->Update();


		//�T�����J�n���A������--------------------------
		//�߂��̃E�F�C�|�C���g�Ɉړ�����
		if (moveToOhterPlace->CurrentProgress() == AiResult::OPERATE_SUCCESS && !readyToFollowPathFlag)
		{
			readyToFollowPathFlag = true;
			startFlag = true;
		}
		if (route.size() != 0 && !initRouteFlag)
		{
			followPath->Init(route);
			initRouteFlag = true;
		}

		//�E�F�C�|�C���g�ɉ����Ĉړ�����
		if (readyToFollowPathFlag && !readyToGoToGoalFlag && route.size() != 0)
		{
			followPath->Update();
		}
		//�S�[���܂ł��ǂ蒅������ړI�n�܂Ō�����
		if (followPath->CurrentProgress() == AiResult::OPERATE_SUCCESS && !readyToGoToGoalFlag)
		{
			readyToGoToGoalFlag = true;
			moveToOhterPlace->Init(endPos, *item[searchItemIndex].GetCollisionData()->center);
		}

		//�T�����J�n���A������--------------------------
	}

	//���ԓ��Ɋl��������܂��߂��̃A�C�e����T���B��莞�ԓ��ɂ��ǂ蒅���Ȃ���Ύ��s
	if (!seachItemFlag && item[searchItemIndex].GetIsAcquired())
	{
		getFlag = true;
	}

	//��莞�ԓ��Ɉ��ʉ񕜂����琬���A�o���Ȃ���Ύ��s


}

AiResult RestoreStamina::CurrentProgress()
{
	return AiResult();
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


	return static_cast<float>(evaluationValue / 10);
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
	result.distance = -1.0f;
	result.itemIndex = -1;
	float minDistance = 10000.0f;
	for (int i = 0; i < distance.size(); ++i)
	{
		if (distance[i] < minDistance)
		{
			result.distance = minDistance;
			result.itemIndex = itemId[i];
		}
	}

	return result;
}
