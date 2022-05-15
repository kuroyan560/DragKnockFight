#include "Vec.h"
#include <vector>
#include "Intersected.h"
#include"AreaCollider.h"
#include"IBossPattern.h"
#include<array>
#include<memory>

#include"StagingInterFace.h"
using namespace std;

#include"CharacterInterFace.h"

#include"NavigationAI.h"
#include"CharacterAI.h"
#include"PlayerAnimation.h"

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
		ANIMAHANDLE_MAX
	};
	int afterImgageTimer;
	std::shared_ptr<PlayerAnimation> anim;

	//ボスのパターン制御-----------------------
	NavigationAI navigationAi;
	CharacterAI characterAi;
	//ボスのパターン制御-----------------------

	//ボスのパラメーター変更
	int bossImGuiHandle;

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
	void OnUpdate(const std::vector<std::vector<int>>& MapData)override;

	//スウィング中も呼び出される更新処理
	void OnUpdateNoRelatedSwing()override {}

	// 描画処理
	void OnDraw()override;
	void OnDrawUI()override {}

	void OnHitMapChip(const HIT_DIR& Dir)override {}

	void OnBreak()override {}
	void OnBreakFinish()override {}
	void OnSwinged()override {}
	void OnSwingedFinish()override {}
	void OnCrash()override {}
	void OnPartnerCrash()override {}
	virtual void OnPilotLeave()override {}	//パイロットがロボから離れた瞬間
	virtual void OnPilotControl()override {}		//パイロットを動かす処理
	virtual void OnPilotReturn()override {}	//パイロットがロボに戻った瞬間

	void Shot(const Vec2<float>& generatePos, const float& forwardAngle, const float& speed);
public:
	void OnKnockOut()override {};
};