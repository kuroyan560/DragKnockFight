#include "DebugKeyManager.h"
#include"../Engine/ImguiApp.h"
#include<assert.h>
#include"../Common/KuroFunc.h"

void DebugKeyManager::CountReset()
{
	for (int i = 0; i < coundKyeUsedNum.size(); ++i)
	{
		coundKyeUsedNum[i] = 0;
	}
}

bool DebugKeyManager::DebugKeyTrigger(int KEY, const std::string &KEY_NAME, const std::string &KEY_NUM)
{
#ifdef  _DEBUG
	CheckKey(KEY, KEY_NAME, KEY_NUM);
	return UsersInput::Instance()->OnTrigger(KEY);
#endif //  DEBUG
	return false;
}

bool DebugKeyManager::DebugKeyState(int KEY, const std::string &KEY_NAME, const std::string &KEY_NUM)
{
#ifdef  _DEBUG
	CheckKey(KEY, KEY_NAME, KEY_NUM);
	return UsersInput::Instance()->Input(KEY);
#endif //  DEBUG
	return false;
}

void DebugKeyManager::DrawImGui()
{
#ifdef  _DEBUG
	ImGui::Begin("DebugKey");
	for (int i = 0; i < debugStringData.size(); ++i)
	{
		std::string drawSting = debugStringData[i]->keyName + ":" + debugStringData[i]->keyNumber;
		ImGui::Text(drawSting.c_str());
	}
	ImGui::End();
#endif  _DEBUG
}

void DebugKeyManager::CheckKey(int KEY, const std::string &KEY_NAME, const std::string &KEY_NUM)
{
	if (debugKeyStorage.size() == 0)
	{
		debugKeyStorage.push_back(KEY);
		debugStringData.push_back(std::make_shared<DebugData>(KEY_NUM, KEY_NAME));
		coundKyeUsedNum.push_back(0);
	}

	//見つからなかったら追加する
	bool findFlag = false;
	for (int i = 0; i < debugKeyStorage.size(); ++i)
	{
		if (debugKeyStorage[i] == KEY)
		{
			findFlag = true;
			break;
		}
	}
	if (!findFlag)
	{
		debugKeyStorage.push_back(KEY);
		debugStringData.push_back(std::make_shared<DebugData>(KEY_NUM, KEY_NAME));
		coundKyeUsedNum.push_back(0);
	}


	for (int i = 0; i < debugKeyStorage.size(); ++i)
	{
		if (debugKeyStorage[i] == KEY)
		{
			++coundKyeUsedNum[i];
		}
	}

	//同じキーが二回以上使われていたらエラーを吐く
	for (int i = 0; i < coundKyeUsedNum.size(); ++i)
	{
		if (2 <= coundKyeUsedNum[i])
		{
			std::string name = debugStringData[i]->keyNumber + "は既に使われています。片方のみ別のキーを使用してください";
			MessageBox(NULL, KuroFunc::GetWideStrFromStr(name).c_str(), TEXT("デバックキー被り"), MB_OK);
			assert(0);
		}
	}
}
