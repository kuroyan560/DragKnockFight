#pragma once
#include"Singleton.h"
class GUI : public Singleton<GUI>
{
	friend class Singleton<GUI>;
	GUI();

public:
	void Draw();
};

