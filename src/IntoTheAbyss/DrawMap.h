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
	float expEaseRate = 1.0f;
	float alpha = 1.0f;
};

class DrawMap
{
	static const int MAX_CHIP_NUM = 2000;
	static std::shared_ptr<GraphicsPipeline>PIPELINE_BASE;
	static std::shared_ptr<GraphicsPipeline>PIPELINE_SHOCKED;
	static std::shared_ptr<ConstantBuffer>EXT_RATE_BUFF;

	std::shared_ptr<VertexBuffer>vertexBuff;

	ChipData chipDatas[MAX_CHIP_NUM];
	unsigned int chipNum = 0;
public:
	DrawMap();
	void AddChip(const ChipData& Data);
	void Draw(const std::shared_ptr<TextureBuffer>& Tex);
};