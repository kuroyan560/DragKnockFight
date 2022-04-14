#include "DebugKeyManager.h"
#include"../Engine/ImguiApp.h"

void DebugKeyManager::CountReset()
{
	for (int i = 0; i < coundKyeUsedNum.size(); ++i)
	{
		coundKyeUsedNum[i] = 0;
	}
}

bool DebugKeyManager::DebugKeyTrigger(int KEY, const std::string &KEY_NAME)
{
	CheckKey(KEY,KEY_NAME);
	return UsersInput::Instance()->OnTrigger(KEY);
}

bool DebugKeyManager::DebugKeyState(int KEY, const std::string &KEY_NAME)
{
	CheckKey(KEY, KEY_NAME);
	return UsersInput::Instance()->Input(KEY);
}

void DebugKeyManager::DrawImGui()
{
	ImGui::Begin("DebugKey");
	for (int i = 0; i < debugStringData.size(); ++i)
	{
		std::string drawSting = debugStringData[i] + ":";
		ImGui::Text("");
	}
	ImGui::End();
}

void DebugKeyManager::CheckKey(int KEY, const std::string &KEY_NAME)
{
	//������Ȃ�������ǉ�����
	for (int i = 0; i < debugKeyStorage.size(); ++i)
	{
		if (debugKeyStorage[i] != KEY)
		{
			debugKeyStorage.push_back(KEY);
			debugStringData.push_back(KEY_NAME);
			coundKyeUsedNum.push_back(0);
		}
	}

	//�����L�[�����ȏ�g���Ă�����G���[��f��
	for (int i = 0; i < coundKyeUsedNum.size(); ++i)
	{
		if (2 <= coundKyeUsedNum[i])
		{
			return;
		}
	}

	for (int i = 0; i < debugKeyStorage.size(); ++i)
	{
		if (debugKeyStorage[i] == KEY)
		{
			++coundKyeUsedNum[i];
		}
	}

}
