#include "SwingMgr.h"

SwingMgr::SwingMgr()
{

	/*===== �R���X�g���N�^ =====*/

	isSwingPlayer = false;
	isSwingBoss = false;
	easingTimer = 0;
	easingStartVec = {};
	easingEndVec = {};

}

void SwingMgr::Init()
{

	/*===== ���������� =====*/

	isSwingPlayer = false;
	isSwingBoss = false;
	easingTimer = 0;
	easingStartVec = {};
	easingEndVec = {};

}

void SwingMgr::Update()
{

	/*===== �X�V���� =====*/

}
