#include"../KuroEngine.h"
#include"IStageSelectImage.h"

// ステージ選択画面の矢印
class StageSelectArrow {

private:

	/*===== メンバ変数 =====*/

	LerpData pos;		// 座標など
	LerpData expData;	// 演出用
	float angle;		// 回転させる角度
	int arrowHandle;	// 矢の画像ハンドル
	bool isZoom;		// ズームしているかのフラグ


public:

	/*===== メンバ変数 =====*/

	StageSelectArrow();
	void Init(const Vec2<float>& Pos, const float& Angle);
	void Update();
	void Draw();

	inline void SetZoom(const bool& Zoom) { isZoom = Zoom; }

};