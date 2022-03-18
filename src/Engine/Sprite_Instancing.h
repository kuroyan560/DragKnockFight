#pragma once
#include<memory>
class GraphicsPipeline;
class StructuredBuffer;

#include"SpriteMesh.h"
#include"KuroMath.h"
#include"Color.h"
#include<vector>
#include"Transform2D.h"

//各インスタンスの情報
class InstanceData
{
	friend class Sprite_Instancing;
	Color color;
	bool colorDirty = false;
public:
	Transform2D transform;
	void SetColor(const Color& Color)
	{
		color = Color;
		colorDirty = true;
	}
};

//インスタンシング描画用スプライト
class Sprite_Instancing
{
private:
	//インスタンシング描画最大数
	static const int INTANCING_NUM_MAX = 300;
	//パイプライン
	static std::shared_ptr<GraphicsPipeline>PIPELINE[AlphaBlendModeNum];
	//白のベタ塗りテクスチャ
	static std::shared_ptr<TextureBuffer>DEFAULT_TEX;

private:
	//構造化バッファ送信用データ
	struct StructuredData
	{
		Matrix mat;
		Color color;
	};
	std::shared_ptr<StructuredBuffer>structuredBuff;

	//テクスチャバッファ
	std::shared_ptr<TextureBuffer>texBuff;

public:
	//メッシュ（頂点情報）　※全てのインスタンスに影響
	SpriteMesh mesh;

	//各インスタンスの情報
	std::vector<InstanceData>instanceDatas;

	//テクスチャ、スプライト名
	Sprite_Instancing(const std::shared_ptr<TextureBuffer>& Texture = nullptr, const char* Name = nullptr);

	//テクスチャセット
	void SetTexture(const std::shared_ptr<TextureBuffer>& Texture);

	//描画
	void Draw(const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);
};