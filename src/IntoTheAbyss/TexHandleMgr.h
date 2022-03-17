#pragma once
#include<vector>
#include<memory>
class TextureBuffer;
#include<string>
#include"Vec.h"

static class TexHandleMgr
{
	//�o�^���
	struct RegisterInfo
	{
		std::string path;
		int handle;

		RegisterInfo(const std::string& Path, const int& Handle) :path(Path), handle(Handle) {}
	};
	static std::vector<RegisterInfo>info;

	struct RegisterInfo_Div
	{
		std::string path;
		std::vector<int>handles;

		RegisterInfo_Div(const std::string& Path, const std::vector<int>& Handles) :path(Path), handles(Handles) {}
	};
	static std::vector<RegisterInfo_Div>info_div;

	//�e�N�X�`���o�b�t�@
	static std::vector<std::shared_ptr<TextureBuffer>>textures;

public:
	static int LoadGraph(const std::string& Path);
	static void LoadDivGraph(const std::string& Path, const int& AllNum, const Vec2<int>& SplitNum, int* Array);

	static const std::shared_ptr<TextureBuffer>& GetTexBuffer(const int& Handle) { return textures[Handle]; }

	static const std::vector<std::shared_ptr<TextureBuffer>>&DebugGet() { return textures; }
};
