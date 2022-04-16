#include "UsersInput.h"
#include <cassert>
#include<DirectXMath.h>
#include"KuroFunc.h"

#pragma comment(lib,"xinput.lib")

#define STICK_INPUT_MAX 32768.0f

UsersInput* UsersInput::INSTANCE = nullptr;

bool UsersInput::StickInDeadZone(Vec2<float>& Thumb, const Vec2<float>& DeadRate)
{
	bool x = false, y = false;
	if ((Thumb.x < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadRate.x
		&& Thumb.x > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadRate.x))
	{
		Thumb.x = 0.0f;
		x = true;
	}
	if((Thumb.y < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadRate.y
			&& Thumb.y > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadRate.y))
	{
		Thumb.y = 0.0f;
		y = true;
	}
	if (x && y)return true;
	return false;
}

void UsersInput::Initialize(const WNDCLASSEX& WinClass, const HWND& Hwnd)
{
	HRESULT hr;
	//DirectInput�I�u�W�F�N�g�̐���
	if (FAILED(hr = DirectInput8Create(
		WinClass.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr)))assert(0);

	//�L�[�{�[�h�f�o�C�X�̐���
	if (FAILED(hr = dinput->CreateDevice(GUID_SysKeyboard, &keyDev, NULL)))assert(0);
	if (FAILED(hr = keyDev->SetDataFormat(&c_dfDIKeyboard)))assert(0);	//�W���`��
	if (FAILED(hr = keyDev->SetCooperativeLevel(		//�r�����䃌�x���̃Z�b�g
		Hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE/* | DISCL_NOWINKEY*/)))assert(0);

	//�}�E�X�f�o�C�X�̐���
	if (FAILED(hr = dinput->CreateDevice(GUID_SysMouse, &mouseDev, NULL)))assert(0);
	if (FAILED(hr = mouseDev->SetDataFormat(&c_dfDIMouse2)))assert(0);
	if (FAILED(hr = mouseDev->SetCooperativeLevel(
		Hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY)))assert(0);

	/*
		�g���Ă���t���O�ɂ���
		DISCL_FOREGROUND	��ʂ���O�ɂ���ꍇ�̂ݓ��͂��󂯕t����
		DISCL_NONEXCLUSIVE	�f�o�C�X�����̃A�v�������Ő�L���Ȃ�
		DISCL_NOWINKEY		Windows�L�[�𖳌��ɂ���
	*/
}

void UsersInput::Update(const HWND& Hwnd, const Vec2<float>& WinSize)
{
	//�L�[�{�[�h
	memcpy(oldkeys, keys, KEY_NUM);
	//�L�[�{�[�h���̎擾�J�n
	HRESULT result = keyDev->Acquire();	//�{���͈��ł������A�A�v�������ʂ���S�ʂɖ߂�x�Ăяo���K�v�����邽�߁A���t���[���Ăяo���B
	//�S�L�[�̓��͏�Ԃ��擾����
	result = keyDev->GetDeviceState(sizeof(keys), keys);

	//�}�E�X
	result = mouseDev->Acquire();
	oldMouseState = mouseState;
	result = mouseDev->GetDeviceState(sizeof(mouseState), &mouseState);

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(Hwnd, &p);
	mousePos.x = p.x;
	mousePos.x = KuroFunc::Clamp(0, WinSize.x, mousePos.x);
	mousePos.y = p.y;
	mousePos.y = KuroFunc::Clamp(0, WinSize.y, mousePos.y);

	//�R���g���[���[
	for (int i = 0; i < CONTROLLER_NUM; ++i)
	{
		oldXinputState[i] = xinputState[i];
		ZeroMemory(&xinputState[i], sizeof(XINPUT_STATE));

		DWORD dwResult = XInputGetState(i, &xinputState[i]);

		//�R���g���[���[���ڑ�����Ă��Ȃ�
		if (dwResult != ERROR_SUCCESS)continue;

		//�R���g���[���[���ڑ�����Ă���
		if (0 < shakeTimer[i])
		{
			shakeTimer[i]--;
			XINPUT_VIBRATION vibration;
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

			if (shakeTimer == 0)
			{
				vibration.wLeftMotorSpeed = 0.0f; // use any value between 0-65535 here
				vibration.wRightMotorSpeed = 0.0f; // use any value between 0-65535 here
			}
			else
			{
				vibration.wLeftMotorSpeed = 65535.0f * shakePower[i]; // use any value between 0-65535 here
				vibration.wRightMotorSpeed = 65535.0f * shakePower[i]; // use any value between 0-65535 here
			}
			XInputSetState(dwResult, &vibration);
		}
	}
}

