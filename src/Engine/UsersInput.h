#pragma once
#include"Vec.h"
#include<stdio.h>

#include <dinput.h>
#define DIRECTINPUT_VERSION 0x0800	//DirectInput�̃o�[�W�����w��
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include<wrl.h>
#include <Xinput.h>

//#include"Collision.h"
//namespace CollisionPrimitve
//{
//	class Ray;
//}

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

public:
	static UsersInput* INSTANCE;
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
	XINPUT_STATE xinputState;
	XINPUT_STATE oldXinputState;
	float shakePower = 0.0f;;
	int shakeTimer = 0;
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
	bool OnTrigger(int KeyCode);
	bool Input(int KeyCode);
	bool OffTrigger(int KeyCode);

	//�}�E�X
	bool OnTrigger(MOUSE_BUTTON Button);
	bool Input(MOUSE_BUTTON Button);
	bool OffTrigger(MOUSE_BUTTON Button);

	const Vec2<float>& GetMousePos()const { return mousePos; }
	MouseMove GetMouseMove();
	//Ray GetMouseRay();

	//XBOX�R���g���[���[
	bool OnTrigger(XBOX_BUTTON Button);
	bool OnTrigger(XBOX_STICK StickInput, const float& DeadRange = 0.3f, const Vec2<float>& DeadRate = { 1.0f,1.0f });
	bool Input(XBOX_BUTTON Button);
	bool Input(XBOX_STICK StickInput, const float& DeadRange = 0.3f, const Vec2<float>& DeadRate = { 1.0f,1.0f });
	bool OffTrigger(XBOX_BUTTON Button);
	bool OffTrigger(XBOX_STICK StickInput, const float& DeadRange = 0.3f, const Vec2<float>& DeadRate = { 1.0f,1.0f });

	//�f�b�h�]�[������̓x����(1.0f���ƃf�t�H���g�̗p)
	Vec2<float>GetLeftStickVec(const Vec2<float>& DeadRate = { 1.0f,1.0f });
	Vec2<float>GetRightStickVec(const Vec2<float>& DeadRate = { 1.0f,1.0f });
	void ShakeController(const float& Power, const int& Span);
};

