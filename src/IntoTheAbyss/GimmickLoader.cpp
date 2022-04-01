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


	//使用できるギミック名の初期化
	gimmickName[GIMMCK_NAME_THOWMPE] = "Thwomp";
	gimmickName[GIMMCK_NAME_THOWN] = "Thown";

	//使用できる全ギミック共通のKey値の文字の初期化
	gimmickCommonName[GIMMCK_COMMON_DATA_NAME] = "name";

	//特定のギミックのKey値の文字の初期化
	gimmickThowmpeDataName[GIMMCK_THOWMPE_DATA_TYPE] = "type";
	gimmickThowmpeDataName[GIMMCK_THOWMPE_DATA_SIZE] = "size";

	//特定のギミックの種類で使える文字の初期化
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

	// ファイルデータ
	std::ifstream ifs;
	// ファイルを開く。
	ifs.open(FILE_PASS);
	// ファイルが開けたかをチェックする。
	if (ifs.fail())
	{
		return;
	}

	//ファイルから情報を選択
	std::string line;
	//ギミックデータを追加する

	GimmickName eGimmickName = GIMMCK_NAME_NONE;

	while (getline(ifs, line))
	{


		//一行分の文字列をストリームに変換して解析しやすくなる
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		std::string key;//ここでvかf等の判断をする
		getline(line_stream, key, ' ');



		//Key値エラー-----------------------
		bool keyErrorFlag = true;	//エラー探知用のフラグ
		//共通のKEY値を探索
		for (int keyIndex = 0; keyIndex < gimmickCommonName.size(); ++keyIndex)
		{
			if (gimmickCommonName[keyIndex] == key)
			{
				keyErrorFlag = false;
			}
		}
		//特定のKEY値を探索
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
		//空白はエラーを出さない
		if (key == "")
		{
			keyErrorFlag = false;
		}
		//エラー処理
		if (keyErrorFlag)
		{
			std::string errorName = key + "は使う事が出来ないKey値です。\n誤字を書いていないか確認お願いします。";
			MessageBox(NULL, KuroFunc::GetWideStrFromStr(errorName).c_str(), TEXT("Key値使用不可"), MB_OK);
			assert(0);
		}
		//Key値エラー-----------------------


		//keyがnameならギミック名を見る
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
		//登録されたギミック名からどの処理を通すか決める
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
	// ファイルデータ
	std::ifstream ifs;
	// ファイルを開く。
	ifs.open(FILE_PASS);
	// ファイルが開けたかをチェックする。
	if (ifs.fail())
	{
		return;
	}

	//ファイルから情報を選択
	std::string line;
	//ギミックデータを追加する

	GimmickName eGimmickName = GIMMCK_NAME_NONE;

	while (getline(ifs, line))
	{

		//一行分の文字列をストリームに変換して解析しやすくなる
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		std::string key;//ここでvかf等の判断をする
		getline(line_stream, key, ' ');



		//Key値エラー-----------------------
		bool keyErrorFlag = true;	//エラー探知用のフラグ
		//共通のKEY値を探索
		for (int keyIndex = 0; keyIndex < gimmickCommonName.size(); ++keyIndex)
		{
			if (gimmickCommonName[keyIndex] == key)
			{
				keyErrorFlag = false;
			}
		}
		//特定のKEY値を探索
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
		//空白はエラーを出さない
		if (key == "")
		{
			keyErrorFlag = false;
		}
		//エラー処理
		if (keyErrorFlag)
		{
			std::string errorName = key + "は使う事が出来ないKey値です。\n誤字を書いていないか確認お願いします。";
			MessageBox(NULL, KuroFunc::GetWideStrFromStr(errorName).c_str(), TEXT("Key値使用不可"), MB_OK);
			assert(0);
		}
		//Key値エラー-----------------------


		//keyがnameならギミック名を見る
		GimmickName get = CheckName(key, &line_stream);
		if (get != -1 || eGimmickName == -1)
		{
			eGimmickName = get;
		}


		//登録されたギミック名からどの処理を通すか決める
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
	//ステージ番号が配列の範囲外なら返さない
	if (STAGE_NUM < 0 || allBubbleData.size() <= STAGE_NUM)
	{
		return std::vector<std::shared_ptr<BubbleData>>();
	}
	//部屋番号が配列の範囲外なら返さない
	if (ROOM_NUM < 0 || allBubbleData[STAGE_NUM].size() <= ROOM_NUM)
	{
		return std::vector<std::shared_ptr<BubbleData>>();
	}
	//指定した場所がnullptrなら返さない
	if (allBubbleData[STAGE_NUM][ROOM_NUM].size() == 0)
	{
		return std::vector<std::shared_ptr<BubbleData>>();
	}
	return allBubbleData[STAGE_NUM][ROOM_NUM];
}

std::vector<std::shared_ptr<ThownpeData>> GimmickLoader::GetThowpeData(const int &STAGE_NUM, const int &ROOM_NUM)
{
	//ステージ番号が配列の範囲外なら返さない
	if (STAGE_NUM < 0 || allThowmpeData.size() < STAGE_NUM)
	{
		return std::vector<std::shared_ptr<ThownpeData>>();
	}
	//部屋番号が配列の範囲外なら返さない
	if (ROOM_NUM < 0 || allThowmpeData[STAGE_NUM].size() < ROOM_NUM)
	{
		return std::vector<std::shared_ptr<ThownpeData>>();
	}
	//指定した場所がnullptrなら返さない
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

		const std::string gimmickLoadErrorString = gimmickName[GIMMCK_NAME_THOWMPE] + " " + std::to_string(GIMMICK_NUMBER) + " ギミックファイルに情報が書かれていません";
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

		const std::string gimmickLoadErrorString = gimmickName[GIMMCK_NAME_THOWMPE] + " " + std::to_string(GIMMICK_NUMBER) + " ギミックファイルに情報が書かれていません";
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
	//ステージ番号が配列の範囲外ならfalse
	if (STAGE_NUM < 0 || allThowmpeData.size() < STAGE_NUM)
	{
		return false;
	}
	//部屋番号が配列の範囲外ならfalse
	if (ROOM_NUM < 0 || allThowmpeData[STAGE_NUM].size() < ROOM_NUM)
	{
		return false;
	}
	//指定した場所がnullptrならfalse
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
					errorStrings[stageNum][roomNum].push_back(std::string(gimmickName[GIMMCK_NAME_THOWMPE] + " " + std::to_string(errorNum) + " 開始座標を追加してください"));
				}

				if (allThowmpeData[stageNum][roomNum][errorNum]->endPos.x == -1.0f &&
					allThowmpeData[stageNum][roomNum][errorNum]->endPos.y == -1.0f)
				{
					errorStrings[stageNum][roomNum].push_back(std::string(gimmickName[GIMMCK_NAME_THOWMPE] + " " + std::to_string(errorNum) + " 終了座標を追加してください"));
				}
			}
		}
	}



	std::string errorMessageString = "ステージを読み込んだ際に一部のギミックが追加されませんでした。\nマップチップが正しい位置に置かれているか確認してください\n\n";
	std::string gimmcikIntroString = "ギミックの種類_ギミック番号_解決策の順で表示します\n\n";
	std::string gimmickListTitleString = "[[追加されなかったギミックリスト]]\n";

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
						gimmickListString += "[ステージ" + std::to_string(stageNum) + "]\n";
					}
					else
					{
						gimmickListString += "\n[ステージ" + std::to_string(stageNum) + "]\n";
					}
					initStageFlag = true;

				}


				if (!initRoomFlag)
				{
					gimmickListString += "[エリア" + std::to_string(roomNum) + "]\n";
					initRoomFlag = true;
				}
				gimmickListString += errorStrings[stageNum][roomNum][errorNum] + "\n";
				checkErrorFlag = true;
			}
		}
	}

	if (checkErrorFlag)
	{
		MessageBox(NULL, KuroFunc::GetWideStrFromStr(gimmickListString).c_str(), TEXT("ギミック使用不可"), MB_OK);
	}
}

