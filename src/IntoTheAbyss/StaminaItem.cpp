#include "StaminaItem.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"

StaminaItem::StaminaItem()
{

	/*===== 初期化 =====*/

	isActive = false;

}

void StaminaItem::Init()
{

	/*===== 初期化 =====*/

	isActive = false;

}

void StaminaItem::Generate(const Vec2<float>& GeneratePos, const Vec2<float>& ForwardVec, const float& HealAmount, const float& Vel, STAMINA_ITEM_ID ItemID, const bool& IsAcquired, Vec2<float>* CharaPos, CHARA_ID CharaID)
{

	/*===== 生成処理 =====*/

	// 変数を代入する。
	pos = GeneratePos;
	forwardVec = ForwardVec;
	healAmount = HealAmount;
	vel = Vel;
	isAcquired = IsAcquired;
	itemID = ItemID;
	charaID = CharaID;
	isActive = true;

	// 初めから取得されている状態だったら参照を保存する。
	if (isAcquired && CharaPos != nullptr) {

		charaPos = CharaPos;

	}

}

void StaminaItem::Update()
{

	/*===== 更新処理 =====*/

	float prevVel = 0;
	float moveAngle = 0;

	// IDによって処理を分ける。
	switch (itemID)
	{
	case StaminaItem::STAMINA_ITEM_ID::SPONE_ITEM:

		/*-- スポーンしたアイテムだったら --*/

		// 取得されたら。
		if (isAcquired && charaPos != nullptr) {

			// 移動すべき角度を求める。
			float moveAngle = atan2f(charaPos->y - pos.y, charaPos->x - pos.x);

			pos += {cosf(moveAngle)* TRACING_SPEED, sinf(moveAngle)* TRACING_SPEED};

		}


		break;
	case StaminaItem::STAMINA_ITEM_ID::CRASH_ITEM:

		/*-- クラッシュ時などに出るアイテムだったら --*/

		// 一定時間動きを止めるタイマーが起動していたら処理を飛ばす。
		if (0 < stopTimer) {

			--stopTimer;
			break;

		}

		// 移動量を0に近づける。
		prevVel = vel;
		vel -= vel / 10.0f;
		if (vel < 0.1f) vel = 0;

		// 移動量を足す。
		pos += forwardVec * vel;

		// 移動量が限りなく0になったら一定時間動きを止めるタイマーを起動する。トリガー判定。
		if (vel <= 0 && 0 < vel) {

			stopTimer = STOP_TIMER;

		}

		// [移動量が0] 且つ [動きを止めるタイマーが起動していなかったら] キャラクターに向かって追尾させる。
		if (vel <= 0 && charaPos != nullptr) {

			// 移動すべき角度を求める。
			moveAngle = atan2f(charaPos->y - pos.y, charaPos->x - pos.x);

			pos += {cosf(moveAngle)* TRACING_SPEED, sinf(moveAngle)* TRACING_SPEED};

		}

		break;
	default:
		break;
	}


}

void StaminaItem::Draw()
{

	/*===== 描画処理 =====*/

	DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(pos), DRAW_RADIUS, itemColor);

}

bool StaminaItem::CheckHit(const Vec2<float>& CharaPos, const float& CharaRadius, CHARA_ID CharaID)
{

	// 回収されていたら。
	if (isAcquired) {

		// 現在保存されているキャラクターのIDと今渡されているIDが同じだったら当たり判定を行う。
		if (charaID == CharaID) {

			// 2点間の距離を求める。
			float distance = (pos - CharaPos).Length();

			// 2点間の距離が半径よりも小さかったら。
			if (distance <= CharaRadius + HIT_RADIUS) {

				return true;

			}

		}

	}
	// 回収されていなかったら。
	else{
	
		
	
	}

	return false;
}
