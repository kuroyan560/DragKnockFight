#include "TexHandleMgr.h"
#include"KuroEngine.h"

std::vector<TexHandleMgr::RegisterInfo>TexHandleMgr::info;
std::vector<TexHandleMgr::RegisterInfo_Div>TexHandleMgr::info_div;
std::vector<std::shared_ptr<TextureBuffer>>TexHandleMgr::textures;

int TexHandleMgr::LoadGraph(const std::string& Path)
{
    for (auto& i : info)
    {
        if (i.path == Path)return i.handle;
    }

    auto tex = D3D12App::Instance()->GenerateTextureBuffer(Path);
    textures.emplace_back(tex);

    int handle = textures.size() - 1;
    info.emplace_back(Path, handle);

    return handle;
}

void TexHandleMgr::LoadDivGraph(const std::string& Path, const int& AllNum, const Vec2<int>& SplitNum, int* Array)
{
    for (auto& info : info_div)
    {
        if (info.path == Path)
        {
            for (int i = 0; i < info.handles.size(); ++i)
            {
                Array[i] = info.handles[i];
            }
            return;
        }
    }

    auto tex = D3D12App::Instance()->GenerateTextureBuffer(Path, AllNum, SplitNum);

    std::vector<int>handles;

    for (int i = 0; i < tex.size(); ++i)
    {
        textures.emplace_back(tex[i]);
        int handle = textures.size() - 1;
        handles.emplace_back(handle);
    }

    info_div.emplace_back(Path, handles);

    for (int i = 0; i < handles.size(); ++i)Array[i] = handles[i];
}
