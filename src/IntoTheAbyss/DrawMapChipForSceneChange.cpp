#include "DrawMapChipForSceneChange.h"
#include "TexHandleMgr.h"
#include "StageMgr.h"
#include "CharacterManager.h"

#include"Camera.h"
#include"ScrollMgr.h"


DrawMapChipForSceneChange::DrawMapChipForSceneChange()
{
	auto backBuff = D3D12App::Instance()->GetBackBuffRenderTarget();
	Vec2<int>s(WinApp::Instance()->GetExpandWinSize().x, WinApp::Instance()->GetExpandWinSize().y);
	mapBuffer = D3D12App::Instance()->GenerateRenderTarget(backBuff->GetDesc().Format, Color(56, 22, 74, 255), s, L"SceneChangeMapSS");

	//scroll.camera = camera;
	//camera->scroll = scroll;

	//scroll = std::make_shared<LocalScrollMgr>();

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


	sceneChageFlag = false;
}

void DrawMapChipForSceneChange::Init(int STAGE_NUM, bool SCENE_CHANGE_FLAG)
{
	StageMgr::Instance()->SetLocalMapChipData(STAGE_NUM, 0);

	mapChip = *StageMgr::Instance()->GetLocalMap();

	stageNum = STAGE_NUM;
	isSS = true;


	playerPos = StageMgr::Instance()->GetPlayerResponePos(STAGE_NUM, 0);
	bossPos = StageMgr::Instance()->GetBossResponePos(STAGE_NUM, 0);

	MapChipArray tmp = *StageMgr::Instance()->GetLocalMap();
	Vec2<float>distance = (bossPos - playerPos) / 2.0f;
	centralPos = playerPos + distance;

	Vec2<float>mapSize(tmp[0].size() * MAP_CHIP_SIZE, tmp.size() * MAP_CHIP_SIZE);
	Vec2<float>adj = { 0.0f,-40.0f };


	sceneChageFlag = SCENE_CHANGE_FLAG;
	if (!sceneChageFlag)
	{
		centralPos = mapSize / 2.0f;
		playerPos.x = 0.0f;
		playerPos.y = centralPos.y;
		bossPos.x = tmp[0].size() * MAP_CHIP_SIZE;
		bossPos.y = centralPos.y;
	}

	ScrollMgr::Instance()->Init(centralPos, mapSize, adj);
	Camera::Instance()->Init();

	Camera::Instance()->Zoom(playerPos, bossPos);
	//ScrollMgr::Instance()->zoom = ZOOM;
	Camera::Instance()->zoom = ScrollMgr::Instance()->zoom;

}

void DrawMapChipForSceneChange::Finalize()
{
	isSS = false;
}

void DrawMapChipForSceneChange::Update()
{
	Camera::Instance()->Update();
	if (sceneChageFlag)
	{
		Camera::Instance()->Zoom(playerPos, bossPos);
		ScrollMgr::Instance()->zoom = Camera::Instance()->zoom;
	}
	ScrollMgr::Instance()->Update(centralPos, true);
}

#include"ClearInfoContainer.h"
#include"DrawFunc.h"
void DrawMapChipForSceneChange::Draw(const float& ChangeRate)
{
	static auto BLACK = D3D12App::Instance()->GenerateTextureBuffer(Color(0.0f, 0.0f, 0.0f, 0.15f));
	static const int NON_COMMENT = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_tag/non.png");

	//if (isSS)
	//{
		KuroEngine::Instance().Graphics().SetRenderTargets({ mapBuffer });
		KuroEngine::Instance().Graphics().ClearRenderTarget({ mapBuffer });
		DrawMapChip(mapChip, stageNum, 0);
		DrawFunc::DrawExtendGraph2D({ 0,0 }, mapBuffer->GetGraphSize().Float(), BLACK, Color(1.0f, 1.0f, 1.0f, (1.0f - ChangeRate)));

		//クリア情報
		auto& clearInfoContainer = ClearInfoContainerMgr::Instance()->GetContainer(stageNum);
		if (clearInfoContainer.clear)
		{
			DrawFunc::DrawGraph({ 0,0 }, clearInfoContainer.clearInfoRenderTarget, Color(1.0f, 1.0f, 1.0f, (1.0f - ChangeRate)));
		}

		//ステージコメント
		static int OLD_STAGE_NUM = -1;
		static int COMMENT_MOVE_WAIT_TIMER = 0;
		static const int COMMENT_MOVE_WAIT_TOTAL_TIME = 45;
		if (OLD_STAGE_NUM != stageNum)
		{
			int commentGraph = NON_COMMENT;
			if (stageNum < stageComment.size())
			{
				commentGraph = stageComment[stageNum];
			}
			commentSprite->SetTexture(TexHandleMgr::GetTexBuffer(commentGraph));
			commentSprite->mesh.SetUv(0.0f, 1.0f, 0.0f, 1.0f);
			commentSprite->mesh.SetSize(TexHandleMgr::GetTexBuffer(commentGraph)->GetGraphSize().Float());
			commentSprite->mesh.SetAnchorPoint({ 1.0f,0.0f });
			COMMENT_MOVE_WAIT_TIMER = 0;
			OLD_STAGE_NUM = stageNum;
		}
		COMMENT_MOVE_WAIT_TIMER++;
		if (COMMENT_MOVE_WAIT_TOTAL_TIME <= COMMENT_MOVE_WAIT_TIMER)
		{
			const float SCROLL = 0.001f;
			commentSprite->mesh.AddUv({ SCROLL,0.0f }, { SCROLL,0.0f }, { SCROLL,0.0f }, { SCROLL,0.0f });
		}
		commentSprite->transform.SetPos({ WinApp::Instance()->GetExpandWinSize().x,KuroMath::Lerp(0.0f,-128.0f,ChangeRate) });
		DrawFunc::DrawBox2D({ 0,0 }, commentSprite->mesh.GetSize() * Vec2<float>(1.0f, 1.3f), Color(0.0f, 0.0f, 0.0f, 0.7f * KuroMath::Lerp(1.0f, 0.0f, ChangeRate)), true, AlphaBlendMode_Trans);
		commentSprite->Draw();

		crt.Excute(D3D12App::Instance()->GetCmdList(), mapBuffer);
		KuroEngine::Instance().Graphics().SetRenderTargets({ mapBuffer });
		crt.DrawResult(AlphaBlendMode_None);
		KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	//}
}

