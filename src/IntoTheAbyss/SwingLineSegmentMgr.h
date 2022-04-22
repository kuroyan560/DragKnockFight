#include "Vec.h"
#include <array>
#include <vector>

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
	void Draw();

	// 距離修正 スイング中は線分の更新を止めるが、スイング中に紐の長さが変わってしまう場合があるのでそれに対応させるために追加。
	void ResetDistance(const Vec2<float>& Pos, const float& Distance);

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

	int UIHandle;		// ボタンのUIの画像
	int arrowHandle;	// 矢印画像
	int lineHandle;		// 線の画像
	int reticleHandle;	// 照準画像

	bool isClockWise;	// 時計回りか？

	const float ANGLE_DIFFERENCE = 0.349066f;	// 20°

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
	void Update(const Vec2<float>& Pos, const Vec2<float>& TargetVec, const float& Distance, const bool& IsSwing, const bool& NoMove, const std::vector<std::vector<int>>& mapData);

	// 描画処理
	void Draw();

private:

	// 線分とマップチップの交差
	const Vec2<float>& SwingLineSegmentMgr::CheckHitMapChip(const Vec2<float>& StartPos, const Vec2<float>& EndPos);

};