#include "TacticalLayer.h"

FollowPath::FollowPath(const std::shared_ptr<MovingBetweenTwoPoints> &OPERATE)
{
	operateFollow = OPERATE;
}

void FollowPath::Init(const std::vector<WayPointData> &ROUTE)
{
	route = ROUTE;
	routeHandle = 0;
	goalFlag = false;
	operateFollow->Init(route[0].pos, route[1].pos);
}

void FollowPath::Update()
{
	//����E�F�C�|�C���g�Ɨ���Ă�����߂Â�
	//��_�Ԃ̈ړ�
	operateFollow->Update();

	//����������ʂ̓�_�Ԃ̈ړ����s��
	if (operateFollow->CurrentProgress() == AiResult::OPERATE_SUCCESS)
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
			operateFollow->Init(route[routeHandle].pos, route[routeHandle + 1].pos);
		}
	}
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
