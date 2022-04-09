#include "SwingMgr.h"

SwingMgr::SwingMgr()
{

	/*===== コンストラクタ =====*/

	isSwingPlayer = false;
	isSwingBoss = false;
	easingTimer = 0;
	easingStartVec = {};
	easingEndVec = {};

}

void SwingMgr::Init()
{

	/*===== 初期化処理 =====*/

	isSwingPlayer = false;
	isSwingBoss = false;
	easingTimer = 0;
	easingStartVec = {};
	easingEndVec = {};

}

void SwingMgr::Update()
{

	/*===== 更新処理 =====*/

}
