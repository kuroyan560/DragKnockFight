#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <array>

class RoundFinishParticle {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;
	Vec2<float> dir;
	float changeLength;
	int useGraphHanlde;
	bool isActive;
	bool isReturn;

	const float MIN_MOVE_LENGTH = 150.0f;
	const float MAX_MOVE_LENGTH = 200.0f;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	RoundFinishParticle();

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& CharaPos, const int& UseGraphHandle);

	// �X�V����
	void Update(const Vec2<float>& LeftCharaPos);

	// �`�揈��
	void Draw();

};

class RoundFinishParticleMgr : public Singleton<RoundFinishParticleMgr> {

public:

	/*===== �����o�ϐ� =====*/

	static const int MAX_PARTICLE = 70;
	std::array<RoundFinishParticle, MAX_PARTICLE> particles;

	std::array<int, 3> particleGraph;

	int easingTimer;
	const int EASING_TIMER = 90;


public:

	/*===== �����o�֐� ======*/

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& CharaPos);

	// �X�V����
	void Update(const Vec2<float>& LeftCharaPos);

	// �`�揈��
	void Draw();

	// �߂�B
	void SetReturn();


};