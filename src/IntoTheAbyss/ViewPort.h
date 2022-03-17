#pragma once
#include"Vec.h"
#include<array>
#include"ViewPortCollider.h"
#include"Singleton.h"

/// <summary>
/// �r���[�|�[�g�Ɋւ��鏈��
/// </summary>
class ViewPort : public Singleton<ViewPort>
{
public:

	void Init(Vec2<float> PLAYER_POS, Vec2<float> WINDOW_SIZE);
	void Update(const Vec2<float>& playerPos);
	void Draw();

	void MoveLine(int LINE, float ZOOM_RATE);

	void SetPlayerPosX(float PLAYER_POS);
	void SetPlayerPosY(float PLAYER_POS);

	// �����߂�����
	void PushBackAura(const Vec2<float>& pushBackAmount);
	void PushBackAuraHoldUp(const float& pushBackAmount);
	void PushBackAuraHoldDown(const float& pushBackAmount);
	void PushBackAuraHoldRight(const float& pushBackAmount);
	void PushBackAuraHoldLeft(const float& pushBackAmount);
	void PushBackLine(int LINE, const float& pushBackAmount);

	// �E�B���h�E�T�C�Y���ςɂ���B
	void AddAddLineValue(const float& value, const int& ID);
	void SavePrevFlamePos();

	float zoomRate;
	Vec2<float> windowSize;


	enum { LEFT_UP, LEFT_DOWN, RIGHT_UP, RIGHT_DOWN };
	std::array<Vec2<float>, 4> pointPos;//��`���\������l�̓_�̏��
	std::array<VertexData, 4> pointData;	// �����蔻��Ɏg�p���钸�_�f�[�^ �O�t���[���̍��W�Ȃǂ�ۑ����Ă���B
	Vec2<float> centralPos;
	Vec2<float> playerPos, prevPlayerPos;
	array<bool, 4> enableAuraFlags;//�����̕����Ɠ������Ă��邩

public:

	enum Dir { UP, DOWN, LEFT, RIGHT };
	std::array<Vec2<float>, 4> initPointPos;//��`���\������l�̓_�̏��̏����l
	Vec2<float> honraiCentralPos;


	array<bool, 4>hitLineFlag;//�����̕ӂƓ���������
	array<float, 4>addLineValue;//�����̕ӂƓ���������
	array<float, 4>honraiAddLineValue;//�����̕ӂƓ���������
	array<float, 4> increLineVel;
	array<int, 4> countHitLine;//�e���ӂƓ��������񐔂��L�^����
	array<float, 4>addVel;		//���v���C���[���e���������r���ǂ̕����Ɍ����Ă��邩
	float decreVel;
	array<int, 4> intervalTimer;
	array<bool, 4> holdFlags;//�����̕����Ɠ������Ă��邩
	array<bool, 4> noHitFlags;//�ǂ̕ӂ̓����蔻��𖳌��ɂ��邩


	bool isHitRight;		// X���ŃI�[���ɓ������Ă��邩�ǂ����̃t���O
	bool isHitLeft;			// X���ŃI�[���ɓ������Ă��邩�ǂ����̃t���O
	bool isHitBottom;		// Y���ŃI�[���ɓ������Ă��邩�ǂ����̃t���O
	bool isHitTop;			// Y���ŃI�[���ɓ������Ă��邩�ǂ����̃t���O


	bool zoomFlag;
	const float zoomValue = 0.1f;		//�Y�[����
	const float decreVelValue = 0.001f;	//�Y�[���̌��炷�ړ���
	float zoomVel;

	bool RayHit(Vec2<float> a, Vec2<float> b, Vec2<float> c, Vec2<float> d)
	{
		double s, t;
		s = (a.x - b.x) * (c.y - a.y) - (a.y - b.y) * (c.x - a.x);
		t = (a.x - b.x) * (d.y - a.y) - (a.y - b.y) * (d.x - a.x);
		if (s * t > 0)
			return false;

		s = (c.x - d.x) * (a.y - c.y) - (c.y - d.y) * (a.x - c.x);
		t = (c.x - d.x) * (b.y - c.y) - (c.y - d.y) * (b.x - c.x);
		if (s * t > 0)
			return false;
		return true;
	}
};
