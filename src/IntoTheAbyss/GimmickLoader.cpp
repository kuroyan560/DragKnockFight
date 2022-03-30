#include "GimmickLoader.h"
#include"KuroEngine.h"

GimmickLoader::GimmickLoader()
{
	int stageNum = 4;
	allThowmpeData.reserve(stageNum);
	allThowmpeData.resize(stageNum);
	allBubbleData.reserve(stageNum);
	allBubbleData.resize(stageNum);

	errorStrings.resize(stageNum);


	//�g�p�ł���M�~�b�N���̏�����
	gimmickName[GIMMCK_NAME_THOWMPE] = "Thwomp";
	gimmickName[GIMMCK_NAME_THOWN] = "Thown";

	//�g�p�ł���S�M�~�b�N���ʂ�Key�l�̕����̏�����
	gimmickCommonName[GIMMCK_COMMON_DATA_NAME] = "name";

	//����̃M�~�b�N��Key�l�̕����̏�����
	gimmickThowmpeDataName[GIMMCK_THOWMPE_DATA_TYPE] = "type";
	gimmickThowmpeDataName[GIMMCK_THOWMPE_DATA_SIZE] = "size";

	//����̃M�~�b�N�̎�ނŎg���镶���̏�����
	gimmickThowmpeType[GIMMCK_THOWMPE_TYPE_OFF] = "off";
	gimmickThowmpeType[GIMMCK_THOWMPE_TYPE_ON_LOW] = "onLow";
	gimmickThowmpeType[GIMMCK_THOWMPE_TYPE_ON_HIGH] = "onHigh";

	thownData = std::make_shared<ThownData>();

	oldStageNum = -1;
	oldRoomNum = -1;
};

void GimmickLoader::LoadData(const int &STAGE_NUM, const int &ROOM_NUM, const std::string &FILE_PASS)
{
	std::vector<std::shared_ptr<ThownpeData>> gimmickData;



	allThowmpeData[STAGE_NUM].push_back(std::vector<std::shared_ptr<ThownpeData>>());
	allBubbleData[STAGE_NUM].push_back({});


	errorStrings[STAGE_NUM].push_back({});

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
		if (key == "hitBox")
		{
			keyErrorFlag = false;
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
		GimmickName get = CheckName(key, &line_stream);
		if (get != -1 || eGimmickName == -1)
		{
			switch (get)
			{
			case GimmickLoader::GIMMCK_NAME_NONE:
				break;
			case GimmickLoader::GIMMCK_NAME_THOWMPE:
				gimmickData.push_back(std::make_unique<ThownpeData>());
				break;
			case GimmickLoader::GIMMCK_NAME_THOWN:
				break;
			case GimmickLoader::GIMMCK_NAME_MAX:
				break;
			default:
				break;
			}
			eGimmickName = get;
		}

		int gimmickArrayNum = gimmickData.size() - 1;
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

		allThowmpeData[STAGE_NUM][ROOM_NUM] = gimmickData;
	}
}

void GimmickLoader::LoadData(const std::string &FILE_PASS)
{
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
		if (key == "hitBox")
		{
			keyErrorFlag = false;
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
		GimmickName get = CheckName(key, &line_stream);
		if (get != -1 || eGimmickName == -1)
		{
			eGimmickName = get;
		}


		//�o�^���ꂽ�M�~�b�N������ǂ̏�����ʂ������߂�
		switch (eGimmickName)
		{
		case GIMMCK_NAME_NONE:
			break;

		case GIMMCK_NAME_THOWN:
			LoadThownData(key, &line_stream, thownData);
			break;

		case GIMMCK_NAME_MAX:
			break;
		default:
			break;
		}
	}
}

std::vector<std::shared_ptr<BubbleData>> GimmickLoader::GetBubbleData(const int &STAGE_NUM, const int &ROOM_NUM)
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

std::vector<std::shared_ptr<ThownpeData>> GimmickLoader::GetThowpeData(const int &STAGE_NUM, const int &ROOM_NUM)
{
	//�X�e�[�W�ԍ����z��͈̔͊O�Ȃ�Ԃ��Ȃ�
	if (STAGE_NUM < 0 || allThowmpeData.size() < STAGE_NUM)
	{
		return std::vector<std::shared_ptr<ThownpeData>>();
	}
	//�����ԍ����z��͈̔͊O�Ȃ�Ԃ��Ȃ�
	if (ROOM_NUM < 0 || allThowmpeData[STAGE_NUM].size() < ROOM_NUM)
	{
		return std::vector<std::shared_ptr<ThownpeData>>();
	}
	//�w�肵���ꏊ��nullptr�Ȃ�Ԃ��Ȃ�
	if (allThowmpeData[STAGE_NUM][ROOM_NUM].size() == 0)
	{
		return std::vector<std::shared_ptr<ThownpeData>>();
	}
	return allThowmpeData[STAGE_NUM][ROOM_NUM];
}

