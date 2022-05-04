#include "StrategicLayer.h"
#include"DebugKeyManager.h"

RestoreStamina::RestoreStamina(const std::shared_ptr<FollowPath> &FOLLOW_PATH, const std::shared_ptr<MovingBetweenTwoPoints> &MOVING_BETWEEN_TOW_POINTS, const std::array<std::array<std::shared_ptr<WayPointData>, NavigationAI::WAYPOINT_MAX_Y>, NavigationAI::WAYPOINT_MAX_X> &WAYPOINTS) :followPath(FOLLOW_PATH), moveToOhterPlace(MOVING_BETWEEN_TOW_POINTS)
{
	searchStartPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	searchGoalPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	initFlag = false;
	readyToFollowPathFlag = false;
	readyToGoToGoalFlag = false;
	dEndPos = { 1300.0f,400.0f };
	initRouteFlag = false;
}

void RestoreStamina::Update()
{
	//�T���͈͓��ɃA�C�e��������̂����ׂ�
	if (1)
	{

	}

	if (DebugKeyManager::Instance()->DebugKeyTrigger(DIK_F, "BossStart", "DIK_F"))
	{
		initFlag = true;
	}

	//�T�������A�C�e������ߏ�̃E�F�C�|�C���g�𒲂ׂ�B�������炻�̃E�F�C�|�C���g���S�[���Ƃ���
	if (!initFlag)
	{
		//�{�X����ł��߂��E�F�C�|�C���g���X�^�[�g�n�_�Ƃ���
		searchStartPoint->Init(dStartPos);
		startPoint = searchStartPoint->Update();
		startPos = startPoint.pos;

		searchGoalPoint->Init(dEndPos);
		endPoint = searchGoalPoint->Update();
		endPos = endPoint.pos;

		//�{�X�̌��݂̍��W�ƃX�^�[�g�n�_�̈ړ�
		moveToOhterPlace->Init(dStartPos, startPos);
		initFlag = true;
	}
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
		moveToOhterPlace->Init(endPos, dEndPos);
	}

	//�T�����J�n���A������--------------------------


	//���ԓ��Ɋl��������܂��߂��̃A�C�e����T���B��莞�ԓ��ɂ��ǂ蒅���Ȃ���Ύ��s

	//��莞�ԓ��Ɉ��ʉ񕜂����琬���A�o���Ȃ���Ύ��s


}

AiResult RestoreStamina::CurrentProgress()
{
	return AiResult();
}
