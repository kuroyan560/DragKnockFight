#pragma once
#include<memory>
class GraphicsPipeline;
class ConstantBuffer;

#include"Color.h"
#include"KuroMath.h"
#include"SpriteMesh.h"
#include"Transform2D.h"


class Sprite
{
private:
	static std::shared_ptr<GraphicsPipeline>PIPELINE[AlphaBlendModeNum];
	//���̃x�^�h��e�N�X�`��
	static std::shared_ptr<TextureBuffer>DEFAULT_TEX;

private:
	//�萔�o�b�t�@���M�p�f�[�^
	struct ConstantData
	{
		Matrix mat;
		Color color;
	}constData;

	//�萔�o�b�t�@
	std::shared_ptr<ConstantBuffer>constBuff;

	//�e�N�X�`���o�b�t�@
	std::shared_ptr<TextureBuffer>texBuff;

public:
	//�g�����X�t�H�[��
	Transform2D transform;
	//���b�V���i���_���j
	SpriteMesh mesh;

	//�e�N�X�`���A�X�v���C�g��
	Sprite(const std::shared_ptr<TextureBuffer>& Texture = nullptr, const char* Name = nullptr);

	//�e�N�X�`���Z�b�g
	void SetTexture(const std::shared_ptr<TextureBuffer>& Texture);

	//�F�Z�b�g�i�`��̍ۂɂ��̒l����Z�����j
	void SetColor(const Color& Color);
	
	//�`��
	void Draw(const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);
};