void GimmickLoader::SetThwompStartPos(const int &STAGE_NUM, const int &ROOM_NUM, const int &GIMMICK_NUMBER, const Vec2<float> &POS)
{
	bool nullCheckFlag = allThowmpeData[STAGE_NUM][ROOM_NUM].size() != 0 && GIMMICK_NUMBER <= allThowmpeData[STAGE_NUM][ROOM_NUM].size();

	if (nullCheckFlag)
	{
		allThowmpeData[STAGE_NUM][ROOM_NUM][GIMMICK_NUMBER]->startPos = POS;
	}
	else
	{
		for (int i = 0; i < errorCountData.size(); ++i)
		{
			if (errorCountData[i] == GIMMICK_NUMBER)
			{
				return;
			}
		}
		errorCountData.push_back(GIMMICK_NUMBER);

		const std::string gimmickLoadErrorString = gimmickName[GIMMCK_NAME_THOWMPE] + " " + std::to_string(GIMMICK_NUMBER) + " �M�~�b�N�t�@�C���ɏ�񂪏�����Ă��܂���";
		AddErrorStirng(STAGE_NUM, ROOM_NUM, gimmickLoadErrorString);
	}
}

void GimmickLoader::SetThwompEndPos(const int &STAGE_NUM, const int &ROOM_NUM, const int &GIMMICK_NUMBER, const Vec2<float> &POS)
{
	bool nullCheckFlag = allThowmpeData[STAGE_NUM][ROOM_NUM].size() != 0 && GIMMICK_NUMBER <= allThowmpeData[STAGE_NUM][ROOM_NUM].size();

	if (nullCheckFlag)
	{
		allThowmpeData[STAGE_NUM][ROOM_NUM][GIMMICK_NUMBER]->endPos = POS;
	}
	else
	{
		for (int i = 0; i < errorCountData.size(); ++i)
		{
			if (errorCountData[i] == GIMMICK_NUMBER)
			{
				return;
			}
		}
		errorCountData.push_back(GIMMICK_NUMBER);

		const std::string gimmickLoadErrorString = gimmickName[GIMMCK_NAME_THOWMPE] + " " + std::to_string(GIMMICK_NUMBER) + " �M�~�b�N�t�@�C���ɏ�񂪏�����Ă��܂���";
		AddErrorStirng(STAGE_NUM, ROOM_NUM, gimmickLoadErrorString);
	}
}

void GimmickLoader::PushBubbleData(const int &STAGE_NUM, const int &ROOM_NUM, const int &GIMMICK_NUMBER, const Vec2<float> &POS)
{
	allBubbleData[STAGE_NUM][ROOM_NUM].push_back(std::make_shared<BubbleData>());
	allBubbleData[STAGE_NUM][ROOM_NUM][GIMMICK_NUMBER]->pos = POS;
}

const bool &GimmickLoader::CanLoadData(const int &STAGE_NUM, const int &ROOM_NUM, const int &GIMMICK_NUMBER)
{
	//�X�e�[�W�ԍ����z��͈̔͊O�Ȃ�false
	if (STAGE_NUM < 0 || allThowmpeData.size() < STAGE_NUM)
	{
		return false;
	}
	//�����ԍ����z��͈̔͊O�Ȃ�false
	if (ROOM_NUM < 0 || allThowmpeData[STAGE_NUM].size() < ROOM_NUM)
	{
		return false;
	}
	//�w�肵���ꏊ��nullptr�Ȃ�false
	if (GIMMICK_NUMBER < 0 || allThowmpeData[STAGE_NUM][ROOM_NUM].size() < GIMMICK_NUMBER)
	{
		return false;
	}

	return true;
}

