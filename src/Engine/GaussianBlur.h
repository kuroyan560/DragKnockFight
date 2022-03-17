#pragma once
#include"PostEffect.h"

class ComputePipeline;
class ConstantBuffer;
class TextureBuffer;
class RenderTarget;

class GaussianBlur : public PostEffect
{
	static const int NUM_WEIGHTS = 8;
	//�d�݃e�[�u��
	float weights[NUM_WEIGHTS];

	//�d�݃e�[�u���̒萔�o�b�t�@
	std::shared_ptr<ConstantBuffer>weightConstBuff;
	//�e�N�X�`�����̒萔�o�b�t�@
	std::shared_ptr<ConstantBuffer>texInfoConstBuff;

	//���u���[�̌���
	std::shared_ptr<ComputePipeline>xBlurPipeline;
	std::shared_ptr<TextureBuffer>xBlurResult;

	//�c�u���[�̌���
	std::shared_ptr<ComputePipeline>yBlurPipeline;
	std::shared_ptr<TextureBuffer>yBlurResult;

	//�ŏI����
	std::shared_ptr<ComputePipeline>finalPipeline;
	std::shared_ptr<TextureBuffer>finalResult;

public:
	//�\�[�X�摜 & ���ʕ`���ݒ�A�ڂ�������
	GaussianBlur(const Vec2<int>& Size, const DXGI_FORMAT& Format, const float& BlurPower = 8.0f);
	//�{�P�
	void SetBlurPower(const float& BlurPower);
	//���s
	void Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const std::shared_ptr<TextureBuffer>& SourceTex)override;

	//���ʂ̃e�N�X�`���擾
	std::shared_ptr<TextureBuffer>& GetResultTex() { return finalResult; }

	//�E�B���h�E�T�C�Y�Ō��ʂ�`��
	void DrawResult(const AlphaBlendMode& AlphaBlend);
};