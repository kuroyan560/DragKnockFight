#pragma once
#include"Singleton.h"

static const enum FACE_TYPE { DEFAULT, DAMAGE, BREAK, DEAD, FACE_TYPE_NUM };

class FaceIcon : public Singleton<FaceIcon>
{
	friend class Singleton<FaceIcon>;
	FaceIcon();

	//Šç”wŒi
	int backGraph;

	FACE_TYPE left;
	FACE_TYPE right;

public:
	void Init();
	void Draw();

	void ChangeLeft(const FACE_TYPE& Status);
	void ChangeRight(const FACE_TYPE& Status);
};