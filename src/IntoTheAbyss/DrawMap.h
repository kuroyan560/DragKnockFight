#pragma once
#include"Vec.h"

#include<memory>
class GraphicsPipeline;
class VertexBuffer;
class ConstantBuffer;
class TextureBuffer;
class StructuredBuffer;
#include<vector>

struct ChipData
{
	Vec2<float>pos;
	float radian = 0.0f;
	float shocked = 0.0f;
};

class DrawMap
{
	static const int MAX_CHIP_NUM = 300;
	static std::shared_ptr<GraphicsPipeline>PIPELINE;
	static std::shared_ptr<ConstantBuffer>EXT_RATE_BUFF;

	// (0,0,-1) のベタ塗りノーマルマップ
	static std::shared_ptr<TextureBuffer>DEFAULT_NORMAL_MAP;
	//黒のベタ塗りテクスチャ
	static std::shared_ptr<TextureBuffer>DEFAULT_EMISSIVE_MAP;

	std::shared_ptr<VertexBuffer>vertexBuff;

	ChipData chipDatas[MAX_CHIP_NUM];
	unsigned int chipNum = 0;
public:
	DrawMap();
	void AddChip(const ChipData& Data);
	void Draw(const std::shared_ptr<TextureBuffer>& Tex,const std::shared_ptr<TextureBuffer>& NormalMap = nullptr, const std::shared_ptr<TextureBuffer>& EmissiveMap = nullptr);
};