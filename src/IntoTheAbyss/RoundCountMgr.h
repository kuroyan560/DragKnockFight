#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <array>

// ���E���hUI�P��
class RoundCount {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;		// ���W
	Vec2<float> exp;
	int alpha;
	float angle;
	bool isActive;			// �`�悷�邩
	bool isFill;			// ���܂��Ă��邩(�������`�悷�邩)

public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	RoundCount();

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& Pos);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw(const int& FrameGraph, const int& InnerGraph);

	// ���߂�B
	inline void SetFill() { isFill = true; }

};

// ���E���hUI�Ǘ��N���X
class RoundCountMgr : public Singleton<RoundCountMgr> {

public:

	/*===== �����o�ϐ� =====*/

	std::array<RoundCount, 3> counter;

	int nowRound;		// ���E���h�� �����Ŏg�p���郉�E���h����1~�̒l �z��̗v�f���Ƃ��Ďg�p����ۂɂ�-1����K�v����B ���̒l�̓��E���h���I�������ɉ��Z�����B
	int maxRound;

	int frameGraph;
	int innerGraph;
	int roundGraph;

public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	RoundCountMgr();

	// ����������
	void Init(const int& MaxRound);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// ���E���h���C���N�������g
	void RoundIncrement();

};