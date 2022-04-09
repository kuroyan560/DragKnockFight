#pragma once
#include "Vec.h"
#include "Singleton.h"
#include<vector>
#include<array>
#include"ShakeMgr.h"
#include"Camera.h"
#include"KuroMath.h"

class ScrollMgr : public Singleton<ScrollMgr> {

public:
	/*===== �����o�ϐ� =====*/
	Vec2<float> honraiScrollAmount;

	Vec2<float> scrollAmount;		//�X�N���[����
	float zoom;			// �J�����̃Y�[����

	const std::vector<std::vector<int>> *mapChipPtr;//�}�b�v�`�b�v���̃|�C���^
	std::array<bool, 4> hitData;			//�㉺���E�A�ǂ̕����ɃX�N���[���Ǘ��p�̃}�b�v�`�b�v�Ɉ�������������
	std::array<bool, 4> oldHitData;			//�㉺���E�A�ǂ̕����ɃX�N���[���Ǘ��p�̃}�b�v�`�b�v�Ɉ��������������̑O�t���[�����
	std::array<bool, 4> stopScrollData;		//�㉺���E�A�ǂ̕����ɍs���Ȃ������邩
	Vec2<float> oldScrollData;

	std::array<Vec2<float>, 4> detectPos;//�X�N���[�����m�p�̍��W

	Vec2<float> scrollMaxValue;
	Vec2<float> scrollMinValue;


public:
	ScrollMgr();

	/*===== �����o�֐� =====*/
	//������
	inline void Init(const std::vector<std::vector<int>> *CHIP_DATA)
	{
		//scrollAmount = Vec2<float>(WIN_WIDTH_HALF, WIN_HEIGHT_HALF);
		//honraiScrollAmount = scrollAmount;

		mapChipPtr = CHIP_DATA;
		oldHitData[0] = true;
		oldHitData[1] = true;
		oldHitData[2] = true;
		oldHitData[3] = true;
	};

	void Update();


	void DetectMapChipForScroll(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE = Vec2<float>(1280 / 2.0f, 720 / 2.0f));

	void CalucurateScroll(const Vec2<float> &SCROLL_DATA);


	void WarpScroll(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE = Vec2<float>(1280 / 2.0f, 720 / 2.0f));
	void Restart();
	void AlimentScrollAmount();


	enum { UP, DOWN, LEFT, RIGHT };

	bool warpFlag;

	Vec2<float> CaluStartScrollLine(const Vec2<float> &SIZE)
	{
		Vec2<float> distance(detectPos[RIGHT].x, detectPos[DOWN].y);
		Vec2<float> startPos;

		float adj = 50.0f / 2.0f;

		startPos.x = (detectPos[LEFT].x + SIZE.x);
		startPos.y = (detectPos[UP].y + SIZE.y);

		return startPos;
	};


	Vec2<float> CaluEndScrollLine(const Vec2<float> &SIZE)
	{
		Vec2<float> distance(detectPos[RIGHT].x, detectPos[DOWN].y);
		Vec2<float> endPos;

		float adj = 50.0f / 2.0f;

		endPos.x = (distance.x - SIZE.x);
		endPos.y = (distance.y - SIZE.y);

		return endPos;
	};

	void StopScroll(const int &DIR);

	Vec2<float>Affect(const Vec2<float>&Pos)	//�X�N���[���ƃY�[����K�p������
	{
		Vec2<float> scrollShakeZoom = scrollAmount + ShakeMgr::Instance()->shakeAmount;
		scrollShakeZoom.x *= zoom;
		scrollShakeZoom.y *= zoom;
		return Pos * zoom - scrollShakeZoom;
	}
};