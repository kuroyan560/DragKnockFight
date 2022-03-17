#include "CSVLoader.h"
#include <assert.h>
#include <fstream>
#include <iostream>

void CSVLoader::CSVLoad(vector<vector<int>>* csvData, const string& fName)
{

	// ������nullptr��������G���[���o��
	if (csvData == nullptr) assert(0);

	// �t�@�C���f�[�^
	ifstream ifs;

	// �t�@�C�����J���B
	ifs.open(fName.c_str());

	// �t�@�C�����J���������`�F�b�N����B
	if (ifs.fail())
	{
		//���s
		//assert(0);
		return;
	}

	// 1�s�̃f�[�^���擾���邽�߂̕ϐ��B
	string line = {};

	const int offset = 48;

	// �t�@�C���̏I�_�܂Ń��[�v����B
	while (getline(ifs, line))
	{

		// 1�s�̓GWave�f�[�^���ꎞ�ۊǂ��Ă������߂̃R���e�i�B
		vector<int> result;

		// ������ۑ����Ă����ϐ��B
		int disitBuff = 0;

		// �ǂݍ��񂾐�����G�f�[�^�ɕϊ�����B
		for (int index = 0; index < line.length(); index++)
		{

			// �ǂݍ��񂾕�����0�������珈�����΂�
			if (line[index] == ',') {

				// �ꎞ�ۑ��p�R���e�i�Ƀv�b�V���o�b�N
				result.push_back(disitBuff);
				// ������
				disitBuff = 0;

				continue;
			}

			// disitBuff��10��������B(�O�̗v�f�Œl�������Ă���΁A�O�̃t���[���̒l��10��������B)
			disitBuff *= 10.0f;

			// �ǂݍ���1�s�̕�����̎w��̃C���f�b�N�X�̕������擾����B
			disitBuff += line[index] - offset;

		}

		// �ꎞ�ۑ��p�R���e�i�Ƀv�b�V���o�b�N
		result.push_back(disitBuff);

		csvData->push_back(result);

	}

}
