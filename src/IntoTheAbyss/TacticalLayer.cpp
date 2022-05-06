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
	operateFollow.Init(route[0].pos, route[1].pos);
}

void FollowPath::Update()
{
	//����E�F�C�|�C���g�Ɨ���Ă�����߂Â�
	//��_�Ԃ̈ړ�
	operateFollow.Update();

	//����������ʂ̓�_�Ԃ̈ړ����s��
	if (operateFollow.CurrentProgress() == AiResult::OPERATE_SUCCESS)
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
		Vec2<float>goalPos(CharacterManager::Instance()->Right()->pos);
		//X���͎��w�AY���̓{�X�̍��W�ɍ��킹�����W
		goalPos.x = (StageMgr::Instance()->GetMapChipData(0, 0)[0].size() * 50.0f) - 150.0f;
		searchPoint.Init(goalPos);
		endPoint = searchPoint.Update();
		timer = 1;
	}
	++timer;
}

AiResult MoveToOwnGround::CurrentProgress()
{
	return AiResult();
}
