#include "StrategicLayer.h"
#include"DebugKeyManager.h"
#include"StaminaItemMgr.h"
#include"BulletCollision.h"
#include"../IntoTheAbyss/CharacterManager.h"

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
	searchArea.radius = 500.0f;

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
			std::vector<float>distance;
			std::vector<int>itemId;
			//�T���͈͓��ɃA�C�e��������̂����ׂ�
			for (int i = 0; i < item.size(); ++i)
			{
				//�A�C�e������ȏ㌩������T������������
				//�����ċ����𑪂�
				if (BulletCollision::Instance()->CheckSphereAndSphere(*item[i].GetCollisionData(), searchArea))
				{
					distance.push_back(searchArea.center->Distance(*item[i].GetCollisionData()->center));
					itemId.push_back(i);
					seachItemFlag = false;
					initFlag = false;
				}
			}

			//�T���͈͓������ԋ߂��A�C�e��������
			float minDistance = 10000.0f;
			for (int i = 0; i < distance.size(); ++i)
			{
				if (distance[i] < minDistance)
				{
					minDistance = distance[i];
					searchItemIndex = itemId[i];
				}
			}

			if (distance.size() != 0)
			{
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
