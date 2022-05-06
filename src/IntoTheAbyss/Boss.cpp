#include "Boss.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "MapChipCollider.h"
#include "WinApp.h"
#include "UsersInput.h"
#include "SuperiorityGauge.h"
#include "SuperiorityGauge.h"
#include "SlowMgr.h"
#include "StunEffect.h"

#include"TexHandleMgr.h"
#include"BossPatternNormalMove.h"
#include"BossPatternAttack.h"
#include"BossPatternSwing.h"
#include"CrashMgr.h"
#include"CharacterInterFace.h"

#include"ParticleMgr.h"
#include"DebugKeyManager.h"

#include"DebugParameter.h"
#include"AfterImage.h"

#include"CharacterAIData.h"


static const Vec2<float> SCALE = { 80.0f,80.0f };
Boss::Boss() :CharacterInterFace(SCALE)
{
	graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/enemy.png");
	graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/enemy_back.png");

	bossPattern[0] = std::make_unique<BossPatternNormalMove>();
	bossPattern[1] = std::make_unique<BossPatternAttack>();
	bossPattern[2] = std::make_unique<BossPatternSwing>();


	//�p�^�[���ɓn���f�[�^�̏�����
	patternData.moveVel = &moveVel;
}

void Boss::OnInit()
{
	/*===== �������� =====*/

	initScale = SCALE * 5.0f;
	size = initScale;
	moveVel = { 0,0 };
	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;
	prevIntersectedLine = INTERSECTED_NONE;
	moveTimer = 0;
	initPaticleFlag = false;

	bossPatternNow = BOSS_PATTERN_NORMALMOVE;
	patternTimer = 0;
}

#include"Camera.h"
void Boss::OnUpdate(const std::vector<std::vector<int>> &MapData)
{
	/*===== �X�V���� =====*/

	//if (Camera::Instance()->Active())
	//{
	//	moveVel = { 0,0 };
	//	return;
	//}

	// �p�[�g�i�[���U��񂵂Ă�����c�����o���B
	if (partner.lock()->GetNowSwing()) {

		DIR dir = FRONT;
		if (vel.y < 0)dir = BACK;
		AfterImageMgr::Instance()->Generate(pos, {}, 0, graphHandle[dir], Color(239, 1, 144, 255), true, size);

	}

	// �p�[�g�i�[���U��񂵏�Ԃ�������X�V�������s��Ȃ��B
	if (!(!partner.lock()->GetNowSwing() && !nowSwing)) return;

	for (int i = 0; i < patternData.nearLimmitLine.size(); ++i)
	{
		patternData.nearLimmitLine[i].startPos = pos;

		float angle = i * (360.0f / patternData.nearLimmitLine.size());
		float dir = Angle::ConvertToRadian(angle);
		float distance = 150.0f;
		patternData.nearLimmitLine[i].endPos = pos + Vec2<float>(cosf(dir), sinf(dir)) * distance;
	}

	for (int i = 0; i < patternData.farLimmitLine.size(); ++i)
	{
		patternData.farLimmitLine[i].startPos = pos;

		float angle = i * (360.0f / patternData.farLimmitLine.size());
		float dir = Angle::ConvertToRadian(angle);
		float distance = 350.0f;
		patternData.farLimmitLine[i].endPos = pos + Vec2<float>(cosf(dir), sinf(dir)) * distance;
	}

	if (bossPatternNow != BOSS_PATTERN_NORMALMOVE)
	{
		moveVel = { 10.0f,0.0f };
	}



	for (int i = 0; i < patternData.bulltData.size(); ++i)
	{
		if (patternData.bulltData[i].initFlag)
		{
			patternData.bulltData[i].Reset();
		}
	}

	// [�U��񂵒����U��񂳂ꒆ��������] �X�V�������s��Ȃ��B�@�@�Ղ̎����ł��B
	bool isSwingNow = this->GetNowSwing() || partner.lock()->GetNowSwing();

	// [�d����] [�X�^�����o��] �͓������Ȃ�
	if (0 < afterSwingDelay || StunEffect::Instance()->isActive) {
		// �������Ȃ��B
	}
	else if (isSwingNow) {

	}
	else if (GetCanMove()) {

		//�{�X��AI-----------------------
		++patternTimer;
		//���̏�ԑI��
		if (120 <= patternTimer)
		{
			Vec2<float> dir = GetPartnerPos() - pos;
			dir.Normalize();

			while (1)
			{
				int random = KuroFunc::GetRand(BOSS_PATTERN_MAX - 1);
				//�U��񂵂̏�����������Ă��Ȃ����͐U��񂵂������Ȃ�
				if (fabs(dir.y) <= 0.3f && random == BOSS_PATTERN_SWING)
				{
					continue;
				}
				bossPatternNow = static_cast<E_BossPattern>(random);
				break;
			}
			patternTimer = 0;
		}
		//�{�X��AI-----------------------
		bossPatternNow = BOSS_PATTERN_NORMALMOVE;

		/*if (DebugKeyManager::Instance()->DebugKeyTrigger(DIK_P, "Boss Swing", "DIK_P")) {

			bossPatternNow = static_cast<E_BossPattern>(BOSS_PATTERN_SWING);
			patternTimer = 0;
			bossPattern[bossPatternNow]->Init();

		}*/

		//�{�X�̋���
		if (bossPatternNow != oldBossPattern)
		{
			bossPattern[bossPatternNow]->Init();
		}
		bossPattern[bossPatternNow]->Update(&patternData);
		oldBossPattern = bossPatternNow;

		//�{�X�̒e
		for (int i = 0; i < patternData.bulltData.size(); ++i)
		{
			if (patternData.bulltData[i].initFlag)
			{
				Shot(pos, patternData.bulltData[i].dir, patternData.bulltData[i].speed);
			}
		}
	}
	DebugParameter::Instance()->bossDebugData.bossNowStatus = static_cast<E_BossPattern>(bossPatternNow);


	//�U��񂵖���
	if (CharacterAIOrder::Instance()->swingClockWiseFlag)
	{
		// �U��񂵂̃g���K�[����
		SwingPartner(-Vec2<float>(partner.lock()->pos - pos).GetNormal(), true);
		CharacterAIOrder::Instance()->swingClockWiseFlag = false;
	}else if (CharacterAIOrder::Instance()->swingCounterClockWiseFlag)
	{
		// �U��񂵂̃g���K�[����
		SwingPartner(-Vec2<float>(partner.lock()->pos - pos).GetNormal(), false);
		CharacterAIOrder::Instance()->swingCounterClockWiseFlag = false;
	}



	CWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData);
	//CWSwingSegmentMgr.Init();
	CCWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData);
	//CCWSwingSegmentMgr.Init();

	DebugParameter::Instance()->bossDebugData.moveVel = moveVel;


	// �ړ��ʂɊւ���ϐ��������őS��vel�ɑ������B
	vel = moveVel;

}

