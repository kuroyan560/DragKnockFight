#pragma once
class StrongSwingUI
{
	bool disappear = false;
	int timer;

public:
	void Disappear();
	void Draw(const int& MyIdx);
};