#include "CSVLoader.h"
#include <assert.h>
#include <fstream>
#include <iostream>

void CSVLoader::CSVLoad(vector<vector<int>>* csvData, const string& fName)
{

	// 引数がnullptrだったらエラーを出す
	if (csvData == nullptr) assert(0);

	// ファイルデータ
	ifstream ifs;

	// ファイルを開く。
	ifs.open(fName.c_str());

	// ファイルが開けたかをチェックする。
	if (ifs.fail())
	{
		//失敗
		//assert(0);
		return;
	}

	// 1行のデータを取得するための変数。
	string line = {};

	const int offset = 48;

	// ファイルの終点までループする。
	while (getline(ifs, line))
	{

		// 1行の敵Waveデータを一時保管しておくためのコンテナ。
		vector<int> result;

		// 数字を保存しておく変数。
		int disitBuff = 0;

		// 読み込んだ数字を敵データに変換する。
		for (int index = 0; index < line.length(); index++)
		{

			// 読み込んだ文字列が0だったら処理を飛ばす
			if (line[index] == ',') {

				// 一時保存用コンテナにプッシュバック
				result.push_back(disitBuff);
				// 初期化
				disitBuff = 0;

				continue;
			}

			// disitBuffに10をかける。(前の要素で値が入っていれば、前のフレームの値に10をかける。)
			disitBuff *= 10.0f;

			// 読み込んだ1行の文字列の指定のインデックスの文字を取得する。
			disitBuff += line[index] - offset;

		}

		// 一時保存用コンテナにプッシュバック
		result.push_back(disitBuff);

		csvData->push_back(result);

	}

}
