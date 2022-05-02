#include "Stamina.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"

Stamina::Stamina()
{

	/*===== 初期化 =====*/

	nowGauge = 100;
	isActivate = true;

}

void Stamina::Init()
{

	/*===== 初期化 =====*/

	nowGauge = 100;
	isActivate = true;

}

void Stamina::Empty()
{

	/*===== 初期化 =====*/

	nowGauge = 0;
	isActivate = false;

}

void Stamina::Update()
{

	/*===== 更新処理 =====*/

	if (nowGauge < MAX_GAUGE) {

		isActivate = false;

	}
	else {

		isActivate = true;

	}

}

void Stamina::Draw(const Vec2<float>& DrawPos, const float& Width, const float& Height)
{

	/*===== 描画処理 =====*/

	// スタミナが溜まっている部分を描画
	float per = nowGauge / MAX_GAUGE;

	// スタミナがマックスだったら
	if (1.0f <= per) {

		// アウトライン
		Vec2<float> offset = { 2,2 };
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos - offset), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height) + offset), Color(0x29, 0xC9, 0xB4, 0xFF), true);

		// 内側に明るい四角形を描画
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height)), Color(0x2F, 0xFF, 0x8B, 0xFF), true);

	}
	else {

		Vec2<float> nowGaugePos = DrawPos + Vec2<float>(Width * per, 0);
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos), ScrollMgr::Instance()->Affect(nowGaugePos + Vec2<float>(0, Height)), Color(0x2F, 0xFF, 0x8B, 0xFF), true);

		// スタミナが溜まっていない部分を描画
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(nowGaugePos), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height)), Color(0x2F, 0xFF, 0x8B, 0x55), true, AlphaBlendMode_Trans);

	}

}

float Stamina::AddNowGauge(const float& Add)
{

	/*===== ゲージ量を足す =====*/

	nowGauge += Add;

	// 上限を超えていたら差分をリターンする。
	if (MAX_GAUGE <= nowGauge) {

		float sub = nowGauge - MAX_GAUGE;

		nowGauge -= sub;

		isActivate = true;

		// 超過量を返す。このゲージの次のゲージに足すため。
		return sub;
	}

	//下限を超えていたら差分をリターンする
	if (nowGauge < 0) {

		float sub = 0 - nowGauge;

		nowGauge += sub;

		return sub;
	}

	return 0;

}

StaminaMgr::StaminaMgr()
{

	// 初期化 仮でスタミナゲージを5個設定しています。
	const int KARI = 5;
	for (int index = 0; index < KARI; ++index) {
		stamina.push_back(Stamina());
	}

}

void StaminaMgr::Init()
{

	// 初期化 全てを埋める。
	const int SIZE = stamina.size();
	for (int index = 0; index < SIZE; ++index) {
		stamina[index].Init();
	}

}

void StaminaMgr::Update(const bool& Heal)
{
	static const float HEAL_AMOUNT = 1.0f;
	// スタミナゲージは何もしてなくても少しずつ回復する。
	const int STAMINA_COUNT = stamina.size();

	if (Heal)
	{
		for (int index = 0; index < STAMINA_COUNT; ++index) {

			// 既にマックスだったら処理を飛ばす。
			if (stamina[index].GetIsActivate()) continue;

			stamina[index].AddNowGauge(HEAL_AMOUNT);

			// 手前側から一つずつ順々に回復していくため、リターン。
			break;

		}
	}

	// 更新処理
	for (int index = 0; index < STAMINA_COUNT; ++index) {

		stamina[index].Update();

	}

}

void StaminaMgr::Draw(const Vec2<float>& CharaPos)
{

	static const int STAMINA_GAUGE_WIDTH = 60;
	static const int STAMINA_GAUGE_WIDTH_PER_ONE = 20;
	static const int STAMINA_GAUGE_HEIGHT = 5;
	static const int STAMINA_GAUGE_OFFSET_Y = -64;
	static const int STAMINA_GAUGE_OFFSET_X = 5;
	{

		const int STAMINA_COUNT = stamina.size();
		for (int index = 0; index < STAMINA_COUNT; ++index) {

			// 描画する座標を計算。
			Vec2<float> drawPos = CharaPos;
			drawPos.y += STAMINA_GAUGE_OFFSET_Y;
			drawPos.x = (drawPos.x - STAMINA_GAUGE_WIDTH) + (index * STAMINA_GAUGE_WIDTH_PER_ONE) + (index * STAMINA_GAUGE_OFFSET_X);

			// 描画する座標を渡して描画！
			stamina[index].Draw(drawPos, STAMINA_GAUGE_WIDTH_PER_ONE, STAMINA_GAUGE_HEIGHT);

		}

	}

}

void StaminaMgr::ConsumesStamina(const int& ConsumesStamina)
{

	/*===== スタミナを消費 =====*/

	// スタミナを消費。
	float saveStamina = 0;	// 先頭にあるたまりきっていないスタミナを消さないようにするための変数。
	for (int consumeCount = 0; consumeCount < ConsumesStamina; ++consumeCount) {

		for (int index = stamina.size() - 1; 0 <= index; --index) {

			// たまりきっていなかったら処理を飛ばす。
			if (!stamina[index].GetIsActivate()) {

				// 値がちょっとでも入っていたら保存しておく。
				saveStamina = stamina[index].GetNowGauge();
				stamina[index].Empty();

				continue;
			}

			// 溜まっていたら値を消す。
			stamina[index].Empty();
			break;

		}

	}

	// 最後尾にセーブしておいたスタミナを追加。
	for (int index = 0; index < stamina.size(); ++index)
	{
		if (stamina[index].GetIsActivate())continue;
		stamina[index].AddNowGauge(saveStamina);
		break;
	}
}

bool StaminaMgr::CheckCanAction(const int& ConsumesStamina)
{

	/*===== 指定したアクションができるかどうか ====*/

	int stainaCounter = 0;

	const int STAMINA_COUNT = stamina.size();
	for (int index = 0; index < STAMINA_COUNT; ++index) {

		stainaCounter += stamina[index].GetIsActivate() ? 1 : 0;

	}

	return ConsumesStamina <= stainaCounter;

}

bool StaminaMgr::ConsumesStaminaByGauge(const float& CounsumeStaminaGauge)
{
	float consumeAmount = CounsumeStaminaGauge;

	for (int index = stamina.size() - 1; 0 <= index; --index) {

		//少しでも残っていたら消費
		if (stamina[index].GetNowGauge()) {

			consumeAmount = stamina[index].AddNowGauge(-consumeAmount);

			//消費完了
			if (consumeAmount <= 0)
			{
				return true;
			}
		}
	}

	//消費しきれなかった
	return false;
}
