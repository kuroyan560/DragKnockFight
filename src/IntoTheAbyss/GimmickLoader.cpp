#include "GimmickLoader.h"
#include"KuroEngine.h"

GimmickLoader::GimmickLoader()
{
	int stageNum = 4;
	allGimmickData.reserve(stageNum);
	allGimmickData.resize(stageNum);
	allBubbleData.reserve(stageNum);
	allBubbleData.resize(stageNum);

	//使用できるギミック名の初期化
	gimmickName[GIMMCK_NAME_THOWMPE] = "Thwomp";

	//使用できる全ギミック共通のKey値の文字の初期化
	gimmickCommonName[GIMMCK_COMMON_DATA_NAME] = "name";

	//特定のギミックのKey値の文字の初期化
	gimmickThowmpeDataName[GIMMCK_THOWMPE_DATA_TYPE] = "type";
	gimmickThowmpeDataName[GIMMCK_THOWMPE_DATA_SIZE] = "size";

	//特定のギミックの種類で使える文字の初期化
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
		if (key == gimmickCommonName[GIMMCK_COMMON_DATA_NAME])
		{
			gimmickData.push_back(std::make_shared<ThownpeData>());
			gimmickArrayNum = gimmickData.size() - 1;

			//名前を確認する
			std::string name;
			line_stream >> name;


			bool errorFlag = true;
			int nameIndex = -1;		//添え字からenumに変換する為に使う変数
			for (int i = 0; i < allGimmickData[STAGE_NUM].size(); ++i)
			{
				//読み込んだギミック名が登録されているギミック名なのかどうか判断する
				if (gimmickName[i] == name)
				{
					errorFlag = false;
					nameIndex = i;
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
			eGimmickName = static_cast<GimmickName>(nameIndex);
		}


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
		allGimmickData[STAGE_NUM][ROOM_NUM] = gimmickData;
	}

	bool debug = false;
	
}

std::vector<std::shared_ptr<BubbleData>> GimmickLoader::GetBubbleData(const int& STAGE_NUM, const int& ROOM_NUM)
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

std::vector<std::shared_ptr<ThownpeData>> GimmickLoader::GetThowpeData(const int& STAGE_NUM, const int& ROOM_NUM)
{
	//ステージ番号が配列の範囲外なら返さない
	if (STAGE_NUM < 0 || allGimmickData.size() < STAGE_NUM)
	{
		return std::vector<std::shared_ptr<ThownpeData>>();
	}
	//部屋番号が配列の範囲外なら返さない
	if (ROOM_NUM < 0 || allGimmickData[STAGE_NUM].size() < ROOM_NUM)
	{
		return std::vector<std::shared_ptr<ThownpeData>>();
	}
	//指定した場所がnullptrなら返さない
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
	//ステージ番号が配列の範囲外ならfalse
	if (STAGE_NUM < 0 || allGimmickData.size() < STAGE_NUM)
	{
		return false;
	}
	//部屋番号が配列の範囲外ならfalse
	if (ROOM_NUM < 0 || allGimmickData[STAGE_NUM].size() < ROOM_NUM)
	{
		return false;
	}
	//指定した場所がnullptrならfalse
	if (GIMMICK_NUMBER < 0 || allGimmickData[STAGE_NUM][ROOM_NUM].size() < GIMMICK_NUMBER)
	{
		return false;
	}

	return true;
}

void GimmickLoader::LoadThowmpeData(const std::string& KEY, std::istringstream* LINE_STREAM, std::shared_ptr<ThownpeData> THOWNPE_DATA)
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
