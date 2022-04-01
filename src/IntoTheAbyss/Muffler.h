#pragma once
#include"Vec.h"
#include<memory>
#include<vector>
class VertexBuffer;
class IndexBuffer;
class TextureBuffer;
class ComputePipeline;
class ConstantBuffer;
class GraphicsPipeline;
class LightManager;

class Muffler
{
	struct Vertex
	{
		Vec2<float>pos;
		Vec2<float>uv;
		Vec2<float>vel;	//���x
		Vec2<float>force;	//��
	};
	struct MufflerInfo
	{
		Vec2<int>vertexNum;
		Vec2<float>mufflerSize;
	};
	struct ZoomAndScroll
	{
		float zoom = 0.0f;
		Vec2<float>scroll = { 0.0f,0.0f };
	};

	static const int VERTEX_NUM_X = 8;
	static const int VERTEX_NUM_Y = 8;

	Vertex vertices[VERTEX_NUM_Y][VERTEX_NUM_X];
	std::vector<unsigned int>indices;
	std::shared_ptr<VertexBuffer>vertBuff;
	std::shared_ptr<IndexBuffer>idxBuff;
	std::shared_ptr<TextureBuffer>tex;

	//�R���s���[�g�p�C�v���C��
	std::shared_ptr<ComputePipeline>cPipeline;
	std::shared_ptr<ConstantBuffer>playerPosBuff;
	std::shared_ptr<ConstantBuffer>mufflerInfo;

	//�O���t�B�b�N�X�p�C�v���C��
	std::shared_ptr<GraphicsPipeline>gPipeline;
	std::shared_ptr<ConstantBuffer>zoomAndScroll;
public:
	Muffler();
	void Init(const Vec2<float>& InitPos);
	void Update(const Vec2<float>& PlayerPos);
	void Draw(LightManager& LigManager);
};

