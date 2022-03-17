#pragma once
#include"D3D12Data.h"
#include<memory>
#include<vector>
#include"Vec.h"
#include"Color.h"

class GraphicsPipeline;
class VertexBuffer;

static class DrawFunc
{
	//DrawLine
	static std::shared_ptr<GraphicsPipeline>LINE_PIPELINE[AlphaBlendModeNum];
	static int DRAW_LINE_COUNT;	//�Ă΂ꂽ��
	static std::vector<std::shared_ptr<VertexBuffer>>LINE_VERTEX_BUFF;	//DrawLine�p

	//DrawBox
	static std::shared_ptr<GraphicsPipeline>BOX_PIPELINE[AlphaBlendModeNum];
	static int DRAW_BOX_COUNT;
	static std::vector<std::shared_ptr<VertexBuffer>>BOX_VERTEX_BUFF;

	//DrawCircle
	static std::shared_ptr<GraphicsPipeline>CIRCLE_PIPELINE[AlphaBlendModeNum];
	static int DRAW_CIRCLE_COUNT;
	static std::vector<std::shared_ptr<VertexBuffer>>CIRCLE_VERTEX_BUFF;

	//DrawExtendGraph
	static std::shared_ptr<GraphicsPipeline>EXTEND_GRAPH_PIPELINE[AlphaBlendModeNum];
	static int DRAW_EXTEND_GRAPH_COUNT;
	static std::vector<std::shared_ptr<VertexBuffer>>EXTEND_GRAPH_VERTEX_BUFF;

	//DrawRotaGraph
	static std::shared_ptr<GraphicsPipeline>ROTA_GRAPH_PIPELINE[AlphaBlendModeNum];
	static int DRAW_ROTA_GRAPH_COUNT;
	static std::vector<std::shared_ptr<VertexBuffer>>ROTA_GRAPH_VERTEX_BUFF;

public:
	//�Ăяo���J�E���g���Z�b�g
	static void CountReset()
	{
		DRAW_LINE_COUNT = 0;
		DRAW_BOX_COUNT = 0;
		DRAW_CIRCLE_COUNT = 0;
		DRAW_EXTEND_GRAPH_COUNT = 0;
		DRAW_ROTA_GRAPH_COUNT = 0;
	}

	/// <summary>
	/// �����̕`��
	/// </summary>
	/// <param name="FromPos">�N�_���W</param>
	/// <param name="ToPos">�I�_���W</param>
	/// <param name="Color">�F</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawLine2D(const Vec2<float>& FromPos, const Vec2<float>& ToPos, const Color& LineColor, const AlphaBlendMode& BlendMode = AlphaBlendMode_None);

	/// <summary>
	/// 2D�l�p�`�̕`��
	/// </summary>
	/// <param name="LeftUpPos">������W</param>
	/// <param name="RightBottomPos">�E�����W</param>
	/// <param name="Color">�F</param>
	/// <param name="FillFlg">����h��Ԃ���</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawBox2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const Color& BoxColor, const bool& FillFlg = false, const AlphaBlendMode& BlendMode = AlphaBlendMode_None);

	/// <summary>
	/// �QD�~�̕`��
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="Radius">���a</param>
	/// <param name="Color">�F</param>
	/// <param name="FillFlg">����h��Ԃ���</param>
	/// <param name="LineThickness">����h��Ԃ��Ȃ��ꍇ�̐��̑���</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawCircle2D(const Vec2<float>& Center, const float& Radius, const Color& CircleColor, const bool& FillFlg = false, const int& LineThickness = 1, const AlphaBlendMode& BlendMode = AlphaBlendMode_None);

	/// <summary>
	/// �g��k���`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="RightBottomPos">��`�̉E����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos,
		const std::shared_ptr<TextureBuffer>& Tex, const AlphaBlendMode& BlendMode = AlphaBlendMode_None);

	/// <summary>
	/// �QD�摜��]�`��
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="ExtRate">�g�嗦</param>
	/// <param name="Radian">��]�p�x</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	/// <param name="LRTurn">���E���]�t���O</param>
	static void DrawRotaGraph2D(const Vec2<float>& Center, const float& ExtRate, const float& Radian,
		const std::shared_ptr<TextureBuffer>& Tex, const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans, const bool& LRTurn = false);
};