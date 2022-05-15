#include "Stamina.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include"AudioApp.h"

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

		// マックスになったトリガーの時に一気に大きくする。
		if (!isActivate) {

			SetExp(true);

		}

		isActivate = true;

	}

	// 拡大していたらその値を0に近づける。
	expAmount.x -= expAmount.x / 5.0f;
	expAmount.y -= expAmount.y / 5.0f;

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
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos - offset - expAmount), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height) + offset + expAmount), outerColor, true);

		// 内側に明るい四角形を描画
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos - expAmount), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height) + expAmount), innerColor, true);

	}
	else {

		Vec2<float> nowGaugePos = DrawPos + Vec2<float>(Width * per, 0);
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos - expAmount), ScrollMgr::Instance()->Affect(nowGaugePos + Vec2<float>(0, Height) + expAmount), innerColor, true);

		// スタミナが溜まっていない部分を描画
		Color outerColorBuff = outerColor;
		outerColorBuff /= 3.0f;
		outerColorBuff.Alpha() = 50;
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(nowGaugePos - expAmount), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height) + expAmount), outerColorBuff, true, AlphaBlendMode_Trans);

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

		// ゲージがマックスになったので拡大する演出をつける。
		SetExp(true);

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

void Stamina::SetExp(const bool& isBig)
{

	if (isBig) {
		expAmount = { BIG_EXP_AMOUNT,BIG_EXP_AMOUNT };
	}
	else {
		expAmount = { EXP_AMOUNT,EXP_AMOUNT };
	}

}

void Stamina::SetColor(const Color& InnerColor, const Color& OuterColor)
{

	innerColor = InnerColor;
	outerColor = OuterColor;

}

int StaminaMgr::STAMINA_HEAL_SE[STAMINA_NUM] = { -1 };

StaminaMgr::StaminaMgr()
{
	if (STAMINA_HEAL_SE[0] == -1)
	{
		for (int i = 0; i < STAMINA_NUM; ++i)
		{
			STAMINA_HEAL_SE[i] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/staminaBarHeal_" + std::to_string(i) + ".wav");
		}
	}

	// 初期化 仮でスタミナゲージを5個設定しています。
	for (int index = 0; index < STAMINA_NUM; ++index) {
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

	outOfStaminaEffect.Init();
	emptyTrigger = false;
}

#include"SlowMgr.h"
void StaminaMgr::Update(const bool& Heal, const Vec2<float>& CharacterPos)
{
	const bool oldFullFlg = 100.0f <= stamina.back().GetNowGauge();

	static const float HEAL_AMOUNT = 1.5f;
	// スタミナゲージは何もしてなくても少しずつ回復する。
	const int STAMINA_COUNT = stamina.size();

	if (Heal)
	{
		for (int index = 0; index < STAMINA_COUNT; ++index) {

			// 既にマックスだったら処理を飛ばす。
			if (stamina[index].GetIsActivate()) continue;

			stamina[index].AddNowGauge(HEAL_AMOUNT * SlowMgr::Instance()->slowAmount);
			if (100.0f <= stamina[index].GetNowGauge())AudioApp::Instance()->PlayWave(STAMINA_HEAL_SE[index]);

			// 手前側から一つずつ順々に回復していくため、リターン。
			break;

		}
	}

	// 更新処理
	for (int index = 0; index < STAMINA_COUNT; ++index) {

		stamina[index].Update();

	}

	//スタミナ演出
	static const float OFFSET_Y = -32.0f;
	outOfStaminaEffect.baseEmptyStringPos = { CharacterPos.x,CharacterPos.y + OFFSET_Y };
	outOfStaminaEffect.baseMaxStringPos = { CharacterPos.x,CharacterPos.y + OFFSET_Y };
	outOfStaminaEffect.Update();

	emptyTrigger = false;

	if (!oldFullFlg && 100.0f <= stamina.back().GetNowGauge())outOfStaminaEffect.Full();
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

	outOfStaminaEffect.Draw();
}

void StaminaMgr::SetColor(const Color& InnerColor, const Color& OuterColor)
{

	const int STAMINA_COUNT = stamina.size();
	for (int index = 0; index < STAMINA_COUNT; ++index) {

		stamina[index].SetColor(InnerColor, OuterColor);

	}

}

void StaminaMgr::ConsumesStamina(const int& ConsumesStamina)
{

	/*===== スタミナを消費 =====*/

	//元々空っぽか
	const bool oldEmptyFlg = stamina.front().GetNowGauge() <= 0.0f;

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

	//空っぽ
	if (!oldEmptyFlg && stamina.front().GetNowGauge() <= 0.0f)
	{
		emptyTrigger = true;
		outOfStaminaEffect.Empty();
	}
}

void StaminaMgr::AddStamina(const int& AddStamina)
{
	/*===== スタミナを回復させる =====*/

	if (AddStamina <= 0) return;

	std::vector<int>seHandles;

	//元々最大か
	const bool oldFullFlg = 100.0f <= stamina.back().GetNowGauge();

	int addStamina = AddStamina;

	const int STAMINA_COUNT = stamina.size();
	for (int index = 0; index < STAMINA_COUNT; ++index) {

		// 既にマックスだったら処理を飛ばす。
		if (stamina[index].GetIsActivate()) continue;

		// オーバーして回復しないようにする。
		if (addStamina < 100.0f - stamina[index].GetNowGauge()) {

			stamina[index].AddNowGauge(addStamina);
			stamina[index].SetExp();
			if (100.0f <= stamina[index].GetNowGauge())seHandles.emplace_back(STAMINA_HEAL_SE[index]);

			break;

		}
		// オーバーしていたら。
		else {

			// オーバーしている量を保存する。
			addStamina -= 100.0f - stamina[index].GetNowGauge();

			// 回復する。
			stamina[index].AddNowGauge(100.0f - stamina[index].GetNowGauge());
			stamina[index].SetExp(true);
			if (100.0f <= stamina[index].GetNowGauge())seHandles.emplace_back(STAMINA_HEAL_SE[index]);
		}
	}

	AudioApp::Instance()->PlayWaveArray(seHandles);
	//最大になった
	if (!oldFullFlg && 100.0f <= stamina.back().GetNowGauge())outOfStaminaEffect.Full();
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

void StaminaMgr::ConsumesStaminaByGauge(const float& CounsumeStaminaGauge)
{
	//元々空っぽか
	const bool oldEmptyFlg = stamina.front().GetNowGauge() <= 0.0f;

	float consumeAmount = CounsumeStaminaGauge;

	for (int index = stamina.size() - 1; 0 <= index; --index) {

		//少しでも残っていたら消費
		if (stamina[index].GetNowGauge()) {

			consumeAmount = stamina[index].AddNowGauge(-consumeAmount);

			//消費完了
			if (consumeAmount <= 0)
			{
				break;
			}
		}
	}

	//空っぽ
	if (!oldEmptyFlg && stamina.front().GetNowGauge() <= 0.0f)
	{
		emptyTrigger = true;
		outOfStaminaEffect.Empty();
	}

}
