#pragma once
#include"Vec.h"
#include<stdio.h>

#include <dinput.h>
#define DIRECTINPUT_VERSION 0x0800	//DirectInput�̃o�[�W�����w��
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include<wrl.h>
#include <Xinput.h>

enum MOUSE_BUTTON { LEFT, RIGHT, CENTER };
enum XBOX_BUTTON {
	B = XINPUT_GAMEPAD_B,
	A = XINPUT_GAMEPAD_A,
	X = XINPUT_GAMEPAD_X,
	Y = XINPUT_GAMEPAD_Y,
	START = XINPUT_GAMEPAD_START,
	BACK = XINPUT_GAMEPAD_BACK,
	LB = XINPUT_GAMEPAD_LEFT_SHOULDER,
	RB = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	DPAD_UP = XINPUT_GAMEPAD_DPAD_UP,
	DPAD_DOWN = XINPUT_GAMEPAD_DPAD_DOWN,
	DPAD_LEFT = XINPUT_GAMEPAD_DPAD_LEFT,
	DPAD_RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,
	LT, RT
};
enum XBOX_STICK {
	L_UP, L_DOWN, L_LEFT, L_RIGHT,
	R_UP, R_DOWN, R_LEFT, R_RIGHT, XBOX_STICK_NUM
};

class UsersInput
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	static UsersInput* INSTANCE;
public:
	static UsersInput* Instance()
	{
		if (INSTANCE == nullptr)
		{
			printf("UsersInput�̃C���X�^���X���Ăяo�����Ƃ��܂�����nullptr�ł���\n");
			assert(0);
		}
		return INSTANCE;
	}

	struct MouseMove
	{
		LONG IX;
		LONG IY;
		LONG IZ;
	};

private:
	ComPtr<IDirectInput8> dinput = nullptr;

	//�L�[�{�[�h
	ComPtr<IDirectInputDevice8> keyDev;
	static const int KEY_NUM = 256;
	BYTE keys[KEY_NUM] = {};
	BYTE oldkeys[KEY_NUM] = {};

	//�}�E�X
	ComPtr<IDirectInputDevice8>mouseDev;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 oldMouseState = {};
	//�}�E�X�̃Q�[����ԓ��ł̃��C
	Vec2<float> mousePos;

	//XINPUT(�R���g���[���[�p)
	static const int CONTROLLER_NUM = 3;
	XINPUT_STATE xinputState[CONTROLLER_NUM];
	XINPUT_STATE oldXinputState[CONTROLLER_NUM];
	float shakePower[CONTROLLER_NUM] = { 0.0f };
	int shakeTimer[CONTROLLER_NUM] = { 0 };
	//�f�b�h�]�[���ɓ����Ă��邩(DeadRate : �f�b�h�]�[������̓x�����A1.0f���ƃf�t�H���g)
	bool StickInDeadZone(Vec2<float>& Thumb, const Vec2<float>& DeadRate);

	void Initialize(const WNDCLASSEX& WinClass,const HWND& Hwnd);

public:
	UsersInput(const WNDCLASSEX& WinClass, const HWND& Hwnd)
	{
		if (INSTANCE != nullptr)
		{
			printf("���ɃC���X�^���X������܂��BUsersInput�͂P�̃C���X�^���X�������Ă܂���\n");
			assert(0);
		}
		INSTANCE = this;
		Initialize(WinClass, Hwnd);
	}
	~UsersInput() {};

	void Update(const HWND& Hwnd, const Vec2<float>& WinSize);

	//�L�[�{�[�h
	bool KeyOnTrigger(int KeyCode);
	bool KeyInput(int KeyCode);
	bool KeyOffTrigger(int KeyCode);

	//�}�E�X
	bool MouseOnTrigger(MOUSE_BUTTON Button);
	bool MouseInput(MOUSE_BUTTON Button);
	bool MouseOffTrigger(MOUSE_BUTTON Button);

	const Vec2<float>& GetMousePos()const { return mousePos; }
	MouseMove GetMouseMove();
	//Ray GetMouseRay();

	//XBOX�R���g���[���[
	bool ControllerOnTrigger(const int& ControllerIdx, XBOX_BUTTON Button);
	bool ControllerOnTrigger(const int& ControllerIdx, XBOX_STICK StickInput, const float& DeadRange = 0.3f, const Vec2<float>& DeadRate = { 1.0f,1.0f });
	bool ControllerInput(const int& ControllerIdx, XBOX_BUTTON Button);
	bool ControllerInput(const int& ControllerIdx, XBOX_STICK StickInput, const float& DeadRange = 0.3f, const Vec2<float>& DeadRate = { 1.0f,1.0f });
	bool ControllerOffTrigger(const int& ControllerIdx, XBOX_BUTTON Button);
	bool ControllerOffTrigger(const int& ControllerIdx, XBOX_STICK StickInput, const float& DeadRange = 0.3f, const Vec2<float>& DeadRate = { 1.0f,1.0f });

	//�f�b�h�]�[������̓x����(1.0f���ƃf�t�H���g�̗p)
	Vec2<float>GetLeftStickVec(const int& ControllerIdx, const Vec2<float>& DeadRate = { 1.0f,1.0f });
	Vec2<float>GetRightStickVec(const int& ControllerIdx, const Vec2<float>& DeadRate = { 1.0f,1.0f });
	// "Power" must fit between 0.0f and 1.0f.
	void ShakeController(const int& ControllerIdx, const float& Power, const int& Span);

	// ���̃X�e�B�b�N�̃��[�g���~���������̂ŗՎ��Ŏ������܂����B�J�����I�����������Ă��������G�G
	Vec2<float>GetLeftStickVecFuna(const int& ControllerIdx){return Vec2<float>(xinputState[ControllerIdx].Gamepad.sThumbLX, -xinputState[ControllerIdx].Gamepad.sThumbLY);}
	Vec2<float>GetRightStickVecFuna(const int& ControllerIdx){return Vec2<float>(xinputState[ControllerIdx].Gamepad.sThumbRX, -xinputState[ControllerIdx].Gamepad.sThumbRY);}

};