bool UsersInput::KeyOnTrigger(int KeyCode)
{
	return (!oldkeys[KeyCode] && keys[KeyCode]);
}

bool UsersInput::KeyInput(int KeyCode)
{
	return keys[KeyCode];
}

bool UsersInput::KeyOffTrigger(int KeyCode)
{
	return (oldkeys[KeyCode] && !keys[KeyCode]);
}

bool UsersInput::MouseOnTrigger(MOUSE_BUTTON Button)
{
	return (!oldMouseState.rgbButtons[Button] && mouseState.rgbButtons[Button]);
}

bool UsersInput::MouseInput(MOUSE_BUTTON Button)
{
	return mouseState.rgbButtons[Button];
}

bool UsersInput::MouseOffTrigger(MOUSE_BUTTON Button)
{
	return (oldMouseState.rgbButtons[Button] && !mouseState.rgbButtons[Button]);
}

UsersInput::MouseMove UsersInput::GetMouseMove()
{
	MouseMove tmp;
	tmp.IX = mouseState.lX;
	tmp.IY = mouseState.lY;
	tmp.IZ = mouseState.lZ;
	return tmp;
}

//#include"Object.h"
//#include"WinApp.h"
//Ray UsersInput::GetMouseRay()
//{
//	Ray mouseRay;
//	Vec3<float> start = MyFunc::ConvertScreenToWorld(mousePos, 0.0f,
//	Camera::GetNowCam()->MatView(),
//		Camera::GetNowCam()->MatProjection(),
//		App::GetWinApp().GetWinSize());
//	mouseRay.start = { start.x,start.y,start.z };
//
//	Vec3<float> to = MyFunc::ConvertScreenToWorld(mousePos, 1.0f,
//		Camera::GetNowCam()->MatView(),
//		Camera::GetNowCam()->MatProjection(),
//		App::GetWinApp().GetWinSize());
//	Vec3<float> vec(to - start);
//	mouseRay.dir = vec.GetNormal();
//
//	return mouseRay;
//}

bool UsersInput::ControllerOnTrigger(const int& ControllerIdx, XBOX_BUTTON Button)
{
	//�g���K�[
	if (Button == LT)
	{
		return oldXinputState[ControllerIdx].Gamepad.bLeftTrigger <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD
			&& ControllerInput(ControllerIdx, Button);
	}
	else if (Button == RT)
	{
		return oldXinputState[ControllerIdx].Gamepad.bRightTrigger <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD
			&& ControllerInput(ControllerIdx, Button);
	}
	else
	{
		return !(oldXinputState[ControllerIdx].Gamepad.wButtons & Button)
			&& ControllerInput(ControllerIdx, Button);
	}
	assert(0);
	return false;
}

