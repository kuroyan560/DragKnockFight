#pragma once
#include<memory>
class RenderTarget;
class ClearInfoContainer
{
public:
	ClearInfoContainer();
	int maxBreakCount = 0;
	std::shared_ptr<RenderTarget>clearInfoRenderTarget;
	bool clear = false;
};

#include"Singleton.h"
#include<vector>
class ClearInfoContainerMgr : public Singleton<ClearInfoContainerMgr>
{
	friend class Singleton<ClearInfoContainerMgr>;
	ClearInfoContainerMgr() {}
	std::vector<ClearInfoContainer>containers;
public:
	void Generate(const int& StageNum)
	{
		containers.clear();
		containers.resize(StageNum);
	}
	ClearInfoContainer& GetContainer(const int& StageNum) { return containers[StageNum]; }
};