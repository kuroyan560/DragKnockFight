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


	for (int i = 0; i < errorName.size(); ++i)
	{
		std::string name = "DIK_" + std::to_string(i);
		errorName[i] = name;
	}
}

bool DebugKeyManager::DebugKeyTrigger(int KEY, const std::string &KEY_NAME, const std::string &KEY_NUM)
{
#ifdef  _DEBUG
	CheckKey(KEY, KEY_NAME, KEY_NUM);
	return UsersInput::Instance()->KeyOnTrigger(KEY);
#endif //  DEBUG
	return false;
}

bool DebugKeyManager::DebugKeyState(int KEY, const std::string &KEY_NAME, const std::string &KEY_NUM)
{
#ifdef  _DEBUG
	CheckKey(KEY, KEY_NAME, KEY_NUM);
	return UsersInput::Instance()->KeyInput(KEY);
#endif //  DEBUG
	return false;
}

void DebugKeyManager::DrawImGui()
{
#ifdef  _DEBUG
	if (DebugImGuiManager::Instance()->DrawFlag(imguiHandle))
	{
		ImGui::Begin("DebugKey");
		for (int i = 0; i < debugStringData.size(); ++i)
		{
			std::string drawSting = debugStringData[i]->keyName + ":" + debugStringData[i]->keyNumber;
			ImGui::Text(drawSting.c_str());
		}
		ImGui::End();
	}
#endif //  DEBUG
}

void DebugKeyManager::CheckKey(int KEY, const std::string &KEY_NAME, const std::string &KEY_NUM)
{
	if (debugKeyStorage.size() == 0)
	{
		CheckErrorNumber(KEY_NUM);
		debugKeyStorage.push_back(KEY);
		debugStringData.push_back(std::make_shared<DebugData>(KEY_NUM, KEY_NAME));
		coundKyeUsedNum.push_back(0);
	}

	//������Ȃ�������ǉ�����
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
		CheckErrorNumber(KEY_NUM);
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

	//�����L�[�����ȏ�g���Ă�����G���[��f��
	for (int i = 0; i < coundKyeUsedNum.size(); ++i)
	{
		if (2 <= coundKyeUsedNum[i])
		{
			std::string name = debugStringData[i]->keyNumber + "�͊��Ɏg���Ă��܂��B�Е��̂ݕʂ̃L�[���g�p���Ă�������";
			MessageBox(NULL, KuroFunc::GetWideStrFromStr(name).c_str(), TEXT("�f�o�b�N�L�[���"), MB_OK);
			assert(0);
		}
	}
}
