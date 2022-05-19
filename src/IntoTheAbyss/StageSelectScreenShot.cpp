#include "StageSelectScreenShot.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"IntoTheAbyss/StageSelectOffsetPosDebug.h"

StageSelectScreenShot::StageSelectScreenShot()
{
	screenShotHandle[0] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/0.png");
	screenShotHandle[1] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/1.png");
	screenShotHandle[2] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/2.png");
	stageNumberHandle[0] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_tag/0.png");
	stageNumberHandle[1] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_tag/1.png");
	stageNumberHandle[2] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_tag/2.png");
	selectNum = 0;
}

void StageSelectScreenShot::Init()
{
	screenShotLerpData.Init(Vec2<float>(640.0f, 360.0f), Vec2<float>(1.12f, 1.0f));
	stageNumberData.Init(Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), 610.0f), Vec2<float>(1.0f, 1.0f));
	expData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	stageNumberExpData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	zoomOutFlag = false;
}

void StageSelectScreenShot::Update()
{
	if (zoomOutFlag)
	{
		screenShotLerpData.lerpPos = { 640.0f,144.0f };
		screenShotLerpData.lerpSize = { 0.4f,0.4f };

		// �Y�[���A�E�g���Ă�����X�e�[�W���̏ꏊ�����̕��ɐ�����΂��B
		stageNumberData.lerpPos = Vec2<float>(stageNumberData.lerpPos.x, 2000);

	}
	else
	{
		screenShotLerpData.lerpPos = { 640.0f,360.0f };
		screenShotLerpData.lerpSize = { 1.12f,1.0f };

		// �Y�[���C�����Ă�����X�e�[�W���̏ꏊ�����̈ʒu�ɖ߂��B
		stageNumberData.lerpPos = Vec2<float>(stageNumberData.lerpPos.x, 610.0f);

	}
	screenShotLerpData.Lerp();

	// �g�k�̂��߂̑傫����0�ɖ߂��B
	expData.lerpPos = Vec2<float>(0, 0);
	expData.lerpSize = Vec2<float>(0, 0);
	expData.Lerp();
	stageNumberExpData.lerpPos = Vec2<float>(0, 0);
	stageNumberExpData.lerpSize = Vec2<float>(0, 0);
	stageNumberExpData.Lerp();
	stageNumberData.Lerp();
}

void StageSelectScreenShot::Draw()
{
	Vec2<float> debugPos = StageSelectOffsetPosDebug::Instance()->pos;
	DrawFunc::DrawRotaGraph2D(screenShotLerpData.pos + expData.pos + debugPos, screenShotLerpData.size + expData.size, 0.0f, TexHandleMgr::GetTexBuffer(screenShotHandle[selectNum]));
	DrawFunc::DrawRotaGraph2D(stageNumberData.pos + stageNumberExpData.pos + debugPos, stageNumberData.size + stageNumberExpData.size, 0.0f, TexHandleMgr::GetTexBuffer(stageNumberHandle[selectNum]));
}

void StageSelectScreenShot::Next()
{
	++selectNum;

	// �X�e�[�W�̔ԍ���UI���k��������B
	stageNumberExpData.size = Vec2<float>(-1.0f, -1.0f);

	// �ő�l�ɒB������ŏ��̃n���h���ɖ߂��B
	if (screenShotHandle.size() - 1 < selectNum) selectNum = 0;

}

void StageSelectScreenShot::Prev()
{
	--selectNum;

	// �X�e�[�W�̔ԍ���UI���k��������B
	stageNumberExpData.size = Vec2<float>(-1.0f, -1.0f);

	// �ŏ��l�ɒB������ŏ��̃n���h���ɖ߂��B
	if (selectNum < 0) selectNum = screenShotHandle.size() - 1;
}

void StageSelectScreenShot::ImGuiDraw()
{
	/*ImGui::Begin("StageSelectImage");
	ImGui::InputFloat("PosX", &screenShotLerpData.pos.x);
	ImGui::InputFloat("PosY", &screenShotLerpData.pos.y);
	ImGui::InputFloat("SizeX", &screenShotLerpData.size.x);
	ImGui::InputFloat("SizeY", &screenShotLerpData.size.y);
	ImGui::Checkbox("ZoomOut", &zoomOutFlag);
	ImGui::End();*/
}
