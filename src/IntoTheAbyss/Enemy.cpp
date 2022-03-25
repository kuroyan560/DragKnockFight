#include "Enemy.h"

#include"WinApp.h"
#include"DrawFunc.h"

Enemy::Enemy()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*-- �R���X�g���N�^ --*/

	// �e�ϐ���������
	pos = { WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f - 100.0f };
	forwardVec = { 1.0f,0.0f };

}

void Enemy::Init()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*-- ���������� --*/

	// �e�ϐ���������
	pos = { WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f - 100.0f };
	forwardVec = { 1.0f,0.0f };

}

void Enemy::Update()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	//const int WIN_HEIGHT = WinApp::Instance()->GetExpandWinSize().y;

	/*-- �X�V���� --*/

	// �ړ��������s���B
	pos += forwardVec * Vec2<float>(SPEED, SPEED);

	// �����߂��������s���B
	if (pos.x + SIZE.x >= WIN_WIDTH) {

		// �����߂��B
		pos.x = WIN_WIDTH - SIZE.x;

		// �ړ������𔽓]������B
		forwardVec.x *= -1.0f;

	}
	if (pos.x - SIZE.x <= 0) {

		// �����߂��B
		pos.x = SIZE.x;

		// �ړ������𔽓]������B
		forwardVec.x *= -1.0f;

	}

}

#include"D3D12App.h"
void Enemy::Draw()
{

	/*-- �`�揈�� --*/
	//DrawBox(pos.x - SIZE.x, pos.y - SIZE.y, pos.x + SIZE.x, pos.y + SIZE.y, GetColor(0xE5, 0x80, 0x80), TRUE);
	DrawFunc::DrawBox2D({ pos.x - SIZE.x, pos.y - SIZE.y }, { pos.x + SIZE.x, pos.y + SIZE.y }, Color(229, 128, 128, 255), D3D12App::Instance()->GetBackBuffFormat(), true);

}