bool UsersInput::ControllerOnTrigger(const int& ControllerIdx, XBOX_STICK StickInput, const float& DeadRange, const Vec2<float>& DeadRate)
{
	Vec2<float>oldVec;
	Vec2<float>vec;
	bool isLeftStick = StickInput <= L_RIGHT;
	if (isLeftStick)
	{
		oldVec = Vec2<float>(oldXinputState[ControllerIdx].Gamepad.sThumbLX, oldXinputState[ControllerIdx].Gamepad.sThumbLY);
		vec = Vec2<float>(xinputState[ControllerIdx].Gamepad.sThumbLX, xinputState[ControllerIdx].Gamepad.sThumbLY);
	}
	else
	{
		oldVec = Vec2<float>(oldXinputState[ControllerIdx].Gamepad.sThumbRX, oldXinputState[ControllerIdx].Gamepad.sThumbRY);
		vec = Vec2<float>(xinputState[ControllerIdx].Gamepad.sThumbRX, xinputState[ControllerIdx].Gamepad.sThumbRY);
	}

	if (!StickInDeadZone(oldVec, DeadRate))return false;
	if (StickInDeadZone(vec, DeadRate))return false;

	bool result = false;

	if (StickInput % 4 == L_UP)
	{
		result = !(DeadRange < (oldVec.y / STICK_INPUT_MAX))
			&& DeadRange < (vec.y / STICK_INPUT_MAX);
	}
	else if (StickInput % 4 == L_DOWN)
	{
		result = !(oldVec.y / STICK_INPUT_MAX < -DeadRange)
			&& vec.y / STICK_INPUT_MAX < -DeadRange;
	}
	else if (StickInput % 4 == L_RIGHT)
	{
		result = !(DeadRange < (oldVec.x / STICK_INPUT_MAX))
			&& DeadRange < (vec.x / STICK_INPUT_MAX);
	}
	else if (StickInput % 4 == L_LEFT)
	{
		result =  !(oldVec.x / STICK_INPUT_MAX < -DeadRange)
			&& vec.x / STICK_INPUT_MAX < -DeadRange;
	}
	else
	{
		assert(0);
	}

	return result;
}

bool UsersInput::ControllerInput(const int& ControllerIdx, XBOX_BUTTON Button)
{
	if (Button == LT)
	{
		return XINPUT_GAMEPAD_TRIGGER_THRESHOLD < xinputState[ControllerIdx].Gamepad.bLeftTrigger;
	}
	else if (Button == RT)
	{
		return XINPUT_GAMEPAD_TRIGGER_THRESHOLD < xinputState[ControllerIdx].Gamepad.bRightTrigger;
	}
	else
	{
		return xinputState[ControllerIdx].Gamepad.wButtons & Button;
	}
	assert(0);
	return false;
}

bool UsersInput::ControllerInput(const int& ControllerIdx, XBOX_STICK StickInput, const float& DeadRange, const Vec2<float>& DeadRate)
{
	Vec2<float>vec;
	bool isLeftStick = StickInput <= L_RIGHT;
	if (isLeftStick)
	{
		vec = Vec2<float>(xinputState[ControllerIdx].Gamepad.sThumbLX, xinputState[ControllerIdx].Gamepad.sThumbLY);
	}
	else
	{
		vec = Vec2<float>(xinputState[ControllerIdx].Gamepad.sThumbRX, xinputState[ControllerIdx].Gamepad.sThumbRY);
	}

	if (StickInDeadZone(vec, DeadRate))return false;

	if (StickInput % 4 == L_UP)
	{
		return DeadRange < (vec.y / STICK_INPUT_MAX);
	}
	else if (StickInput % 4 == L_DOWN)
	{
		return  vec.y / STICK_INPUT_MAX < -DeadRange;
	}
	else if (StickInput % 4 == L_RIGHT)
	{
		return DeadRange < (vec.x / STICK_INPUT_MAX);
	}
	else if (StickInput % 4 == L_LEFT)
	{
		return  vec.x / STICK_INPUT_MAX < -DeadRange;
	}

	assert(0);
	return false;
}

