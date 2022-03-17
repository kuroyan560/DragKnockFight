#pragma once
#include"Vec.h"
#include"Color.h"

enum AuraDir
{
	AURA_DIR_UPORDOWN,		//上下に伸びている
	AURA_DIR_LEFTORRIGHT	//左右に伸びている
};

class AuraBlock
{
public:
	AuraBlock();
	void Init(Vec2<float> POS, Vec2<float> SIZE, AuraDir UPDOWN_OR_SIDE_FLAG);
	void Finalize();
	void Update();
	void Draw();

	Vec2<float> leftUpPos, rightDownPos;

	AuraDir eDir;

	AuraDir GetDir()
	{
		return eDir;
	}
private:
	//int color;
	Color color;
};

