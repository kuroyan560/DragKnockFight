#include "BossPatternSwing.h"
#include "../IntoTheAbyss/SwingMgr.h"

BossPatternSwing::BossPatternSwing()
{
}

void BossPatternSwing::Init()
{
}

void BossPatternSwing::Update(BossPatternData *DATA)
{

	if (!swingFlag)
	{
		// �U��񂵂̊J�n�x�N�g�����擾�B
		SwingMgr::Instance()->easingStartVec = SwingMgr::Instance()->playerPos - SwingMgr::Instance()->bossPos;
		SwingMgr::Instance()->easingStartVec.Normalize();
		SwingMgr::Instance()->easingNowVec = SwingMgr::Instance()->easingStartVec;

		// �U��񂵂̏I���x�N�g�����擾�B
		SwingMgr::Instance()->easingEndVec = (SwingMgr::Instance()->playerPos - SwingMgr::Instance()->bossPos) * Vec2<float>(1.0f, -1.0f);
		SwingMgr::Instance()->easingEndVec.Normalize();

		// �U��񂵃t���O��L��������B
		SwingMgr::Instance()->isSwingPlayer = false;
		SwingMgr::Instance()->isSwingBoss = true;

		// �e�^�C�}�[���������B
		SwingMgr::Instance()->easingTimer = 0;
		SwingMgr::Instance()->easeAmount = 0;
		SwingMgr::Instance()->easeChangeAmountY = SwingMgr::Instance()->easingEndVec.y - SwingMgr::Instance()->easingStartVec.y;
		swingFlag = true;
	}
	//// �N�[���^�C����ݒ�B
	//swingCoolTime = SWING_COOLTIME;

	//SwingMgr::Instance()->PlaySE();
}
