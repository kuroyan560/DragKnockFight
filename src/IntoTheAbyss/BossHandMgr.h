#pragma once
#include"BossHand.h"
#include<memory>

class BossHandMgr
{
public:
	BossHandMgr();

	std::unique_ptr<BossHand>hand;
};

