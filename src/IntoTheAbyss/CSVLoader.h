#pragma once
#pragma once
#include <string>
#include <vector>
#include "Singleton.h"

using namespace std;

/// <summary>
/// 二次元のベクター配列にCSVの敵データを記録します
/// </summary>
class CSVLoader :public Singleton<CSVLoader>
{

public:

	/// <summary>
	/// CSVを読み込み、二次元の敵ベクター配列に記録します
	/// </summary>
	/// <param name="miniWaveData">二次元の敵ベクター配列</param>
	/// <param name="fName">ファイル名</param>
	void CSVLoad(vector<vector<int>>* csvData, const string& fName);

};
