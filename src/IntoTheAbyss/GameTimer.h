#pragma once
#include"../Common/Vec.h"
#include<vector>
#include"../Common/Singleton.h"

/// <summary>
/// ゲームの全体の時間を計算します
/// </summary>
class GameTimer:public Singleton<GameTimer>
{
public:
	GameTimer();

	/// <summary>
	/// 時間の描画座標を初期化します
	/// </summary>
	/// <param name="POS">時間の描画座標</param>
	void Init(const Vec2<float> &POS, int TIME, const Vec2<float> &COUNT_DOWN_START_POS, const Vec2<float> &COUNT_DOWN_END_POS);
	void Finalize();
	void Update();
	void Draw();

	/// <summary>
	/// 時間を計ります
	/// </summary>
	void Start();

	/// <summary>
	/// フレーム単位で時間を渡します
	/// </summary>
	/// <returns>フレーム数</returns>
	int GetFlame();

	/// <summary>
	/// 秒単位で時間を渡します
	/// </summary>
	/// <returns>秒数</returns>
	int GetSeceond();

	/// <summary>
	/// 時間切れを知らせます
	/// </summary>
	/// <returns>true...時間切れ,false...時間切れじゃない</returns>
	bool TimeUpFlag();

	/// <summary>
	/// カウントダウンが終了した判定を渡します
	/// </summary>
	/// <returns>true...カウントダウン終了,false...終了していない</returns>
	bool StartGame();


	bool FinishAllEffect();


	void Debug();

private:
	Vec2<float> timerPos;

	std::vector<int> number;

	int timer;
	float flame;
	bool startFlag;
	bool timeUpFlag;

	std::vector<int> minitueHandle;
	std::vector<int> timeHandle;
	std::vector<int> miriHandle;

	Vec2<float>  initCountDownPos, countDownPos;
	float countDownEndPos;
	Vec2<float> texSize;
	int countDownNum;
	
	bool countDownFlag;
	int countDownHandle[4], finishHandle;

	int finishTimer;

	bool counDownFinishFlag;

	std::vector<int> CountNumber(int TIME);
};

