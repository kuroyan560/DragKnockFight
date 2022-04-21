#include "Vec.h"
#include "Singleton.h"
#include <array>

// 残像演出
class AfterImageMgr : public Singleton<AfterImageMgr> {

private:

	struct AfterImage {

		/*===== メンバ変数 =====*/

		Vec2<float> pos;	// 描画座標
		Vec2<float> extRate;// 大きさ
		int srcHandle;		// 使用画像ハンドル
		int destHandle;		// 被せるよう画像ハンドル
		float alpha;		// アルファ値
		float radian;		// 角度
		bool isActive;		// アクティブフラグ

		// コンストラクタ
		AfterImage() :pos({}), srcHandle(0), destHandle(0), alpha(0), extRate({}), radian(0), isActive(false) {};

	};


private:

	/*===== メンバ変数 =====*/

	static const int AFTERIMAGE_COUNT = 50;
	std::array<AfterImage, AFTERIMAGE_COUNT> afterImages;
	int delay;				// 残像を生成する遅延
	const int DELAY = 3;	// 残像を生成する遅延


public:

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& Pos, const Vec2<float>& ExtRate, const float& Radian, const int& SrcHandle, const int& DestHandle);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

};