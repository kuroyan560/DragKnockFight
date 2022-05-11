#include "TacticalLayer.h"
#include"CharacterManager.h"
#include"StageMgr.h"

FollowPath::FollowPath()
{
	goalFlag = false;
}

void FollowPath::Init(const std::vector<WayPointData> &ROUTE)
{
	route = ROUTE;
	routeHandle = 0;
	goalFlag = false;
	stackFlag = false;
	operateFollow.Init(route[0].pos, route[1].pos);

	initFlag = false;
}

void FollowPath::Update()
{
	//�X�^�b�N���肪�o����X�^�b�N�p�̏����𓮂����A����ȊO�̓p�X��H��
	if (!stackFlag)
	{
		operateFollow.Update();
	}
	else
	{
		dontStack.Update();
	}

	AiResult result = operateFollow.CurrentProgress();

	//����������ʂ̓�_�Ԃ̈ړ����s��
	if (result == AiResult::OPERATE_SUCCESS)
	{
		++routeHandle;
		//�ŒZ���[�g�̃S�[���܂ŒH�蒅����
		if (route.size() <= routeHandle || route.size() <= routeHandle + 1)
		{
			goalFlag = true;
		}
		//���ǂ蒅���܂œ�_�Ԃ̈ړ��̑�����
		else
		{
			operateFollow.Init(route[routeHandle].pos, route[routeHandle + 1].pos);
		}
	}

	//�{�X����������̕��@�œ�������������Ă��Ȃ�
	bool canMoveFlag = CharacterManager::Instance()->Right()->GetNowSwing();

	//�������~�܂��Ă��邱�Ƃ��m�F������X�^�b�N���Ȃ��ꏊ��T��
	if (result == AiResult::OPERATE_FAIL && routeHandle < route.size() && initFlag)
	{
		dontStack.Init(CharacterManager::Instance()->Right()->pos);
		stackFlag = true;
	}
	//�����ł����s�ł��X�^�b�N��Ԃ���ړ���������
	if (dontStack.CurrentProgress() != AiResult::OPERATE_INPROCESS)
	{
		stackFlag = false;
	}

	initFlag = true;
}

AiResult FollowPath::CurrentProgress()
{
	if (goalFlag)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
}



MoveToOwnGround::MoveToOwnGround()
{
	timer = 0;
	timeOver = 60 * 3;
}

void MoveToOwnGround::Init()
{

}

void MoveToOwnGround::Update()
{
	//�T���J�n
	if (route.size() != 0 && (startPoint.handle != prevStartHandle || endPoint.handle != prevEndHandle))
	{
		followPath.Init(route);
		prevStartHandle = startPoint.handle;
		prevEndHandle = endPoint.handle;
	}
	followPath.Update();


	//�X�^�[�g�n�_�̒T��
	searchPoint.Init(CharacterManager::Instance()->Right()->pos);
	startPoint = searchPoint.Update();

	//��莞�Ԃ��Ƃɉ������S�[���ɂ��邩�؂�ւ���
	if (timeOver <= timer || timer == 0)
	{
		//�S�[���n�_�̒T��
		int goalXPoint = CharacterAIData::Instance()->wayPoints[0].size() - 3;

		//�w�n����Y���W�����_���Ŏ��
		int goalYPoint = KuroFunc::GetRand(0, CharacterAIData::Instance()->wayPoints.size() - 1);
		while (CharacterAIData::Instance()->wayPoints[goalYPoint][goalXPoint].isWall)
		{
			goalYPoint = KuroFunc::GetRand(0, CharacterAIData::Instance()->wayPoints.size() - 1);
		}
		endPoint = CharacterAIData::Instance()->wayPoints[goalYPoint][goalXPoint];
		timer = 1;
	}
	++timer;
}

AiResult MoveToOwnGround::CurrentProgress()
{
	return AiResult();
}