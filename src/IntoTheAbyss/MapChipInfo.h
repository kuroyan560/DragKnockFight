#pragma once
#include"Vec.h"
#include<vector>


struct MapChipDrawData
{
	int handle = -1;	//画像ハンドル
	float radian;//画像の角度
	bool animationFlag;//アニメーションを有効にするフラグ
	int interval;		//アニメーションの間隔
	int animationNum;//現在参照しているアニメーションの画像
	Vec2<float> offset;	//32*32の画像じゃない場合の画像位置調整
	Vec2<float> offsetInterval;//上下左右の画像の間隔を空ける
	float shocked = 0.0f;	//衝撃率
	float expEaseRate = 1.0f;	//伸縮

	void Reset()
	{
		handle = -1;	//画像ハンドル
		radian = 0.0f;//画像の角度
		animationFlag = false;//アニメーションを有効にするフラグ
		interval = 0;		//アニメーションの間隔
		animationNum = 0;//現在参照しているアニメーションの画像
		offset = {};	//32*32の画像じゃない場合の画像位置調整
		offsetInterval = {};//上下左右の画像の間隔を空ける
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

