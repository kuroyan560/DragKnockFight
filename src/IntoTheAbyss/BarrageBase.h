#pragma once
#include "Vec.h"
#include "BulletMgrBase.h"

// �e�����N���X
class BarrageBase{

public:

	/*===== �����o�֐� =====*/

	virtual void Start() = 0;
	virtual void Init() = 0;
	virtual bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const int& GraphHandle) = 0;	// true���A���Ă�����I���B

};