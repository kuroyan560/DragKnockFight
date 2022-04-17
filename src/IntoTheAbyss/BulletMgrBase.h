#pragma once
#include <memory>
#include <array>
#include"Bullet.h"
class BulletMgrBase
{
public:
	BulletMgrBase() {}
	static const int BULLET_COUNT = 150;	// �e�̍ő吔
	std::array<Bullet, BULLET_COUNT> bullets;	// �e�N���X

	void Init();
	void Update();
	void Draw();
	void Generate(const int& GraphHandle, const Vec2<float>& GeneratePos, const float& ForwardAngle, const float& Speed);
};
