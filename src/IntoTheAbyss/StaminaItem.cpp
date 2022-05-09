#include "StaminaItem.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include"TexHandleMgr.h"
#include "BulletCollision.h"

int StaminaItem::GRAPH_HANDLE[STAR_COLOR::GRAPH_NUM] = { -1 };

StaminaItem::StaminaItem()
{
	if (GRAPH_HANDLE[0] == -1)
	{
		TexHandleMgr::LoadDivGraph("resource/ChainCombat/background_star.png", 4, { 4,1 }, GRAPH_HANDLE);
	}

	/*===== 初期化 =====*/

	isActive = false;

	collisionData = std::make_shared<SphereCollision>();
	collisionData->center = &pos;
	collisionData->radius = HIT_RADIUS;

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
	graph = GRAPH_HANDLE[STAR_COLOR::NONE];
	isActive = true;

	// 初めから取得されている状態だったら参照を保存する。
	if (isAcquired && CharaPos != nullptr) {

		charaPos = CharaPos;

		if (CharaID == CHARA_ID::LEFT)
		{
			graph = GRAPH_HANDLE[STAR_COLOR::LEFT];
		}
		else
		{
			graph = GRAPH_HANDLE[STAR_COLOR::RIGHT];
		}
	}

	flashRad = Angle(KuroFunc::GetRand(0, 360));
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

		// 取得されていなかったら点滅
		if (!isAcquired)
		{
			flashRad += Angle::ConvertToRadian(1.0f);
			if (Angle(180) < flashRad)flashRad = 0.0f;
			break;
		}

		// 移動量を0に近づける。
		vel -= vel / 10.0f;
		if (vel < 1.0f) vel = 0;

		// 移動量を足す。
		pos += forwardVec * vel;

		// [移動量が0] 且つ [動きを止めるタイマーが起動していなかったら] キャラクターに向かって追尾させる。
		if (vel <= 0 && charaPos != nullptr) {

			// 移動すべき角度を求める。
			moveAngle = atan2f(charaPos->y - pos.y, charaPos->x - pos.x);

			pos += {cosf(moveAngle)* TRACING_SPEED, sinf(moveAngle)* TRACING_SPEED};

		}


		break;
	case StaminaItem::STAMINA_ITEM_ID::CRASH_ITEM:

		/*-- クラッシュ時などに出るアイテムだったら --*/

		// 移動量を0に近づける。
		vel -= vel / 10.0f;
		if (vel < 0.5f) vel = 0;

		// 移動量を足す。
		pos += forwardVec * vel;

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

	//DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(pos), DRAW_RADIUS * Camera::Instance()->zoom, itemColor, isAcquired);
	const float extDiffer = isAcquired ? 1.0f : 0.75f;
	const Vec2<float>extRate = { Camera::Instance()->zoom * extDiffer,Camera::Instance()->zoom * extDiffer };
	static const float ALPHA_MIN = 0.6f;
	const float alpha = isAcquired ? 1.0f : (sin(flashRad) * (1.0f - ALPHA_MIN) + ALPHA_MIN);
	DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), extRate, 0.0f, TexHandleMgr::GetTexBuffer(graph), Color(1.0f, 1.0f, 1.0f, alpha));

}

bool StaminaItem::CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, const float& PilotRadius, CHARA_ID CharaID, const Vec2<float>* PilotPos)
{
	if (CharaPos == nullptr && PilotPos) return false;

	// 移動中だったら当たり判定を行わない。
	if (0 < vel) return false;

	// 回収されていたら。
	if (isAcquired) {

		// 現在保存されているキャラクターのIDと今渡されているIDが同じだったら当たり判定を行う。
		if (charaID == CharaID) {

			// 2点間の距離を求める。
			float distance = (pos - *CharaPos).Length();

			// 2点間の距離が半径よりも小さかったら。
			if (distance <= CharaRadius) {

				return true;

			}

			//パイロットとの判定
			if (PilotPos != nullptr)
			{
				distance = PilotPos->Distance(pos);

				if (distance <= PilotRadius)return true;
			}
		}

	}
	// 回収されていなかったら。
	else {

		// 2点間の距離を求める。
		float distance = (pos - *CharaPos).Length();

		// 2点間の距離が半径よりも小さかったら。
		if (distance <= CharaRadius + HIT_RADIUS) {

			return true;

		}

		//パイロットとの判定
		if (PilotPos != nullptr)
		{
			distance = PilotPos->Distance(pos);

			if (distance <= PilotRadius)return true;
		}
	}

	return false;
}

void StaminaItem::Acquire(Vec2<float>* CharaPos, CHARA_ID CharaID)
{

	/*===== 取得された状態にする =====*/

	isAcquired = true;
	charaPos = CharaPos;
	forwardVec = pos - *CharaPos;
	forwardVec.Normalize();
	vel = ACQUIRED_VEL;
	charaID = CharaID;
	if (CharaID == CHARA_ID::LEFT)
	{
		graph = GRAPH_HANDLE[STAR_COLOR::LEFT];
	}
	else
	{
		graph = GRAPH_HANDLE[STAR_COLOR::RIGHT];
	}

}