bool UsersInput::ControllerOffTrigger(const int& ControllerIdx, XBOX_BUTTON Button)
{
	//�g���K�[
	if (Button == LT)
	{
		return XINPUT_GAMEPAD_TRIGGER_THRESHOLD < oldXinputState[ControllerIdx].Gamepad.bLeftTrigger
			&& !ControllerInput(ControllerIdx, Button);
	}
	else if (Button == RT)
	{
		return XINPUT_GAMEPAD_TRIGGER_THRESHOLD < oldXinputState[ControllerIdx].Gamepad.bRightTrigger
			&& !ControllerInput(ControllerIdx, Button);
	}
	//�{�^��
	else
	{
		return (oldXinputState[ControllerIdx].Gamepad.wButtons & Button)
			&& !ControllerInput(ControllerIdx, Button);
	}
	assert(0);
	return false;
}

bool UsersInput::ControllerOffTrigger(const int& ControllerIdx, XBOX_STICK StickInput, const float& DeadRange, const Vec2<float>& DeadRate)
{
	Vec2<float>oldVec;
	Vec2<float>vec;
	bool isLeftStick = StickInput <= L_RIGHT;
	if (isLeftStick)
	{
		oldVec = Vec2<float>(oldXinputState[ControllerIdx].Gamepad.sThumbLX, oldXinputState[ControllerIdx].Gamepad.sThumbLY);
		vec = Vec2<float>(xinputState[ControllerIdx].Gamepad.sThumbLX, xinputState[ControllerIdx].Gamepad.sThumbLY);
	}
	else
	{
		oldVec = Vec2<float>(oldXinputState[ControllerIdx].Gamepad.sThumbRX, oldXinputState[ControllerIdx].Gamepad.sThumbRY);
		vec = Vec2<float>(xinputState[ControllerIdx].Gamepad.sThumbRX, xinputState[ControllerIdx].Gamepad.sThumbRY);
	}

	if (!StickInDeadZone(oldVec, DeadRate))return false;
	if (StickInDeadZone(vec, DeadRate))return false;

	bool result = false;
	if (StickInput % 4 == L_UP)
	{
		result = DeadRange < (oldVec.y / STICK_INPUT_MAX)
			&& !(DeadRange < (vec.y / STICK_INPUT_MAX));
	}
	else if (StickInput % 4 == L_DOWN)
	{
		result = oldVec.y / STICK_INPUT_MAX < -DeadRange
			&& !(vec.y / STICK_INPUT_MAX < -DeadRange);
	}
	else if (StickInput % 4 == L_RIGHT)
	{
		result = DeadRange < (oldVec.x / STICK_INPUT_MAX)
			&& !(DeadRange < (vec.x / STICK_INPUT_MAX));
	}
	else if (StickInput % 4 == L_LEFT)
	{
		result = oldVec.x / STICK_INPUT_MAX < -DeadRange
			&& !(vec.x / STICK_INPUT_MAX < -DeadRange);
	}
	else
	{
		assert(0);
	}
	if (result)
	{
	}
	return result;
}

Vec2<float> UsersInput::GetLeftStickVec(const int& ControllerIdx, const Vec2<float>& DeadRate)
{
	Vec2<float>result(xinputState[ControllerIdx].Gamepad.sThumbLX, -xinputState[ControllerIdx].Gamepad.sThumbLY);
	StickInDeadZone(result, DeadRate);
	return result.GetNormal();
}

Vec2<float> UsersInput::GetRightStickVec(const int& ControllerIdx, const Vec2<float>& DeadRate)
{
	Vec2<float>result(xinputState[ControllerIdx].Gamepad.sThumbRX, -xinputState[ControllerIdx].Gamepad.sThumbRY);
	StickInDeadZone(result, DeadRate);
	return result.GetNormal();
}

void UsersInput::ShakeController(const int& ControllerIdx, const float& Power, const int& Span)
{
	if (!(0 < Power && Power <= 1.0f))assert(0);
	shakePower[ControllerIdx] = Power;
	shakeTimer[ControllerIdx] = Span;
}
