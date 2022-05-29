#pragma once
#include "Vec.h"
#include <array>
#include "Singleton.h"

class BackGroundParticle {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;
	Vec2<float> exp;
	Vec2<float> targetExp;		// EXP���Ԃ���ۂ̖ڕW
	Vec2<float> stageSize;
	float speed;
	static float lissajousTimer;// ���T�[�W���Ȑ��Ɏg�p����^�C�}�[
	float angle;
	int timer;					// �����ƐF�X�g�p����^�C�}�[
	int desTimer;				// ���ł��ĕʂ̏ꏊ�ɍs�����߂̃^�C�}�[
	int defDesTimer;			// ���ł��ĕʂ̏ꏊ�ɍs�����߂̃^�C�}�[�̃f�t�H���g�l
	bool isActive;

	const int MAX_DES_TIMER = 600;
	const int MAX_SHOOTING_STAR_DES_TIMER = 600;
	const float MAX_SPEED = 40.0f;
	const float MIN_SPEED = 10.0f;

	enum class STATUS {

		STAY,
		SHOOTING_STAR,

	};
	STATUS status;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	BackGroundParticle();

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& Pos, const STATUS& Status, const Vec2<float>& StageSize);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �����蔻��
	void CheckHit(const Vec2<float>& Pos, const float& Size);

};

class BackGroundParticleMgr : public Singleton<BackGroundParticleMgr> {

public:

	/*===== �����o�ϐ� =====*/

	static const int PARTICLR_COUNT = 300;
	std::array<BackGroundParticle, PARTICLR_COUNT> particles;

	int shootingStarGenerateTimer;
	int defShootingStarGenerateTimer;

	Vec2<float> stageSize;


public:

	/*===== �����o�֐� =====*/

	BackGroundParticleMgr();

	// ����������
	void Init();

	// ��������
	void StageStartGenerate(const Vec2<float>& StageSize);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �L�����Ƃ̓����蔻��
	void CheckHit(const Vec2<float>& Pos, const float& Size);

};