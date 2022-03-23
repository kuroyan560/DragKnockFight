#pragma once
#include"D3D12Data.h"
#include<memory>
#include<vector>
#include"Vec.h"
#include"Color.h"

//画像にクリッピングベタ塗り
class DrawFunc_Color
{
	//DrawExtendGraph
	static std::shared_ptr<GraphicsPipeline>EXTEND_GRAPH_PIPELINE;
	static int DRAW_EXTEND_GRAPH_COUNT;
	static std::vector<std::shared_ptr<VertexBuffer>>EXTEND_GRAPH_VERTEX_BUFF;

	//DrawRotaGraph
	static std::shared_ptr<GraphicsPipeline>ROTA_GRAPH_PIPELINE;
	static int DRAW_ROTA_GRAPH_COUNT;
	static std::vector<std::shared_ptr<VertexBuffer>>ROTA_GRAPH_VERTEX_BUFF;

public:
	//呼び出しカウントリセット
	static void CountReset()
	{
		DRAW_EXTEND_GRAPH_COUNT = 0;
		DRAW_ROTA_GRAPH_COUNT = 0;
	}

	/// <summary>
	/// 拡大縮小描画
	/// </summary>
	/// <param name="LeftUpPos">矩形の左上座標</param>
	/// <param name="RightBottomPos">矩形の右上座標</param>
	/// <param name="Tex">テクスチャ</param>
	/// <param name="Paint">塗りつぶす色</param>
	static void DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos,
		const std::shared_ptr<TextureBuffer>& Tex, const Color& Paint = Color());

	/// <summary>
	/// ２D画像回転描画
	/// </summary>
	/// <param name="Center">中心座標</param>
	/// <param name="ExtRate">拡大率</param>
	/// <param name="Radian">回転角度</param>
	/// <param name="Tex">テクスチャ</param>
	/// <param name="Paint">塗りつぶす色</param>
	static void DrawRotaGraph2D(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
		const std::shared_ptr<TextureBuffer>& Tex, const Color& Paint = Color(), const Vec2<float>& RotaCenterUV = { 0.5f,0.5f });
};

