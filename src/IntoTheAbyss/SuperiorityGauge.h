#pragma once
#include"../Common/Singleton.h"
#include<memory>
#include"../Common/Vec.h"
#include"CharacterInterFace.h"

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
	/// ゲージ変動
	/// </summary>
	/// <param name="Team">左右どちらのチームか</param>
	/// <param name="Value">変動する量
	/// </param>
	void AddGauge(const WHICH_TEAM& Team, const float& Value);

	/// <summary>
	/// プレイヤーに関するゲージのデータを入手します
	/// </summary>
	/// <returns>プレイヤーのゲージデータ</returns>
	const GaugeData& GetLeftGaugeData();

	/// <summary>
	/// エネミーに関するゲージのデータを入手します
	/// </summary>
	/// <returns>エネミーのゲージデータ</returns>
	const GaugeData& GetRightGaugeData();

	const bool &IsStacking();

	void Init();
	void Update();
	void Draw();

	void DebugValue(float *ADD_VALUE);
private:
	static float GAUGE_MAX_VALUE;			//ゲージの最大量
	static int STACK_MAX_TIMER;			//何秒間拮抗したら拮抗状態だと判断するか

	GaugeData gaugeData[TEAM_NUM];

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
