#pragma once
#pragma once
#include <string>
#include <vector>
#include "Singleton.h"

using namespace std;

/// <summary>
/// �񎟌��̃x�N�^�[�z���CSV�̓G�f�[�^���L�^���܂�
/// </summary>
class CSVLoader :public Singleton<CSVLoader>
{

public:

	/// <summary>
	/// CSV��ǂݍ��݁A�񎟌��̓G�x�N�^�[�z��ɋL�^���܂�
	/// </summary>
	/// <param name="miniWaveData">�񎟌��̓G�x�N�^�[�z��</param>
	/// <param name="fName">�t�@�C����</param>
	void CSVLoad(vector<vector<int>>* csvData, const string& fName);

};
