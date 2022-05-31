#pragma once
#include"../Common/Singleton.h"
#include"../KuroEngine.h"

class StageSelectTransfer: public Singleton<StageSelectTransfer>
{
public:
	StageSelectTransfer();

	std::shared_ptr<RenderTarget>screenShot;
};