void DrawMapChipForSceneChange::DrawMapChip(const MapChipArray &mapChipData, const int &stageNum, const int &roomNum)
{
	std::map<int, std::vector<ChipData>>datas;

	// 描画するチップのサイズ
	const float DRAW_MAP_CHIP_SIZE = MAP_CHIP_SIZE * ScrollMgr::Instance()->zoom;
	SizeData wallChipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);


	// マップチップの縦の要素数を取得。
	const int HEIGHT = mapChipData.size();
	for (int height = 0; height < HEIGHT; ++height) {

		// マップチップの横の要素数を取得。
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			if (mapChip[height][width].drawData.shocked)mapChip[height][width].drawData.shocked -= 0.02f;
			if (mapChip[height][width].drawData.expEaseRate < 1.0f)mapChip[height][width].drawData.expEaseRate += 0.005f;

			// ブロック以外だったら処理を飛ばす。
			bool blockFlag = (mapChipData[height][width].chipType >= wallChipMemorySize.min && mapChipData[height][width].chipType <= wallChipMemorySize.max);
			if (blockFlag)
			{
				// スクロール量から描画する位置を求める。
				const Vec2<float> drawPos = ScrollMgr::Instance()->Affect({ width * MAP_CHIP_SIZE,height * MAP_CHIP_SIZE });

				// 画面外だったら描画しない。
				if (drawPos.x < -DRAW_MAP_CHIP_SIZE || drawPos.x > WinApp::Instance()->GetWinSize().x + DRAW_MAP_CHIP_SIZE) continue;
				if (drawPos.y < -DRAW_MAP_CHIP_SIZE || drawPos.y > WinApp::Instance()->GetWinSize().y + DRAW_MAP_CHIP_SIZE) continue;


				vector<std::shared_ptr<MapChipAnimationData>>tmpAnimation = StageMgr::Instance()->animationData;
				int handle = -1;
				if (height < 0 || mapChip.size() <= height) continue;
				if (width < 0 || mapChip[height].size() <= width) continue;
				//アニメーションフラグが有効ならアニメーション用の情報を行う
				if (mapChip[height][width].drawData.animationFlag)
				{
					int arrayHandle = mapChip[height][width].drawData.handle;
					++mapChip[height][width].drawData.interval;
					//アニメーションの間隔
					if (mapChip[height][width].drawData.interval % tmpAnimation[arrayHandle]->maxInterval == 0)
					{
						++mapChip[height][width].drawData.animationNum;
						mapChip[height][width].drawData.interval = 0;
					}
					//アニメーション画像の総数に達したら最初に戻る
					if (tmpAnimation[arrayHandle]->handle.size() <= mapChip[height][width].drawData.animationNum)
					{
						mapChip[height][width].drawData.animationNum = 0;
					}
					//分割したアニメーションの画像から渡す
					handle = tmpAnimation[arrayHandle]->handle[mapChip[height][width].drawData.animationNum];
				}
				else
				{
					handle = mapChip[height][width].drawData.handle;
				}

				//mapChipDrawData[height][width].shocked = KuroMath::Lerp(mapChipDrawData[height][width].shocked, 0.0f, 0.8f);

				Vec2<float> pos = drawPos;
				pos += mapChip[height][width].drawData.offset;
				if (0 <= handle)
				{
					ChipData chipData;
					chipData.pos = pos;
					chipData.radian = mapChip[height][width].drawData.radian;
					chipData.shocked = mapChip[height][width].drawData.shocked;
					chipData.expEaseRate = mapChip[height][width].drawData.expEaseRate;
					datas[handle].emplace_back(chipData);
					//DrawFunc::DrawRotaGraph2D({ pos.x, pos.y }, 1.6f * ScrollMgr::Instance()->zoom, mapChipDrawData[height][width].radian, TexHandleMgr::GetTexBuffer(handle));
				}
			}
		}
	}

	while (drawMap.size() < datas.size())
	{
		drawMap.emplace_back();
	}

	int i = 0;
	for (auto itr = datas.begin(); itr != datas.end(); ++itr)
	{
		for (int chipIdx = 0; chipIdx < itr->second.size(); ++chipIdx)
		{
			drawMap[i].AddChip(itr->second[chipIdx]);
		}
		drawMap[i].Draw(TexHandleMgr::GetTexBuffer(itr->first));
		i++;
	}

}