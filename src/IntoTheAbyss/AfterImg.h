#pragma once
#include<forward_list>
#include"D3D12Data.h"
#include"Vec.h"

//Žc‘œ
class AfterImg
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE;
	static const int MAX_NUM = 1000;
	static const float MAX_ALPHA;
	class Vertex
	{
	public:
		Vec2<float>leftUpPos;
		Vec2<float>rightBottomPos;
		Vec2<int> miror;
		int srvIdx;
		float alpha;
		float scale;
		Vertex(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const Vec2<bool>& Miror, const int& SRVIdx, const float& Alpha, const float& Scale)
			:leftUpPos(LeftUpPos), rightBottomPos(RightBottomPos), miror({ Miror.x ? 1 : 0 ,Miror.y ? 1 : 0 }), srvIdx(SRVIdx), alpha(Alpha), scale(Scale) {}
	};

	class Img
	{
	public:
		Vec2<float>pos;
		const int lifeSpan;
		int life = 0;
		float alpha = MAX_ALPHA;
		float scale = 1.0f;
		int handle;
		Vec2<float>graphSize;
		Vec2<bool>miror;

		Img(const Vec2<float>& Pos, const float& LifeSpan, const int& Handle, const Vec2<float>& GraphSize, const Vec2<bool>& Miror)
			:pos(Pos), lifeSpan(LifeSpan), handle(Handle), graphSize(GraphSize), miror(Miror) {}
	};

	std::forward_list<Img>imgs;
	std::shared_ptr<VertexBuffer>vertexBuff;

	static const int SRV_MIN_IDX = 1;
	static const int SRV_MAX_IDX = 6;
	int srvIdx = SRV_MIN_IDX;
	std::vector<std::shared_ptr<DescriptorData>>descDatas;
	std::vector<DESC_HANDLE_TYPE>descTypes;

public:
	AfterImg();
	void EmitArray(const Vec2<float>& From, const Vec2<float>& To, const int& GraphHandle, const Vec2<float>& GraphSize, const Vec2<bool>& Miror);
	void Draw(const float& ExtRate, const Vec2<float>& Scroll);
};