GimmickLoader::GimmickName GimmickLoader::CheckName(const std::string &KEY_NAME, std::istringstream *LINE_STREAM)
{
	for (int i = 0; i < gimmickCommonName.size(); ++i)
	{
		if (KEY_NAME == gimmickCommonName[i])
		{
			//名前を確認する
			std::string name;
			*LINE_STREAM >> name;


			bool errorFlag = true;
			int convertIndexToEnum = -1;		//添え字からenumに変換する為に使う変数
			for (int nameIndex = 0; nameIndex < gimmickName.size(); ++nameIndex)
			{
				//読み込んだギミック名が登録されているギミック名なのかどうか判断する
				if (gimmickName[nameIndex] == name)
				{
					errorFlag = false;
					convertIndexToEnum = nameIndex;
					break;
				}
			}

			//エラー処理
			if (errorFlag)
			{
				std::string errorName = name + "は使う事が出来ないギミック名です。\n誤字を書いていないか確認お願いします。";
				MessageBox(NULL, KuroFunc::GetWideStrFromStr(errorName).c_str(), TEXT("ギミック使用不可"), MB_OK);
				assert(0);
			}
			//登録されたギミック名から変換する
			return GimmickName(static_cast<GimmickName>(convertIndexToEnum));
		}
	}
	return GimmickName(-1);
}

void GimmickLoader::LoadThowmpeData(const std::string &KEY, std::istringstream *LINE_STREAM, std::shared_ptr<ThownpeData> THOWNPE_DATA)
{
	//ギミックの種類を判別する
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
			std::string errorName = name + "は使う事が出来ない種類です。\n誤字を書いていないか確認お願いします。";
			MessageBox(NULL, KuroFunc::GetWideStrFromStr(errorName).c_str(), TEXT("type使用不可"), MB_OK);
			assert(0);
		}
	}

	//ギミックのサイズを判別する
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
	//どのギミックが間違って読み込まれようとしているか見る
	errorStrings[STAGE_NUM][ROOM_NUM].push_back(NAME);
}
