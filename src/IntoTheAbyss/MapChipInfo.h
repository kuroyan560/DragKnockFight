#pragma once
#include"Vec.h"
#include<vector>


struct MapChipDrawData
{
	int handle = -1;	//�摜�n���h��
	float radian;//�摜�̊p�x
	bool animationFlag;//�A�j���[�V������L���ɂ���t���O
	int interval;		//�A�j���[�V�����̊Ԋu
	int animationNum;//���ݎQ�Ƃ��Ă���A�j���[�V�����̉摜
	Vec2<float> offset;	//32*32�̉摜����Ȃ��ꍇ�̉摜�ʒu����
	Vec2<float> offsetInterval;//�㉺���E�̉摜�̊Ԋu���󂯂�
	float shocked = 0.0f;	//�Ռ���
	float expEaseRate = 1.0f;	//�L�k

	void Reset()
	{
		handle = -1;	//�摜�n���h��
		radian = 0.0f;//�摜�̊p�x
		animationFlag = false;//�A�j���[�V������L���ɂ���t���O
		interval = 0;		//�A�j���[�V�����̊Ԋu
		animationNum = 0;//���ݎQ�Ƃ��Ă���A�j���[�V�����̉摜
		offset = {};	//32*32�̉摜����Ȃ��ꍇ�̉摜�ʒu����
		offsetInterval = {};//�㉺���E�̉摜�̊Ԋu���󂯂�
		shocked = 0.0f;
		expEaseRate = 1.0f;
	};

	//MapChipDrawData() : handle(-1), radian(0.0f), animationFlag(false), interval(0), animationNum(0), offset(Vec2<float>(0.0f, 0.0f))
	MapChipDrawData() : radian(0.0f), animationFlag(false), interval(0), animationNum(0), offset(Vec2<float>(0.0f, 0.0f))
	{
	};
};


struct ChipInfo
{
	int chipType;
	MapChipDrawData drawData;
};

typedef std::vector<std::vector<ChipInfo>> MapChipArray;

