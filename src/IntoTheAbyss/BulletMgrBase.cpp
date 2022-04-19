#include "BulletMgrBase.h"

void BulletMgrBase::Generate(const int& GraphHandle, const Vec2<float>& GeneratePos, const float& ForwardAngle, const float& Speed)
{
	for (int index = 0; index < BULLET_COUNT; ++index) {

		// �����ς݂������珈�����΂��B
		if (bullets[index].GetIsActive()) continue;

		// ��������p�x���x�N�g���Ɏ����B
		Vec2<float> generateForwardVec = { cosf(ForwardAngle), sinf(ForwardAngle) };

		bullets[index].Generate(GraphHandle, GeneratePos, generateForwardVec, Speed);
		break;
	}
}

void BulletMgrBase::Init()
{
	for (int index = 0; index < BULLET_COUNT; ++index)
	{
		bullets[index].Init();
	}
}

void BulletMgrBase::Update()
{
	for (int index = 0; index < BULLET_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!bullets[index].GetIsActive()) continue;

		bullets[index].Update();

	}
}

void BulletMgrBase::Draw()
{
	for (int index = 0; index < BULLET_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!bullets[index].GetIsActive()) continue;

		bullets[index].Draw();

	}
}
