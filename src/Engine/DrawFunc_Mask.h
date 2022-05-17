#pragma once
#include"D3D12Data.h"
#include<memory>
#include<vector>
#include"Vec.h"

//�`��͈͂��}�X�N�i�͈͊O�͎w�肵���A���t�@�l�ŕ`��j
class DrawFunc_Mask
{
	//DrawExtendGraph
	static int DRAW_EXTEND_GRAPH_COUNT;

	//DrawRotaGraph
	static int DRAW_ROTA_GRAPH_COUNT;

	static int DRAW_BY_MASK_GRAPH_COUNT;

public:
	//�Ăяo���J�E���g���Z�b�g
	static void CountReset()
	{
		DRAW_EXTEND_GRAPH_COUNT = 0;
		DRAW_ROTA_GRAPH_COUNT = 0;
		DRAW_BY_MASK_GRAPH_COUNT = 0;
	}

	/// <summary>
	/// �摜�`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="MaskLeftUpPos">�}�X�N�͈͂̍�����W</param>
	/// <param name="MaskRightBottomPos">�}�X�N�͈͂̉E�����W</param>
	/// <param name="Miror">���]�t���O</param>
	/// <param name="MaskAlpha">�}�X�N�͈͊O�̃A���t�@�l</param>
	static void DrawGraph(const Vec2<float>& LeftUpPos, const std::shared_ptr<TextureBuffer>& Tex,
		const Vec2<float>& MaskLeftUpPos, const Vec2<float>& MaskRightBottomPos,
		const Vec2<bool>& Miror = { false,false }, const float& MaskAlpha = 0.0f);

	/// <summary>
	///  �g��k���`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="RightBottomPos">��`�̉E�����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="MaskLeftUpPos">�}�X�N�͈͂̍�����W</param>
	/// <param name="MaskRightBottomPos">�}�X�N�͈͂̉E�����W</param>
	/// <param name="Miror">���]�t���O</param>
	/// <param name="MaskAlpha">�}�X�N�͈͊O�̃A���t�@�l</param>
	static void DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const std::shared_ptr<TextureBuffer>& Tex,
		const Vec2<float>& MaskLeftUpPos, const Vec2<float>& MaskRightBottomPos,
		const Vec2<bool>& Miror = { false,false }, const float& MaskAlpha = 0.0f);

	/// <summary>
	/// �QD�摜��]�`��
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="ExtRate">�g�嗦</param>
	/// <param name="Radian">��]�p�x</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="MaskCenterPos">�}�X�N�͈͒��S���W</param>
	/// <param name="MaskSize">�}�X�N�͈̓T�C�Y</param>
	/// <param name="RotaCenterUV">��]�̒��SUV</param>
	/// <param name="Miror">���]�t���O</param>
	/// <param name="MaskAlpha">�}�X�N�͈͊O�̃A���t�@�l</param>
	static void DrawRotaGraph2D(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian, const std::shared_ptr<TextureBuffer>& Tex,
		const Vec2<float>& MaskCenterPos, const Vec2<float>& MaskSize,
		const Vec2<float>& RotaCenterUV = { 0.5f,0.5f }, const Vec2<bool>& Miror = { false,false }, const float& MaskAlpha = 0.0f);

	/// <summary>
	/// �����̕`��i�摜�j
	/// </summary>
	/// <param name="FromPos">�N�_���W</param>
	/// <param name="ToPos">�I�_���W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Thickness">���̑���</param>
	/// <param name="MaskLeftUpPos">�}�X�N�͈͂̍�����W</param>
	/// <param name="MaskRightBottomPos">�}�X�N�͈͂̉E�����W</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	/// <param name="Miror">���]�t���O</param>
	static void DrawLine2DGraph(const Vec2<float>& FromPos, const Vec2<float>& ToPos, const std::shared_ptr<TextureBuffer>& Tex, const int& Thickness,
		const Vec2<float>& MaskLeftUpPos, const Vec2<float>& MaskRightBottomPos, const Vec2<bool>& Mirror = { false,false });

	/// <summary>
	/// �}�X�N�摜����ɕ`��i�}�X�N�摜�͈̔͂����e�N�X�`���`��j
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="MaskCenter">�}�X�N�摜�̒��S���W</param>
	/// <param name="MaskTex">�}�X�N�摜</param>
	static void DrawGraphByMaskGraph(const Vec2<float>& Center, const std::shared_ptr<TextureBuffer>& Tex, const Vec2<float>& MaskCenter, const std::shared_ptr<TextureBuffer>& MaskTex);

};

