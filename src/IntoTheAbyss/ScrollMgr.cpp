#include "ScrollMgr.h"
#include "ViewPort.h"

void ScrollMgr::DetectMapChipForScroll(Vec2<float> PLAYER_POS, Vec2<float> SIZE)
{

	//�v���C���[�̍��W����ɏ㉺���E�Ɍ��m�p�̍��W��ݒ肷��@���@�J������p�ƈꏏ
	std::array<Vec2<float>, 4> screenPos;//��p
	Vec2<float> upVec, sideVec;											//�v���C���[����ǂꂭ�炢���ꂽ���Ɍ��m�p�̍��W��u����

	Vec2<float> win = SIZE;

	screenPos[UP] = { PLAYER_POS.x,PLAYER_POS.y - win.y };
	screenPos[DOWN] = { PLAYER_POS.x,PLAYER_POS.y + win.y };
	screenPos[LEFT] = { PLAYER_POS.x - win.x,PLAYER_POS.y };
	screenPos[RIGHT] = { PLAYER_POS.x + win.x,PLAYER_POS.y };



	const std::vector<std::vector<int>> chipData = *mapChipPtr;



	float chipSize = 50.0f;
	int count = 0;
	std::array<Vec2<float>, 4>tmpDetect;
	//�X�N���[���͈͂����m����
	for (int y = 0; y < chipData.size(); ++y)
	{
		for (int x = 0; x < chipData[y].size(); ++x)
		{
			if (chipData[y][x] == 2)
			{
				tmpDetect[count] = { x * chipSize,y * chipSize };
				++count;
			}
		}
	}

	{
		int leftData = 0;
		int rightData = 0;
		int upData = 0;
		int downData = 0;
		//�㉺���E�ǂ���ɂ���̂��𐮗�����
		for (int i = 0; i < 4; ++i)
		{
			//�ǂ�����ԏォ���ׂ�
			if (tmpDetect[i].y < tmpDetect[upData].y)
			{
				upData = i;
			}
			//�ǂ�����ԉ������ׂ�
			if (tmpDetect[downData].y < tmpDetect[i].y)
			{
				downData = i;
			}
			//�ǂ�����ԉE�����ׂ�
			if (tmpDetect[rightData].x < tmpDetect[i].x)
			{
				rightData = i;
			}
			//�ǂ�����ԍ������ׂ�
			if (tmpDetect[i].x < tmpDetect[leftData].x)
			{
				leftData = i;
			}
		}

		//�㉺���E�Ɏd��������
		detectPos[UP] = tmpDetect[upData];
		detectPos[DOWN] = tmpDetect[downData];
		detectPos[LEFT] = tmpDetect[leftData];
		detectPos[RIGHT] = tmpDetect[rightData];
	}





	//�ǂ�����ǂ��܂ŃX�N���[�����Ă��������߂�
	//��
	if (screenPos[UP].y < detectPos[UP].y)
	{
		hitData[UP] = false;

	}
	else
	{
		hitData[UP] = true;
	}


	//��
	if (detectPos[DOWN].y < screenPos[DOWN].y)
	{
		hitData[DOWN] = false;

	}
	else
	{
		hitData[DOWN] = true;
	}


	//��
	if (screenPos[LEFT].x < detectPos[LEFT].x)
	{
		hitData[LEFT] = false;

	}
	else
	{
		hitData[LEFT] = true;
	}


	//�E
	if (detectPos[RIGHT].x < screenPos[RIGHT].x)
	{
		hitData[RIGHT] = false;

	}
	else
	{
		hitData[RIGHT] = true;
	}


	//�X�N���[���̍ő�l�����߂�v�Z��-----------------------
	Vec2<float> distance(detectPos[RIGHT].x, detectPos[DOWN].y);
	Vec2<float> startPos, endPos;

	float adj = chipSize / 2.0f;

	startPos.x = (detectPos[LEFT].x + win.x);
	endPos.x = (distance.x - win.x);

	startPos.y = (detectPos[UP].y + win.y);
	endPos.y = (distance.y - win.y);

	scrollMaxValue = endPos - startPos;
	//�X�N���[���̍ő�l�����߂�v�Z��-----------------------


	//�u���b�N�T�C�Y�����Y�����Ă���̂Œ���
	scrollMaxValue.x += adj;
	scrollMaxValue.y += adj;
	scrollMinValue = { -adj,-adj };
}

