#include "Enemy.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "BulletMgr.h"
#include "Bullet.h"
#include "Collider.h"
#include "ParticleMgr.h"

#include"WinApp.h"
#include"DrawFunc.h"

Enemy::Enemy()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*-- �R���X�g���N�^ --*/

	// �e�ϐ���������
	pos = { WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f - 100.0f };
	//forwardVec = { 1.0f,0.0f };

}

void Enemy::Init()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*-- ���������� --*/

	// �e�ϐ���������
	pos = { WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f - 100.0f };
	//forwardVec = { 1.0f,0.0f };

}

void Enemy::Update(const Vec2<float>& playerPos)
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	//const int WIN_HEIGHT = WinApp::Instance()->GetExpandWinSize().y;

	/*-- �X�V���� --*/

	// �ړ��������s���B
	//pos += forwardVec * Vec2<float>(SPEED, SPEED);

	//// �����߂��������s���B
	//if (pos.x + SIZE.x >= WIN_WIDTH) {

	//	// �����߂��B
	//	pos.x = WIN_WIDTH - SIZE.x;

	//	// �ړ������𔽓]������B
	//	forwardVec.x *= -1.0f;

	//}
	//if (pos.x - SIZE.x <= 0) {

	//	// �����߂��B
	//	pos.x = SIZE.x;

	//	// �ړ������𔽓]������B
	//	forwardVec.x *= -1.0f;

	//}

	// �v���C���[�ɋ߂Â���B
	pos.x += (playerPos.x - pos.x) / 100.0f;
	pos.y += (playerPos.y - pos.y) / 100.0f;

	isHit = false;

}

#include"D3D12App.h"
void Enemy::Draw()
{

	/*-- �`�揈�� --*/
	//DrawBox(pos.x - SIZE.x, pos.y - SIZE.y, pos.x + SIZE.x, pos.y + SIZE.y, GetColor(0xE5, 0x80, 0x80), TRUE);
	DrawFunc::DrawBox2D({ pos.x - SIZE.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
		pos.y - SIZE.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
		{ pos.x + SIZE.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
		pos.y + SIZE.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
		Color(229, 128, 128, 255), D3D12App::Instance()->GetBackBuffFormat(), true);


	if (isHit) {

		DrawFunc::DrawBox2D({ pos.x - SIZE.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y - SIZE.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			{ pos.x + SIZE.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y + SIZE.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			Color(255, 0, 0, 255), D3D12App::Instance()->GetBackBuffFormat(), true);

	}


}

void Enemy::CheckHitBullet()
{

	/*===== �e�ƓG�̓����蔻�� =====*/

	const int BULLET_COUNT = BulletMgr::Instance()->BULLET_COUNT;
	for (int index = 0; index < BULLET_COUNT; ++index) {

		// �e����������Ă��Ȃ������珈�����΂��B
		if (!BulletMgr::Instance()->bullets[index]->isActive)continue;

		// �����蔻����s���B
		Vec2<float> bulletPos = BulletMgr::Instance()->bullets[index]->pos;
		Vec2<float> bulletSize = { BulletMgr::Instance()->bullets[index]->MAX_RADIUS,BulletMgr::Instance()->bullets[index]->MAX_RADIUS };
		Vec2<float> bulletSpeed = { BulletMgr::Instance()->bullets[index]->speed,BulletMgr::Instance()->bullets[index]->speed };
		bool isHitVel = Collider::Instance()->CheckHitVel(bulletPos, BulletMgr::Instance()->bullets[index]->prevPos, bulletSpeed, bulletSize, pos, SIZE);
		bool isHitTop = Collider::Instance()->CheckHitSize(bulletPos, bulletSize, pos, SIZE, INTERSECTED_TOP) != INTERSECTED_NONE;
		bool isHitBottom = Collider::Instance()->CheckHitSize(bulletPos, bulletSize, pos, SIZE, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
		bool isHitRight = Collider::Instance()->CheckHitSize(bulletPos, bulletSize, pos, SIZE, INTERSECTED_RIGHT) != INTERSECTED_NONE;
		bool isHitLeft = Collider::Instance()->CheckHitSize(bulletPos, bulletSize, pos, SIZE, INTERSECTED_LEFT) != INTERSECTED_NONE;

		// �ǂ������炪�������Ă�����
		if (isHitVel || isHitTop || isHitBottom || isHitRight || isHitLeft) {

			// �e������
			ParticleMgr::Instance()->Generate(BulletMgr::Instance()->bullets[index]->pos, BulletMgr::Instance()->bullets[index]->forwardVec, BULLET);
			BulletMgr::Instance()->bullets[index]->Init();

			isHit = true;

		}

	}

}
