#pragma once
class StrongSwingUI
{
	bool disappear = false;
	int timer = 0;

public:
	void Disappear();
	void Draw(const int& MyIdx);
};

#include"PlayerAnimation.h"
#include<vector>
#include<memory>
class StrongSwingTutorialUI
{
	std::shared_ptr<PlayerAnimation>rStick;
	std::shared_ptr<PlayerAnimation>ltButton;
public:
	StrongSwingTutorialUI();
	void Init();
	void Draw(const bool& CanStrongSwing);
};