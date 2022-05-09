#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/CharacterAIData.h"
#include<memory>
/// <summary>
/// 操作層のクラスを纏めた物
/// </summary>

/// <summary>
/// 移動命令を出すクラス
/// </summary>
class OperateMove
{
public:
	OperateMove();

	/// <summary>
	/// 実行
	/// </summary>
	void Update(const Vec2<float> &VELOCITY);

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress();
private:
	bool initFlag;
	Vec2<float>oldPos;

	int stanTimer;
}; 


//ダッシュ
class OperateDash
{
public:
	OperateDash();

	//ダッシュを開始する
	void Init(const Vec2<float> &DASH_SPEED);

	//今のスピードからどれくらいダッシュするか
	const Vec2<float> &Update();
private:
	float rate;
	Vec2<float> dashSpeed;
	float timer;
};