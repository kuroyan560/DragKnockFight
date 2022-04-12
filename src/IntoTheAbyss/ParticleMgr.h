#pragma once
#include"Singleton.h"
#include<memory>
#include"Vec.h"
#include"Color.h"
#include"WinApp.h"

class VertexBuffer;
class ComputePipeline;
class GraphicsPipeline;
class ConstantBuffer;
class TextureBuffer;
class LightManager;

static const enum PARTICLE_TYPE { DASH, BULLET, HIT_MAP, FIRST_DASH, FIRST_DASH_DOUJI, DOSSUN, CRASH };

class ParticleMgr : public Singleton<ParticleMgr>
{
	static const int MAX_NUM = 300;

	static const enum PARTICLE_CUMPUTE_TYPE { NORMAL_SMOKE, FAST_SMOKE, DEFAULT = NORMAL_SMOKE };
	struct Particle
	{
		Vec2<float> pos;	//���W
		Vec2<float>emitPos;
		Vec2<float>emitVec;	//���o�x�N�g��
		float speed;	//�X�s�[�h
		float emitSpeed;	//���o����̃X�s�[�h
		float scale;	//�X�P�[��
		float emitScale;	//���o����̃X�P�[��
		float radian;	//��]�p�x
		float emitRadian;	//���o����̉�]�p�x
		float alpha;	//�A���t�@�l
		int life = 0;	//��������Ă���̃^�C�}�[
		int lifeSpan;	//����
		char isAlive = 0;		// �����t���O
		unsigned int texIdx;	//�e�N�X�`���ԍ�
		unsigned int type = PARTICLE_CUMPUTE_TYPE::DEFAULT;	//�p�[�e�B�N���������

		// ��������
		void Generate(const Vec2<float>& GeneratePos, const Vec2<float>& EmitVec, const int& Type, const int& TexIdx);
	};
	struct ZoomAndScroll
	{
		float zoom = 0.0f;
		Vec2<float>scroll = { 0.0f,0.0f };
	};

	friend class Singleton<ParticleMgr>;
	ParticleMgr();

	//���ɐ�������p�[�e�B�N���̃C���f�b�N�X
	int idx = 0;
	//�p�[�e�B�N���C���X�^���X
	Particle particles[MAX_NUM];
	//�p�[�e�B�N���̏��i�[�o�b�t�@�A�`��ɗ��p
	std::shared_ptr<VertexBuffer>buff;
	//�p�[�e�B�N���̍X�V���s���R���s���[�g�p�C�v���C��
	std::shared_ptr<ComputePipeline>cPipeline;
	//�`��p�p�C�v���C��
	std::shared_ptr<GraphicsPipeline>gPipeline;

	//�Y�[���ƃX�N���[���𑗐M���邽�߂̃o�b�t�@
	std::shared_ptr<ConstantBuffer>zoomAndScroll;

	//�e�N�X�`���͓����T�C�Y�ł���K�v������
	static const int TEX_SIZE = 64;
	static const int SMOKE_NUM = 4;
	static const enum PARTICLE_TEX { WHITE, SMOKE_0, SMOKE_1, SMOKE_2, SMOKE_3, TEX_NUM = 9 };
	std::shared_ptr<TextureBuffer>textures[TEX_NUM];

	void EmitParticle(const Vec2<float>& EmitPos, const Vec2<float>& EmitVec, const int& Type, const int& TexIdx);

public:
	void Init();
	void Update();
	void Draw(LightManager& LigManager);

	// ��������
	void Generate(const Vec2<float>& EmitPos, const Vec2<float>& EmitVec, const PARTICLE_TYPE& Type);
};