void ScrollMgr::CalucurateScroll(Vec2<float> SCROLL_DATA)
{
	if (!warpFlag)
	{
		//�㉺�ŃX�N���[���Ǘ��p�̃`�b�v�����m������X�N���[�����Ȃ�
		//if (hitData[UP] && hitData[DOWN])
		{
			honraiScrollAmount.y -= SCROLL_DATA.y;
		}
		//���E�ŃX�N���[���Ǘ��p�̃`�b�v�����m������X�N���[�����Ȃ�
		//if (hitData[LEFT] && hitData[RIGHT])
		{
			honraiScrollAmount.x -= SCROLL_DATA.x;
		}

		if (!hitData[UP])
		{
			honraiScrollAmount.y = scrollMinValue.y;
		}
		if (!hitData[DOWN])
		{
			honraiScrollAmount.y = scrollMaxValue.y;
		}

		if (!hitData[LEFT])
		{
			honraiScrollAmount.x = scrollMinValue.x;
		}
		if (!hitData[RIGHT])
		{
			honraiScrollAmount.x = scrollMaxValue.x;
		}


		for (int i = 0; i < 4; ++i)
		{
			oldHitData[i] = hitData[i];
		}
		oldScrollData = SCROLL_DATA;
	}
	else
	{
		warpFlag = false;
	}
}

void ScrollMgr::WarpScroll(Vec2<float> PLAYER_POS, Vec2<float> SIZE)
{
	//�v���C���[�̍��W����ɏ㉺���E�Ɍ��m�p�̍��W��ݒ肷��@���@�J������p�ƈꏏ
	std::array<Vec2<float>, 4> screenPos;//��p
	Vec2<float> win = SIZE;

	screenPos[UP] = { PLAYER_POS.x,PLAYER_POS.y - win.y };
	screenPos[DOWN] = { PLAYER_POS.x,PLAYER_POS.y + win.y };
	screenPos[LEFT] = { PLAYER_POS.x - win.x,PLAYER_POS.y };
	screenPos[RIGHT] = { PLAYER_POS.x + win.x,PLAYER_POS.y };

	//�ǂ�����ǂ��܂ŃX�N���[�����Ă��������߂�
	//��
	if (screenPos[UP].y < detectPos[UP].y)
	{
		hitData[UP] = false;
	}
	else
	{
		hitData[UP] = true;
	}



	//��
	if (detectPos[DOWN].y < screenPos[DOWN].y)
	{
		hitData[DOWN] = false;
	}
	else
	{
		hitData[DOWN] = true;
	}


	//��

	if (screenPos[LEFT].x < detectPos[LEFT].x)
	{
		hitData[LEFT] = false;
	}
	else
	{
		hitData[LEFT] = true;
	}

	//�E
	if (detectPos[RIGHT].x < screenPos[RIGHT].x)
	{
		hitData[RIGHT] = false;
	}
	else
	{
		hitData[RIGHT] = true;
	}

	//�X�N���[���̍ő�l�����߂�v�Z��-----------------------
	const std::vector<std::vector<int>> chipData = *mapChipPtr;

	float chipSize = 50.0f;
	Vec2<float> distance(detectPos[RIGHT].x, detectPos[DOWN].y);
	Vec2<float> startPos, endPos;

	float adj = chipSize / 2.0f;

	startPos.x = (detectPos[LEFT].x + win.x);
	endPos.x = (distance.x - win.x);

	startPos.y = (detectPos[UP].y + win.y);
	endPos.y = (distance.y - win.y);
	//�X�N���[���̍ő�l�����߂�v�Z��-----------------------


	//���A���A�E�̃��[�v���̃X�N���[���̒l���C��-----------------------
	if (hitData[DOWN] && !oldHitData[DOWN])
	{
		float tmp = endPos.x - PLAYER_POS.x;
		float tmp2 = endPos.y - PLAYER_POS.y;
		honraiScrollAmount.x = scrollMaxValue.x - tmp;
		honraiScrollAmount.y = scrollMaxValue.y - tmp2;
		warpFlag = true;
	}
	if (hitData[LEFT] && !oldHitData[LEFT])
	{
		honraiScrollAmount.x = PLAYER_POS.x - startPos.x;
		honraiScrollAmount.y = PLAYER_POS.y - startPos.y;
		warpFlag = true;
	}
	if (hitData[RIGHT] && !oldHitData[RIGHT])
	{
		float tmp = endPos.x - PLAYER_POS.x;
		float tmp2 = endPos.y - PLAYER_POS.y;
		honraiScrollAmount.x = scrollMaxValue.x - tmp;
		honraiScrollAmount.y = scrollMaxValue.y - tmp2;
		warpFlag = true;
	}
	//���A���A�E�̃��[�v���̃X�N���[���̒l���C��-----------------------

}

void ScrollMgr::Restart()
{
	scrollAmount = honraiScrollAmount;
	warpFlag = true;
}

void ScrollMgr::AlimentScrollAmount()
{
	if (honraiScrollAmount.x <= scrollMinValue.x)
	{
		honraiScrollAmount.x = scrollMinValue.x;
	}
	if (honraiScrollAmount.y <= scrollMinValue.y)
	{
		honraiScrollAmount.y = scrollMinValue.y;
	}


	if (scrollMaxValue.x <= honraiScrollAmount.x)
	{
		honraiScrollAmount.x = scrollMaxValue.x;
	}
	if (scrollMaxValue.y <= honraiScrollAmount.y)
	{
		honraiScrollAmount.y = scrollMaxValue.y;
	}

}

void ScrollMgr::StopScroll(const int &DIR)
{
	stopScrollData[DIR] = true;
}
