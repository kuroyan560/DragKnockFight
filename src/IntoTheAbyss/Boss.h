#include "Vec.h"
#include <vector>
#include "Intersected.h"
#include"AreaCollider.h"
#include<array>
#include<memory>

#include"StagingInterFace.h"
using namespace std;

#include"CharacterInterFace.h"

#include"NavigationAI.h"
#include"CharacterAI.h"
#include"PlayerAnimation.h"
#include"BossHandMgr.h"

// プレイヤーと引っ張り合うボスクラス
class Boss : public CharacterInterFace
{
public:
	/*===== メンバ変数 =====*/
	Vec2<float> moveVel;		// 移動量
	Vec2<float> swingInertiaVec;// 振り回しの慣性の移動方向
	float swingInertia;			// 振り回しの慣性
	int afterSwingDelay;		// 振り回しのあとにボスを少し動けない状態にするためのタイマー
	INTERSECTED_LINE prevIntersectedLine;

	//画像
	static const enum AnimHandle
	{
		FRONT,
		BACK,
		DAMAGE,
		SWING,
		ANIMAHANDLE_MAX
	};
	int afterImgageTimer;
	std::shared_ptr<PlayerAnimation> anim;

	//ボスのパターン制御-----------------------
	NavigationAI navigationAi;
	CharacterAI characterAi;
	bool initNaviAiFlag;
	//ボスのパターン制御-----------------------

	float bossGraphRadian;

	//ボスのパラメーター変更
	int bossImGuiHandle;
	int prevStaminaMax;

	float bossCount;
	float countDown;
	Vec2<float>shakeDir;
	Sprite bossGraph;
	bool initShakeFalg;
	Vec2<float>bossScale;

	Vec2<float>pointPos;
	float angle;

public:
	/*===== 定数 =====*/

	//const Vec2<float> SCALE = { 80.0f,80.0f };
	const float OFFSET_VEL = 2.0f;
	const float OFFSET_INERTIA = 30.0f;		// 振り回しの慣性
	const int AFTER_SWING_DELAY = 15;
	const int STRUCK_WINDOW_TIMER = 120.0f;

	const float SWING_DISTANCE_DEADLINE = 200.0f;	//プレイヤーを振り回す距離のデットライン

public:
	/*===== メンバ関数 =====*/

	// コンストラクタ
	Boss();

private:
	// 初期化処理
	void OnInit()override;

	// 更新処理
	void OnUpdate(const std::vector<std::vector<int>> &MapData)override;

	//スウィング中も呼び出される更新処理
	void OnUpdateNoRelatedSwing()override {}

	// 描画処理
	void OnDraw()override;
	void OnDrawUI()override {}

	void OnHitMapChip(const HIT_DIR &Dir)override {}

	void OnBreak()override {}
	void OnBreakFinish()override {}
	void OnSwinged()override {}
	void OnSwingedFinish()override {}
	void OnCrash()override {}
	void OnPartnerCrash()override {}
	virtual void OnPilotLeave()override {}	//パイロットがロボから離れた瞬間
	virtual void OnPilotControl()override {}		//パイロットを動かす処理
	virtual void OnPilotReturn()override {}	//パイロットがロボに戻った瞬間

	void Shot(const Vec2<float> &generatePos, const float &forwardAngle, const float &speed);

	void Shake()
	{
		if (maxShakeAmount <= 25.0f)
		{
			maxShakeAmount += 0.5f;
		}
		// シェイク量を更新する。
		//shakeAmount.x = GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		//shakeAmount.y = GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		shakeAmount.x = KuroFunc::GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		shakeAmount.y = KuroFunc::GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
	};
	float maxShakeAmount;
	Vec2<float>shakeAmount;
public:
	void OnKnockOut()override {};
};