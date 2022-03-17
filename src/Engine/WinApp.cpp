#include "WinApp.h"
#include"KuroFunc.h"

WinApp* WinApp::INSTANCE = nullptr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

//�V�X�e�����b�Z�[�W���������邽�߂̊֐��A�E�B���h�E�v���V�[�W�������
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    //���b�Z�[�W�ŕ���
    switch (msg)
    {
    case WM_DESTROY: //�E�B���h�E���j�����ꂽ
        PostQuitMessage(0);	//OS�ɑ΂��ăA�v���̏I����`����
        return 0;
    }
    ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
    return DefWindowProc(hwnd, msg, wparam, lparam);	//�W���̏������s��
}

void WinApp::Initialize(const std::string& WinName, const Vec2<int>WinSize, const bool& FullScreen, const wchar_t* IconPath = nullptr)
{
    const std::wstring winWideName = KuroFunc::GetWideStrFromStr(WinName);
    winSize = WinSize;

    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.lpszClassName = winWideName.c_str();
    wc.style = CS_HREDRAW | CS_VREDRAW;
    auto hInstance = GetModuleHandle(nullptr);
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    //�E�B���h�E�A�C�R���ݒ�
    if (IconPath != nullptr)
    {
        wc.hIcon = (HICON)LoadImage(
            NULL, IconPath, IMAGE_ICON,
            0, 0, LR_SHARED | LR_LOADFROMFILE);
    }
    
    //�E�B���h�E�N���X��OS�ɓo�^
    RegisterClassEx(&wc);

    RECT rect;
    DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX;
    if (FullScreen)
    {
        // Get the settings of the primary display
        DEVMODE devMode = {};
        devMode.dmSize = sizeof(DEVMODE);
        EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

        //�E�B���h�E�T�C�Y{ X���W�@Y���W�@�����@�c�� }
        rect = 
        {
            devMode.dmPosition.x,
            devMode.dmPosition.y,
            devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth),
            devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
        };

        winSize.x = rect.right - rect.left;
        winSize.y = rect.bottom - rect.top;

        winDifferRate = winSize.Float() / WinSize.Float();
    }
    else
    {
        rect = { 0,0, winSize.x, winSize.y };
        AdjustWindowRect(&rect, dwStyle, FALSE);
    }

    hwnd = CreateWindow(wc.lpszClassName,  //�N���X��
        winWideName.c_str(),    //�^�C�g���o�[
        dwStyle,    //�E�B���h�E�X�^�C��
        CW_USEDEFAULT,  //�\��X���W�iOS�ɔC����j
        CW_USEDEFAULT,  //�\��Y���W�iOS�ɔC����j
        rect.right - rect.left, //�E�B���h�E����
        rect.bottom - rect.top, //�E�B���h�E�c��
        nullptr,    //�e�E�B���h�E�n���h��
        nullptr,    //���j���[�n���h��
        hInstance,  //�Ăяo���A�v���P�[�V�����n���h��
        nullptr);   //�I�v�V����

    //�E�B���h�E�\��
    if (FullScreen)
    {
        SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX /*| WS_SYSMENU*/ | WS_THICKFRAME));

        SetWindowPos(
            hwnd,
            HWND_TOPMOST,
            rect.left,
            rect.top,
            rect.right,
            rect.bottom,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);

        //�E�B���h�E�\��
        ShowWindow(hwnd, SW_MAXIMIZE);
    }
    else
    {
        //�E�B���h�E�\��
        ShowWindow(hwnd, SW_SHOW);
    }
}