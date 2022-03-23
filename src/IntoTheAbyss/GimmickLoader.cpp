#include "GimmickLoader.h"
#include"KuroEngine.h"

GimmickLoader::GimmickLoader()
{
	int stageNum = 4;
	allGimmickData.reserve(stageNum);
	allGimmickData.resize(stageNum);
	allBubbleData.reserve(stageNum);
	allBubbleData.resize(stageNum);

	//�g�p�ł���M�~�b�N���̏�����
	gimmickName[GIMMCK_NAME_THOWMPE] = "Thwomp";

	//�g�p�ł���S�M�~�b�N���ʂ�Key�l�̕����̏�����
	gimmickCommonName[GIMMCK_COMMON_DATA_NAME] = "name";

	//����̃M�~�b�N��Key�l�̕����̏�����
	gimmickThowmpeDataName[GIMMCK_THOWMPE_DATA_TYPE] = "type";
	gimmickThowmpeDataName[GIMMCK_THOWMPE_DATA_SIZE] = "size";

	//����̃M�~�b�N�̎�ނŎg���镶���̏�����
	gimmickThowmpeType[GIMMCK_THOWMPE_TYPE_OFF] = "off";;
	gimmickThowmpeType[GIMMCK_THOWMPE_TYPE_ON_LOW] = "onLow";
	gimmickThowmpeType[GIMMCK_THOWMPE_TYPE_ON_HIGH] = "onHigh";
};

void GimmickLoader::LoadData(const int& STAGE_NUM, const int& ROOM_NUM, const std::string& FILE_PASS)
{
	std::vector<std::shared_ptr<ThownpeData>> gimmickData;
	int gimmickArrayNum = 0;



	allGimmickData[STAGE_NUM].push_back(std::vector<std::shared_ptr<ThownpeData>>());
	allBubbleData[STAGE_NUM].push_back({});


	// �t�@�C���f�[�^
	std::ifstream ifs;
	// �t�@�C�����J���B
	ifs.open(FILE_PASS);
	// �t�@�C�����J���������`�F�b�N����B
	if (ifs.fail())
	{
		return;
	}

	//�t�@�C���������I��
	std::string line;
	//�M�~�b�N�f�[�^��ǉ�����

	GimmickName eGimmickName = GIMMCK_NAME_NONE;

	while (getline(ifs, line))
	{
		//��s���̕�������X�g���[���ɕϊ����ĉ�͂��₷���Ȃ�
		std::istringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;//������v��f���̔��f������
		getline(line_stream, key, ' ');



		//Key�l�G���[-----------------------
		bool keyErrorFlag = true;	//�G���[�T�m�p�̃t���O
		//���ʂ�KEY�l��T��
		for (int keyIndex = 0; keyIndex < gimmickCommonName.size(); ++keyIndex)
		{
			if (gimmickCommonName[keyIndex] == key)
			{
				keyErrorFlag = false;
			}
		}
		//�����KEY�l��T��
		for (int keyIndex = 0; keyIndex < gimmickThowmpeDataName.size(); ++keyIndex)
		{
			if (gimmickThowmpeDataName[keyIndex] == key)
			{
				keyErrorFlag = false;
			}
		}
		//�󔒂̓G���[���o���Ȃ�
		if (key == "")
		{
			keyErrorFlag = false;
		}
		//�G���[����
		if (keyErrorFlag)
		{
			std::string errorName = key + "�͎g�������o���Ȃ�Key�l�ł��B\n�뎚�������Ă��Ȃ����m�F���肢���܂��B";
			MessageBox(NULL, KuroFunc::GetWideStrFromStr(errorName).c_str(), TEXT("Key�l�g�p�s��"), MB_OK);
			assert(0);
		}
		//Key�l�G���[-----------------------


		//key��name�Ȃ�M�~�b�N��������
		if (key == gimmickCommonName[GIMMCK_COMMON_DATA_NAME])
		{
			gimmickData.push_back(std::make_shared<ThownpeData>());
			gimmickArrayNum = gimmickData.size() - 1;

			//���O���m�F����
			std::string name;
			line_stream >> name;


			bool errorFlag = true;
			int nameIndex = -1;		//�Y��������enum�ɕϊ�����ׂɎg���ϐ�
			for (int i = 0; i < allGimmickData[STAGE_NUM].size(); ++i)
			{
				//�ǂݍ��񂾃M�~�b�N�����o�^����Ă���M�~�b�N���Ȃ̂��ǂ������f����
				if (gimmickName[i] == name)
				{
					errorFlag = false;
					nameIndex = i;
					break;
				}
			}
			//�G���[����
			if (errorFlag)
			{
				std::string errorName = name + "�͎g�������o���Ȃ��M�~�b�N���ł��B\n�뎚�������Ă��Ȃ����m�F���肢���܂��B";
				MessageBox(NULL, KuroFunc::GetWideStrFromStr(errorName).c_str(), TEXT("�M�~�b�N�g�p�s��"), MB_OK);
				assert(0);
			}
			//�o�^���ꂽ�M�~�b�N������ϊ�����
			eGimmickName = static_cast<GimmickName>(nameIndex);
		}


		//�o�^���ꂽ�M�~�b�N������ǂ̏�����ʂ������߂�
		switch (eGimmickName)
		{
		case GIMMCK_NAME_NONE:
			break;
		case GIMMCK_NAME_THOWMPE:
			LoadThowmpeData(key, &line_stream, gimmickData[gimmickArrayNum]);
			break;
		case GIMMCK_NAME_MAX:
			break;
		default:
			break;
		}
		allGimmickData[STAGE_NUM][ROOM_NUM] = gimmickData;
	}

	bool debug = false;
	
}

