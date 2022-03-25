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

static const enum PARTICLE_TYPE { DASH, FIRST_DASH, FISRT_DASH_DOUJI, BULLET, DOSSUN, HIT_MAP };

class ParticleMgr : public Singleton<ParticleMgr>
{
	static const int MAX_NUM = 300;
	const int GENERATE_PARTICLE = 5;							// ��������p�[�e�B�N��

	struct Particle
	{
		// �؂�グ
		static int RoundUp(int size, int align) {
			return UINT(size + align - 1) & ~(align - 1);
		}

		Vec2<float> pos;			// ���W
		Vec2<float> forwardVec;	// �ړ�����
		Vec2<float> movedVel;		// �ړ�������
		float alpha;
		float speed;		// �ړ����x
		char isAlive = 0;		// �����t���O
		float scale;
		unsigned int texIdx;	//�e�N�X�`���ԍ�

		// ��������
		void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const int& TexIdx);
	};
	struct ZoomAndScroll
	{
		float zoom;
		Vec2<float>scroll;
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
	static const int SMOKE_NUM = 3;
	static const enum PARTICLE_TEX { WHITE, SMOKE_0, SMOKE_1, SMOKE_2, SMOKE_E_0, SMOKE_E_1, SMOKE_E_2, TEX_NUM = 9 };
	std::shared_ptr<TextureBuffer>textures[TEX_NUM];

	int GetTex(const PARTICLE_TYPE& Type);

public:
	void Init();
	void Update();
	void Draw(LightManager& LigManager);

	// ��������
	void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const PARTICLE_TYPE& Type, const float& par = 1.0f);
	void GeneratePer(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const PARTICLE_TYPE& Type, const float& par = 1.0f, const int& generateCount = 5.0f);
};