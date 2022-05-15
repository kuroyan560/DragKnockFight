#include "PlayerAnimation.h"
#include"TexHandleMgr.h"
#include"D3D12Data.h"

PlayerAnimation::PlayerAnimation(const std::vector<Anim>& Anims) : animations(Anims)
{
}

Vec2<int> PlayerAnimation::GetGraphSize()
{
	return TexHandleMgr::GetTexBuffer(animations[status].graph[idx])->GetGraphSize();
}

bool PlayerAnimation::Compare(const std::vector<int>& Status)
{
	for (auto& s : Status)
	{
		if (status == s)return true;
	}
	return false;
}

#include"SlowMgr.h"
void PlayerAnimation::Update()
{
	auto& anim = animations[status];
	timer += SlowMgr::Instance()->slowAmount;
	if (anim.interval <= timer)
	{
		idx++;
		if (anim.graph.size() == idx)
		{
			if (anim.loop)
			{
				idx = 0;
			}
			else
			{
				idx--;
			}
		}
		timer = 0;
	}
}