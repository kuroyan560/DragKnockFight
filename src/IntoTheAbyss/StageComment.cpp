#include "StageComment.h"
#include "TexHandleMgr.h"
#include "../Engine/DrawFunc.h"

StageComment::StageComment()
{
	nonCommentHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_tag/non.png");

	int stageNum = 0;
	while (KuroFunc::ExistFile("resource/ChainCombat/select_scene/stage_tag/" + std::to_string(stageNum) + ".png"))
	{
		stageNum++;
	}
	stageComment.resize(stageNum);
	for (int i = 0; i < stageNum; ++i)
	{
		stageComment[i] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_tag/" + std::to_string(i) + ".png");
	}

	commentSprite = std::make_shared<Sprite>(nullptr, "StageCommentSprite");
	changeRate = 1.0f;
}

void StageComment::Init(int STAGE_NUM)
{
	stageNum = STAGE_NUM;
	prevStageNum = -1;
	appearRate = 0.0f;
	easeVel = 50.0f;
}

void StageComment::Update()
{
	//ステージコメント
	static int COMMENT_MOVE_WAIT_TIMER = 0;
	static const int COMMENT_MOVE_WAIT_TOTAL_TIME = 45;
	if (prevStageNum != stageNum)
	{
		int commentGraph = nonCommentHandle;
		if (stageNum < stageComment.size())
		{
			commentGraph = stageComment[stageNum];
		}
		commentSprite->SetTexture(TexHandleMgr::GetTexBuffer(commentGraph));
		commentSprite->mesh.SetUv(0.0f, 1.0f, 0.0f, 1.0f);

		Vec2<float>data(TexHandleMgr::GetTexBuffer(commentGraph)->GetGraphSize().Float().x / 2.0f + 200.0f, TexHandleMgr::GetTexBuffer(commentGraph)->GetGraphSize().Float().y);
		commentSprite->mesh.SetSize(data);
		commentSprite->mesh.SetAnchorPoint({ 1.0f,0.0f });
		COMMENT_MOVE_WAIT_TIMER = 0;
		prevStageNum = stageNum;
	}
	COMMENT_MOVE_WAIT_TIMER++;
	if (COMMENT_MOVE_WAIT_TOTAL_TIME <= COMMENT_MOVE_WAIT_TIMER)
	{
		const float SCROLL = 0.001f;
		commentSprite->mesh.AddUv({ SCROLL,0.0f }, { SCROLL,0.0f }, { SCROLL,0.0f }, { SCROLL,0.0f });
	}

	//KuroMath::Lerp(0.0f, -128.0f, changeRate);


	if (appearRate < 1.0f)
	{
		appearRate += 1.0f / 60.0f;
	}
	if (1.0f <= appearRate)
	{
		appearRate = 1.0f;
	}

	easeY = KuroMath::Ease(Out, Cubic, appearRate, 0.0f, 1.0f) * -easeVel;

	commentSprite->transform.SetPos({ WinApp::Instance()->GetExpandWinSize().x,680.0f + easeVel + easeY });
}

void StageComment::Draw()
{
	//DrawFunc::DrawBox2D({ 0,0 }, commentSprite->mesh.GetSize() * Vec2<float>(1.0f, 1.3f), Color(0.0f, 0.0f, 0.0f, 0.7f * KuroMath::Lerp(1.0f, 0.0f, changeRate)), true, AlphaBlendMode_Trans);
	Vec2<float>size(commentSprite->mesh.GetSize() * Vec2<float>(1.0f, 1.3f));
	Vec2<float>leftUpPos(0.0f, commentSprite->transform.GetPos().y);
	Vec2<float>rightDownPos(WinApp::Instance()->GetExpandWinSize());
	Vec2<float>easePos(0.0f, easeVel + easeY);

	DrawFunc::DrawBox2D(leftUpPos + easePos, rightDownPos + easePos, Color(0.0f, 0.0f, 0.0f, 0.5f), true, AlphaBlendMode_Trans);
	commentSprite->Draw();
}
