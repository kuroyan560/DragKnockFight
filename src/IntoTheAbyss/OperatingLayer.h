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
	/// <summary>
	/// 生成する際、座標と移動量を渡す
	/// </summary>
	/// <param name="POS">対象のキャラクターの座標</param>
	/// <param name="VELOCITY">移動量</param>
	OperateMove(const std::shared_ptr<Vec2<float>> &POS);

	/// <summary>
	/// 実行
	/// </summary>
	void Update(const Vec2<float> &VELOCITY);

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress();

	std::shared_ptr<Vec2<float>>pos;
	Vec2<float> vel;
private:

	Vec2<float>oldPos;

};