#include"DrawFunc_FillTex.h"
#include"D3D12App.h"
void Boss::OnDraw()
{
	/*===== �`�揈�� =====*/
	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	DIR dir = FRONT;
	if (vel.y < 0)dir = BACK;

	auto drawPos = pos + stagingDevice.GetShake();
	auto drawScale = size * stagingDevice.GetExtRate();
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));

	if (DebugParameter::Instance()->bossDebugData.drawBossFlag)
	{
		DrawFunc_FillTex::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(drawPos - drawScale), ScrollMgr::Instance()->Affect(drawPos + drawScale),
			TexHandleMgr::GetTexBuffer(graphHandle[dir]), CRASH_TEX, stagingDevice.GetFlashAlpha());
	}

	//���C�Ƃ̔���m�F
	if (DebugParameter::Instance()->bossDebugData.drawNearRayFlag)
	{
		for (int i = 0; i < patternData.nearLimmitLine.size(); ++i)
		{
			Vec2<float>drawStartPos = ScrollMgr::Instance()->Affect(patternData.nearLimmitLine[i].startPos);
			Vec2<float>drawEndPos = ScrollMgr::Instance()->Affect(patternData.nearLimmitLine[i].endPos);
			if (patternData.nearLimmitLine[i].hitFlag)
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(255, 0, 0, 255));
			}
			else
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(255, 255, 255, 255));
			}
		}
	}

	if (DebugParameter::Instance()->bossDebugData.drawFarRayFlag)
	{
		for (int i = 0; i < patternData.farLimmitLine.size(); ++i)
		{
			Vec2<float>drawStartPos = ScrollMgr::Instance()->Affect(patternData.farLimmitLine[i].startPos);
			Vec2<float>drawEndPos = ScrollMgr::Instance()->Affect(patternData.farLimmitLine[i].endPos);
			if (patternData.farLimmitLine[i].hitFlag)
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(0, 255, 0, 255));
			}
			else
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(0, 0, 255, 255));
			}
		}
	}


	CWSwingSegmentMgr.Draw(RIGHT_TEAM);
	CCWSwingSegmentMgr.Draw(RIGHT_TEAM);
}

void Boss::Shot(const Vec2<float> &generatePos, const float &forwardAngle, const float &speed)
{
	static const int BULLET_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/bullet_enemy.png");
	bulletMgr.Generate(BULLET_GRAPH, generatePos, forwardAngle, speed);
}

bool Boss::Appear()
{
	if (20 <= moveTimer)
	{
		return true;
	}

	if (SCALE.x < size.x && SCALE.y < size.y)
	{
		float time = 30.0f;
		size -= initScale / time;
	}
	else
	{
		if (!initPaticleFlag)
		{
			Vec2<float>radian(cosf(Angle::ConvertToRadian(0.0f)), sinf(Angle::ConvertToRadian(0.0f)));
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(90.0f)), sinf(Angle::ConvertToRadian(90.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(180.0f)), sinf(Angle::ConvertToRadian(180.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(270.0f)), sinf(Angle::ConvertToRadian(270.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);
			initPaticleFlag = true;
		}

		size = { 80.0f,80.0f };
		++moveTimer;
	}

	return false;
}