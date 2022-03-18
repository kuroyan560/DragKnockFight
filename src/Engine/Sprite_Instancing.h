#pragma once
#include<memory>
class GraphicsPipeline;
class StructuredBuffer;

#include"SpriteMesh.h"
#include"KuroMath.h"
#include"Color.h"
#include<vector>
#include"Transform2D.h"

//�e�C���X�^���X�̏��
class InstanceData
{
	friend class Sprite_Instancing;
	Color color;
	bool colorDirty = false;
public:
	Transform2D transform;
	void SetColor(const Color& Color)
	{
		color = Color;
		colorDirty = true;
	}
};

//�C���X�^���V���O�`��p�X�v���C�g
class Sprite_Instancing
{
private:
	//�C���X�^���V���O�`��ő吔
	static const int INTANCING_NUM_MAX = 300;
	//�p�C�v���C��
	static std::shared_ptr<GraphicsPipeline>PIPELINE[AlphaBlendModeNum];
	//���̃x�^�h��e�N�X�`��
	static std::shared_ptr<TextureBuffer>DEFAULT_TEX;

private:
	//�\�����o�b�t�@���M�p�f�[�^
	struct StructuredData
	{
		Matrix mat;
		Color color;
	};
	std::shared_ptr<StructuredBuffer>structuredBuff;

	//�e�N�X�`���o�b�t�@
	std::shared_ptr<TextureBuffer>texBuff;

public:
	//���b�V���i���_���j�@���S�ẴC���X�^���X�ɉe��
	SpriteMesh mesh;

	//�e�C���X�^���X�̏��
	std::vector<InstanceData>instanceDatas;

	//�e�N�X�`���A�X�v���C�g��
	Sprite_Instancing(const std::shared_ptr<TextureBuffer>& Texture = nullptr, const char* Name = nullptr);

	//�e�N�X�`���Z�b�g
	void SetTexture(const std::shared_ptr<TextureBuffer>& Texture);

	//�`��
	void Draw(const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);
};