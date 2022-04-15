#pragma once
#include"../Common/Singleton.h"
#include<memory>
#include"../Common/Vec.h"

/// <summary>
/// 優勢ゲージ
/// </summary>
class SuperiorityGauge:public Singleton<SuperiorityGauge>
{
private:
	//ゲージに関するデータ
	struct GaugeData
	{
		float gaugeValue;		//ゲージの値
		float gaugeDivValue;	//ゲージの割合
		bool prevOverGaugeFlag;	//1F前の振り切った判定
		bool overGaugeFlag;		//振り切った判定

		GaugeData() {}
		void Init(const float& GaugeMaxValue)
		{
			gaugeValue = GaugeMaxValue / 2.0f;
			gaugeDivValue = 0.5f;
			overGaugeFlag = false;
			prevOverGaugeFlag = false;
		}
	};

public:
	SuperiorityGauge();

	/// <summary>
	/// プレイヤーにゲージ量を追加し、追加した分エネミーのゲージ量を減らします
	/// </summary>
	/// <param name="VALUE">追加する値</param>
	void AddPlayerGauge(const float &VALUE);

	/// <summary>
	/// エネミーにゲージ量を追加し、追加した分プレイヤーのゲージ量を減らします
	/// </summary>
	/// <param name="VALUE"></param>
	void AddEnemyGauge(const float &VALUE);

	/// <summary>
	/// プレイヤーに関するゲージのデータを入手します
	/// </summary>
	/// <returns>プレイヤーのゲージデータ</returns>
	const std::unique_ptr<GaugeData> &GetPlayerGaugeData();

	/// <summary>
	/// エネミーに関するゲージのデータを入手します
	/// </summary>
	/// <returns>エネミーのゲージデータ</returns>
	const std::unique_ptr<GaugeData> &GetEnemyGaugeData();

	const bool &IsStacking();

	void Init();
	void Update();
	void Draw();

	void DebugValue(float *ADD_VALUE);
private:
	static float GAUGE_MAX_VALUE;			//ゲージの最大量
	static int STACK_MAX_TIMER;			//何秒間拮抗したら拮抗状態だと判断するか

	std::unique_ptr<GaugeData> playerGaugeData;	//プレイヤーのゲージデータ
	std::unique_ptr<GaugeData> enemyGaugeData;	//敵のゲージデータ

	bool isStackingFlag;	//拮抗しているかどうか
	int stackTimer;			//何秒拮抗しているか

	//描画用
	int gaugeGraphHandle;
	int gaugeVarGraphPlayer;
	int gaugeVarGraphEnemy;

	void LimitGauge();
	//デバック用
	float oldMaxGauge;

	int imguiHandle;
};