std::vector<std::shared_ptr<BubbleData>> GimmickLoader::GetBubbleData(const int& STAGE_NUM, const int& ROOM_NUM)
{
	//�X�e�[�W�ԍ����z��͈̔͊O�Ȃ�Ԃ��Ȃ�
	if (STAGE_NUM < 0 || allBubbleData.size() <= STAGE_NUM)
	{
		return std::vector<std::shared_ptr<BubbleData>>();
	}
	//�����ԍ����z��͈̔͊O�Ȃ�Ԃ��Ȃ�
	if (ROOM_NUM < 0 || allBubbleData[STAGE_NUM].size() <= ROOM_NUM)
	{
		return std::vector<std::shared_ptr<BubbleData>>();
	}
	//�w�肵���ꏊ��nullptr�Ȃ�Ԃ��Ȃ�
	if (allBubbleData[STAGE_NUM][ROOM_NUM].size() == 0)
	{
		return std::vector<std::shared_ptr<BubbleData>>();
	}
	return allBubbleData[STAGE_NUM][ROOM_NUM];
}

std::vector<std::shared_ptr<ThownpeData>> GimmickLoader::GetThowpeData(const int& STAGE_NUM, const int& ROOM_NUM)
{
	//�X�e�[�W�ԍ����z��͈̔͊O�Ȃ�Ԃ��Ȃ�
	if (STAGE_NUM < 0 || allGimmickData.size() < STAGE_NUM)
	{
		return std::vector<std::shared_ptr<ThownpeData>>();
	}
	//�����ԍ����z��͈̔͊O�Ȃ�Ԃ��Ȃ�
	if (ROOM_NUM < 0 || allGimmickData[STAGE_NUM].size() < ROOM_NUM)
	{
		return std::vector<std::shared_ptr<ThownpeData>>();
	}
	//�w�肵���ꏊ��nullptr�Ȃ�Ԃ��Ȃ�
	if (allGimmickData[STAGE_NUM][ROOM_NUM].size() == 0)
	{
		return std::vector<std::shared_ptr<ThownpeData>>();
	}
	return allGimmickData[STAGE_NUM][ROOM_NUM];
}

void GimmickLoader::SetThwompStartPos(const int& STAGE_NUM, const int& ROOM_NUM, const int& GIMMICK_NUMBER, const Vec2<float>& POS)
{
	allGimmickData[STAGE_NUM][ROOM_NUM][GIMMICK_NUMBER]->startPos = POS;
}

void GimmickLoader::SetThwompEndPos(const int& STAGE_NUM, const int& ROOM_NUM, const int& GIMMICK_NUMBER, const Vec2<float>& POS)
{
	allGimmickData[STAGE_NUM][ROOM_NUM][GIMMICK_NUMBER]->endPos = POS;
}

void GimmickLoader::PushBubbleData(const int& STAGE_NUM, const int& ROOM_NUM, const int& GIMMICK_NUMBER, const Vec2<float>& POS)
{
	allBubbleData[STAGE_NUM][ROOM_NUM].push_back(std::make_shared<BubbleData>());
	allBubbleData[STAGE_NUM][ROOM_NUM][GIMMICK_NUMBER]->pos = POS;
}

const bool &GimmickLoader::CanLoadData(const int &STAGE_NUM, const int &ROOM_NUM, const int &GIMMICK_NUMBER)
{
	//�X�e�[�W�ԍ����z��͈̔͊O�Ȃ�false
	if (STAGE_NUM < 0 || allGimmickData.size() < STAGE_NUM)
	{
		return false;
	}
	//�����ԍ����z��͈̔͊O�Ȃ�false
	if (ROOM_NUM < 0 || allGimmickData[STAGE_NUM].size() < ROOM_NUM)
	{
		return false;
	}
	//�w�肵���ꏊ��nullptr�Ȃ�false
	if (GIMMICK_NUMBER < 0 || allGimmickData[STAGE_NUM][ROOM_NUM].size() < GIMMICK_NUMBER)
	{
		return false;
	}

	return true;
}

void GimmickLoader::LoadThowmpeData(const std::string& KEY, std::istringstream* LINE_STREAM, std::shared_ptr<ThownpeData> THOWNPE_DATA)
{
	//�M�~�b�N�̎�ނ𔻕ʂ���
	if (KEY == gimmickThowmpeDataName[GIMMCK_THOWMPE_DATA_TYPE])
	{
		std::string name;
		*LINE_STREAM >> name;

		if (name == gimmickThowmpeType[GIMMCK_THOWMPE_TYPE_OFF])
		{
			THOWNPE_DATA->type = GIMMICK_DOSSN_OFF;
		}
		else if (name == gimmickThowmpeType[GIMMCK_THOWMPE_TYPE_ON_LOW])
		{
			THOWNPE_DATA->type = GIMMICK_DOSSN_ON_LOW;
		}
		else if (name == gimmickThowmpeType[GIMMCK_THOWMPE_TYPE_ON_HIGH])
		{
			THOWNPE_DATA->type = GIMMICK_DOSSN_ON_HIGH;
		}
		else
		{
			std::string errorName = name + "�͎g�������o���Ȃ���ނł��B\n�뎚�������Ă��Ȃ����m�F���肢���܂��B";
			MessageBox(NULL, KuroFunc::GetWideStrFromStr(errorName).c_str(), TEXT("type�g�p�s��"), MB_OK);
			assert(0);
		}
	}

	//�M�~�b�N�̃T�C�Y�𔻕ʂ���
	if (KEY == gimmickThowmpeDataName[GIMMCK_THOWMPE_DATA_SIZE])
	{
		Vec2<float> sizeData;
		*LINE_STREAM >> sizeData.x;
		*LINE_STREAM >> sizeData.y;
		THOWNPE_DATA->size = sizeData;
	}
}
