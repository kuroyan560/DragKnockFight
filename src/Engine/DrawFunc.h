#pragma once
#include"D3D12Data.h"
#include"Vec.h"
#include"Color.h"

static class DrawFunc
{
	//DrawLine
	static int DRAW_LINE_COUNT;	//�Ă΂ꂽ��

	//DrawBox
	static int DRAW_BOX_COUNT;

	//DrawCircle
	static int DRAW_CIRCLE_COUNT;

	//DrawExtendGraph
	static int DRAW_EXTEND_GRAPH_COUNT;

	//DrawRotaGraph
	static int DRAW_ROTA_GRAPH_COUNT;

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
	/// �����̕`��i�摜�j
	/// </summary>
	/// <param name="FromPos">�N�_���W</param>
	/// <param name="ToPos">�I�_���W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Thickness">���̑���</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	/// <param name="Miror">���]�t���O</param>
	static void DrawLine2DGraph(const Vec2<float>& FromPos, const Vec2<float>& ToPos, const std::shared_ptr<TextureBuffer>& Tex, const int& Thickness, const AlphaBlendMode& BlendMode = AlphaBlendMode_None, const Vec2<bool>& Mirror = { false,false });

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
	/// �摜�`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="EmitColor">�o�͂����</param>
	/// <param name="Mirror">���E���]�t���O</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawGraph(const Vec2<float>& LeftUpPos, const std::shared_ptr<TextureBuffer>& Tex, const Color& EmitColor = Color(), 
		const Vec2<bool>& Miror = { false,false },
		const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);

	/// <summary>
	/// �g��k���`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="RightBottomPos">��`�̉E����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="EmitColor">�o�͂����</param>
	/// <param name="Mirror">���E���]�t���O</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos,
		const std::shared_ptr<TextureBuffer>& Tex, const Color& EmitColor = Color(), 
		const Vec2<bool>& Miror = { false,false },const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);

	/// <summary>
	/// �QD�摜��]�`��
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="ExtRate">�g�嗦</param>
	/// <param name="Radian">��]�p�x</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="EmitColor">�o�͂����</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	/// <param name="Mirror">���E���]�t���O</param>
	static void DrawRotaGraph2D(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
		const std::shared_ptr<TextureBuffer>& Tex, const Color& EmitColor = Color(), const Vec2<float>& RotaCenterUV = { 0.5f,0.5f },
		const Vec2<bool>& Mirror = { false,false }, const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);
};