#pragma once
#include"D3D12Data.h"
#include<memory>
#include<vector>
#include"Vec.h"
#include"Color.h"

//�摜�ɃN���b�s���O�A���̉摜�œh��
class DrawFunc_FillTex
{
	//DrawExtendGraph
	class ExtendGraphVertex
	{
	public:
		Vec2<float>leftUpPos;
		Vec2<float>rightBottomPos;
		float alpha;
		Vec2<int> miror;
		Vec2<float>leftUpPaintUV;
		Vec2<float>rightBottomPaintUV;
		ExtendGraphVertex(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const float& Alpha, const Vec2<bool>& Miror, const Vec2<float>& LeftUpPaintUV, const Vec2<float>& RightBottomPaintUV)
			:leftUpPos(LeftUpPos), rightBottomPos(RightBottomPos), alpha(Alpha), miror({ Miror.x ? 1 : 0 ,Miror.y ? 1 : 0 }), leftUpPaintUV(LeftUpPaintUV), rightBottomPaintUV(RightBottomPaintUV) {}
	};
	static std::shared_ptr<GraphicsPipeline>EXTEND_GRAPH_PIPELINE;
	static int DRAW_EXTEND_GRAPH_COUNT;
	static std::vector<std::shared_ptr<VertexBuffer>>EXTEND_GRAPH_VERTEX_BUFF;
	static void CreateExtendGraphFunc();

	//DrawRotaGraph
	static std::shared_ptr<GraphicsPipeline>ROTA_GRAPH_PIPELINE;
	static int DRAW_ROTA_GRAPH_COUNT;
	static std::vector<std::shared_ptr<VertexBuffer>>ROTA_GRAPH_VERTEX_BUFF;

public:
	//�Ăяo���J�E���g���Z�b�g
	static void CountReset()
	{
		DRAW_EXTEND_GRAPH_COUNT = 0;
		DRAW_ROTA_GRAPH_COUNT = 0;
	}

	/// <summary>
	/// �摜�`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="DestTex">�e�N�X�`��</param>
	/// <param name="SrcTex">�h��Ԃ��e�N�X�`��</param>
	/// <param name="Miror">��`�̉E����W</param>
	/// <param name="LeftUpPaintUV">�h��Ԃ��͈͂̍���UV</param>
	/// <param name="RightBottomPaintUV">�h��Ԃ��͈͂̉E��UV</param>
	static void DrawGraph(const Vec2<float>& LeftUpPos, const std::shared_ptr<TextureBuffer>& DestTex, const std::shared_ptr<TextureBuffer>& SrcTex,
		const float& SrcAlpha, const Vec2<bool>& Miror = { false,false }, const Vec2<float>& LeftUpPaintUV = { 0.0f,0.0f }, const Vec2<float>& RightBottomPaintUV = { 1.0f,1.0f });

	/// <summary>
	/// �g��k���`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="RightBottomPos">��`�̉E����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Paint">�h��Ԃ��F</param>
	/// <param name="Miror">���]�t���O</param>
	/// <param name="LeftUpPaintUV">�h��Ԃ��͈͂̍���UV</param>
	/// <param name="RightBottomPaintUV">�h��Ԃ��͈͂̉E��UV</param>
	static void DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos,
		const std::shared_ptr<TextureBuffer>& DestTex, const std::shared_ptr<TextureBuffer>& SrcTex, const float& SrcAlpha,
		const Vec2<bool>& Miror = { false,false },	const Vec2<float>& LeftUpPaintUV = { 0.0f,0.0f }, const Vec2<float>& RightBottomPaintUV = { 1.0f,1.0f });

	/// <summary>
	/// �QD�摜��]�`��
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="ExtRate">�g�嗦</param>
	/// <param name="Radian">��]�p�x</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Paint">�h��Ԃ��F</param>
	static void DrawRotaGraph2D(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
		const std::shared_ptr<TextureBuffer>& DestTex, const std::shared_ptr<TextureBuffer>& SrcTex, const float& SrcAlpha, const Vec2<float>& RotaCenterUV = { 0.5f,0.5f },
		const Vec2<bool>& Miror = { false,false },const Vec2<float>& LeftUpPaintUV = { 0.0f,0.0f }, const Vec2<float>& RightBottomPaintUV = { 1.0f,1.0f });
};

