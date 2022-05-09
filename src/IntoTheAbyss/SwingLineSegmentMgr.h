#include "Vec.h"
#include <array>
#include <vector>
#include"CharacterInfo.h"

// 振り回し領域可視化のための線分単体
class SwingLineSegment {

private:

	/*===== メンバ変数 =====*/

	Vec2<float> start;		// 線分開始地点
	Vec2<float> end;		// 線分終了地点
	Vec2<float> startDir;	// 中心からのベクトル
	Vec2<float> endDir;		// 中心からのベクトル
	int alpha;				// アルファ値
	int graphHandle;		// 使用する画像
	bool isActive;			// 有効化フラグ


public:

	enum class SEGMENT_ID {

		SEGMENT_ID_LINE,	// 通常の線
		SEGMENT_ID_UI,		// 操作説明等
		SEGMENT_ID_ARROW,	// 矢印

	};

private:

	SEGMENT_ID id;


public:

	// コンストラクタ
	SwingLineSegment() :start({}), end({}), startDir({}), endDir({}), alpha(0), graphHandle(0), isActive(false) {};

	// 初期化処理
	void Init();

	// 更新処理
	void Update(const Vec2<float>& Start, const Vec2<float>& End, const Vec2<float>& StartDir, const Vec2<float>& EndDir, const int& Alpha, const SEGMENT_ID& ID, const int& Handle);

	// 描画処理
	void Draw(const WHICH_TEAM& Team);

	// 距離修正 スイング中は線分の更新を止めるが、スイング中に紐の長さが変わってしまう場合があるのでそれに対応させるために追加。
	void ResetDistance(const Vec2<float>& Pos, const float& Distance);

	// 頂点を移動させる。
	void MoveVertex(const Vec2<float>& Move);

	inline void SetAlpha(const int& Alpha) { alpha = Alpha; }
	inline void Deactivate() { isActive = false; }
	inline const Vec2<float>& GetStart() { return start; }
	inline const Vec2<float>& GetEnd() { return end; }
	inline const bool& GetIsActive() { return isActive; }
	inline const SEGMENT_ID& GetID() { return id; }
	inline void SetEnd(const Vec2<float>& End) { end = End; }
	inline void SetID(const SEGMENT_ID& ID) { id = ID; }
	inline void SetHandle(const int& Handle) { graphHandle = Handle; }

};


// 線分を管理するクラス
class SwingLineSegmentMgr {

private:

	/*===== メンバ変数 =====*/

	static const int LINE_COUNT = 9;	// 線分の数
	std::array<SwingLineSegment, LINE_COUNT> lineSegments;	// 線分
	Vec2<float> reticlePos;	// 照準座標
	Vec2<float> swingStartPos;	// 振り回し時の振り回されているキャラの開始座標
	Vec2<float> swingCharaPos;	// スイングしている時の予測線を動かすための前フレーム座標

	int UIHandle;		// ボタンのUIの画像
	int arrowHandle;	// 矢印画像
	int lineHandle;		// 線の画像
	int reticleHandle;	// 照準画像

	bool isClockWise;	// 時計回りか？

	const float ANGLE_DIFFERENCE = 0.349066f;	// 20°

	bool isHitWallFlag;//振り回した先が壁に当たるかどうか
public:

	/*===== メンバ関数 =====*/

	// セッティング処理
	void Setting(const bool& IsClockWise, const int& HandleUI, const int& HandleArrow, const int& HandleLine, const int& ReticleHandle);

	// 初期化処理
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="Pos"> キャラ座標 </param>
	/// <param name="TargetVec"> 振り回し対象へのベクトル </param>
	/// <param name="Distance"> 振り回し対象までの距離 </param>
	/// <param name="IsSwing"> 振り回しをしているか </param>
	/// <param name="NoMove"> 線分の更新をしないか </param>
	void Update(const Vec2<float>& Pos, const Vec2<float>& TargetVec, const float& Distance, const std::vector<std::vector<int>>& MapData);

	void SetCharaStartPos(const Vec2<float>& CharaPos);	// 振り回し開始時に振り回すキャラの座標を保存する関数を呼ぶ。
	void UpdateSwing(const Vec2<float>& CharaPos);		// 振り回し中に呼ぶ更新処理

	// 描画処理
	void Draw(const WHICH_TEAM& Team);

	/// <summary>
	/// 振り回した先が壁に当たるかどうか
	/// </summary>
	/// <returns>true...当たる,false...当たらない</returns>
	bool IsHitWall();

	// 振り回しの開始座標を設定。 引数はパートナーの座標。
	void SetSwingStartPos(const Vec2<float>& StartPos) { swingStartPos = StartPos; }

	// 振り回しの開始座標から終了座標までの直線距離を計算。
	// CharaPosは振り回している側の座標。SwingTargetVecはCharacterInterfaceのメンバ変数にあるやつ。Distanceはキャラ間の距離。
	// 振り回し中にのみ呼んでください！
	float CalSwingEndDistance(const Vec2<float>& CharaPos, const Vec2<float>& SwingTargetVec, const float& Distance);

private:

	// 線分とマップチップの交差
	const Vec2<float>& SwingLineSegmentMgr::CheckHitMapChip(const Vec2<float>& StartPos, const Vec2<float>& EndPos);

};