void GimmickLoader::ErrorCheck()
{
	for (int stageNum = 0; stageNum < allThowmpeData.size(); ++stageNum)
	{
		for (int roomNum = 0; roomNum < allThowmpeData[stageNum].size(); ++roomNum)
		{
			for (int errorNum = 0; errorNum < allThowmpeData[stageNum][roomNum].size(); ++errorNum)
			{
				if (allThowmpeData[stageNum][roomNum][errorNum]->startPos.x == -1.0f &&
					allThowmpeData[stageNum][roomNum][errorNum]->startPos.y == -1.0f)
				{
					errorStrings[stageNum][roomNum].push_back(std::string(gimmickName[GIMMCK_NAME_THOWMPE] + " " + std::to_string(errorNum) + " �J�n���W��ǉ����Ă�������"));
				}

				if (allThowmpeData[stageNum][roomNum][errorNum]->endPos.x == -1.0f &&
					allThowmpeData[stageNum][roomNum][errorNum]->endPos.y == -1.0f)
				{
					errorStrings[stageNum][roomNum].push_back(std::string(gimmickName[GIMMCK_NAME_THOWMPE] + " " + std::to_string(errorNum) + " �I�����W��ǉ����Ă�������"));
				}
			}
		}
	}



	std::string errorMessageString = "�X�e�[�W��ǂݍ��񂾍ۂɈꕔ�̃M�~�b�N���ǉ�����܂���ł����B\n�}�b�v�`�b�v���������ʒu�ɒu����Ă��邩�m�F���Ă�������\n\n";
	std::string gimmcikIntroString = "�M�~�b�N�̎��_�M�~�b�N�ԍ�_������̏��ŕ\�����܂�\n\n";
	std::string gimmickListTitleString = "[[�ǉ�����Ȃ������M�~�b�N���X�g]]\n";

	std::string gimmickListString(errorMessageString + gimmcikIntroString + gimmickListTitleString);

	bool checkErrorFlag = false;
	bool initStageFlag = false;
	bool initRoomFlag = false;
	for (int stageNum = 0; stageNum < errorStrings.size(); ++stageNum)
	{
		initStageFlag = false;
		for (int roomNum = 0; roomNum < errorStrings[stageNum].size(); ++roomNum)
		{
			initRoomFlag = false;
			for (int errorNum = 0; errorNum < errorStrings[stageNum][roomNum].size(); ++errorNum)
			{
				if (!initStageFlag)
				{
					if (stageNum == 0)
					{
						gimmickListString += "[�X�e�[�W" + std::to_string(stageNum) + "]\n";
					}
					else
					{
						gimmickListString += "\n[�X�e�[�W" + std::to_string(stageNum) + "]\n";
					}
					initStageFlag = true;

				}


				if (!initRoomFlag)
				{
					gimmickListString += "[�G���A" + std::to_string(roomNum) + "]\n";
					initRoomFlag = true;
				}
				gimmickListString += errorStrings[stageNum][roomNum][errorNum] + "\n";
				checkErrorFlag = true;
			}
		}
	}

	if (checkErrorFlag)
	{
		MessageBox(NULL, KuroFunc::GetWideStrFromStr(gimmickListString).c_str(), TEXT("�M�~�b�N�g�p�s��"), MB_OK);
	}
}

GimmickLoader::GimmickName GimmickLoader::CheckName(const std::string &KEY_NAME, std::istringstream *LINE_STREAM)
{
	for (int i = 0; i < gimmickCommonName.size(); ++i)
	{
		if (KEY_NAME == gimmickCommonName[i])
		{
			//���O���m�F����
			std::string name;
			*LINE_STREAM >> name;


			bool errorFlag = true;
			int convertIndexToEnum = -1;		//�Y��������enum�ɕϊ�����ׂɎg���ϐ�
			for (int nameIndex = 0; nameIndex < gimmickName.size(); ++nameIndex)
			{
				//�ǂݍ��񂾃M�~�b�N�����o�^����Ă���M�~�b�N���Ȃ̂��ǂ������f����
				if (gimmickName[nameIndex] == name)
				{
					errorFlag = false;
					convertIndexToEnum = nameIndex;
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
			return GimmickName(static_cast<GimmickName>(convertIndexToEnum));
		}
	}
	return GimmickName(-1);
}

void GimmickLoader::LoadThowmpeData(const std::string &KEY, std::istringstream *LINE_STREAM, std::shared_ptr<ThownpeData> THOWNPE_DATA)
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

void GimmickLoader::LoadThownData(const std::string &KEY, std::istringstream *LINE_STREAM, std::shared_ptr<ThownData> THOWN_DATA)
{
	if (KEY == "hitBox")
	{
		Vec2<float> sizeData;
		*LINE_STREAM >> sizeData.x;
		*LINE_STREAM >> sizeData.y;
		THOWN_DATA->adjValue = sizeData;
	}
}

void GimmickLoader::AddErrorStirng(const int &STAGE_NUM, const int ROOM_NUM, const std::string &NAME)
{
	//�ǂ̃M�~�b�N���Ԉ���ēǂݍ��܂�悤�Ƃ��Ă��邩����
	errorStrings[STAGE_NUM][ROOM_NUM].push_back(NAME);
}
