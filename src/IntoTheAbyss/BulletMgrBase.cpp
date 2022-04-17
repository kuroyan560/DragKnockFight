#include "BulletMgrBase.h"

void BulletMgrBase::Generate(const int& GraphHandle, const Vec2<float>& GeneratePos, const float& ForwardAngle, const float& Speed)
{
	for (int index = 0; index < BULLET_COUNT; ++index) {

		// 生成済みだったら処理を飛ばす。
		if (bullets[index].GetIsActive()) continue;

		// 生成する角度をベクトルに治す。
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

		// 未生成だったら処理を飛ばす。
		if (!bullets[index].GetIsActive()) continue;

		bullets[index].Update();

	}
}

void BulletMgrBase::Draw()
{
	for (int index = 0; index < BULLET_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!bullets[index].GetIsActive()) continue;

		bullets[index].Draw();

	}
}
