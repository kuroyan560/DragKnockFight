#include "BackGround.h"
#include"KuroFunc.h"
#include"TexHandleMgr.h"
#include"Color.h"

BackGround::BackGround()
{
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/background_star.png", 4, { 4,1 }, starGraph);
}

void BackGround::Init(const Vec2<float>& StageSize)
{
	stars.clear();
	const Vec2<float>stageCenter = StageSize / 2.0f;

	static const Vec2<float> STAR_SPAN = { 300,300 };
	static const float RAND_OFFSET_MAX = 220.0f;

	static const float OFFSET_X = STAR_SPAN.x / 2.0f;

	const Vec2<float>arrayNum = StageSize / STAR_SPAN;

	const float startPosY = stageCenter.y - (arrayNum / 2.0f).Int().y * STAR_SPAN.y;
	for (int y = 0; y < arrayNum.Int().y; ++y)
	{
		const float startPosX = stageCenter.x - (arrayNum / 2.0f).Int().x * STAR_SPAN.x + (y % 2 == 0 ? 0 : OFFSET_X);
		
		for (int x = 0; x < arrayNum.Int().x; ++x)
		{
			Star star;
			star.pos.x = startPosX + x * STAR_SPAN.x + KuroFunc::GetRand(RAND_OFFSET_MAX) * KuroFunc::GetRandPlusMinus();
			star.pos.y = startPosY + y * STAR_SPAN.y + KuroFunc::GetRand(RAND_OFFSET_MAX) * KuroFunc::GetRandPlusMinus();
			star.flashRad = Angle(KuroFunc::GetRand(0, 360));
			star.type = (STAR_COLOR)KuroFunc::GetRand(STAR_NUM - 1);
			star.scaleOffset = KuroFunc::GetRand(0.05f) * KuroFunc::GetRandPlusMinus();
			stars.emplace_back(star);
		}
	}
}

void BackGround::Update()
{
	static const float ADD_ANGLE = 1.0f;

	for (auto& s : stars)
	{
		s.flashRad += Angle::ConvertToRadian(ADD_ANGLE);
		s.flashRad.Normalize();
	}
}

#include"DrawFunc.h"
#include"DrawFunc_Color.h"
#include"ScrollMgr.h"
#include"ShakeMgr.h"
#include"WinApp.h"
void BackGround::Draw()
{
	static const int GRAPH_RADIUS = 11;
	static const Color FLASH_COLOR[STAR_NUM] =
	{
		Color(162,27,108,255),
		Color(41,201,180,255),
		Color(186,196,60,255),
		Color(42,144,215,255)
	};

	const auto winSize = WinApp::Instance()->GetExpandWinSize();

	for (auto& s : stars)
	{
		const auto drawPos = ScrollMgr::Instance()->Affect(s.pos);

		//‰æ–ÊŠO‚È‚ç•`‰æ‚µ‚È‚¢
		if (drawPos.x + GRAPH_RADIUS < 0)continue;
		if (winSize.x < drawPos.x - GRAPH_RADIUS)continue;
		if (drawPos.y + GRAPH_RADIUS < 0)continue;
		if (winSize.y < drawPos.y - GRAPH_RADIUS)continue;

		auto flashColor = FLASH_COLOR[s.type];
		flashColor.Alpha() = sin(s.flashRad);
		static const float SCALE = 0.25f;
		static const float SHADOW_SCALE_OFFSET = 0.2f;
		Vec2<float>scale = { SCALE + s.scaleOffset,SCALE + s.scaleOffset };
		scale *= ScrollMgr::Instance()->zoom;
		DrawFunc_Color::DrawRotaGraph2D(drawPos, { scale.x + SHADOW_SCALE_OFFSET,scale.y + SHADOW_SCALE_OFFSET }, 0.0f, TexHandleMgr::GetTexBuffer(starGraph[s.type]), flashColor);
		DrawFunc::DrawRotaGraph2D(drawPos, scale, 0.0f, TexHandleMgr::GetTexBuffer(starGraph[s.type]));
	}
}
