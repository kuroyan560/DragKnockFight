#pragma once
#include "Vec.h"
#include "Singleton.h"
#include"KuroFunc.h"

class ShakeMgr : public Singleton<ShakeMgr> {

public:

	/*===== �����o�ϐ� =====*/

	float maxShakeAmount;	// �V�F�C�N�ʂ̍ő�� �V�F�C�N���n�߂鎞�̓R����ς���B
	Vec2<float> shakeAmount;		// ���̃t���[���S�̂̃V�F�C�N�ʁB


public:

	/*===== �萔 =====*/

	const float FIRST_SHOT_SHAKE_AMOUNT = 5.0f;
	const float DOSSUN_LOW_POWER_SHAKE_AMOUNT = 5.0f;


public:

	/*===== �����o�֐� =====*/

	// ����������
	inline void Init() {

		maxShakeAmount = 0;
		shakeAmount = {};

	}

	// �X�V����
	inline void Update() {

		// �V�F�C�N�ʂ����炷�B
		if (maxShakeAmount > 0) maxShakeAmount -= 0.5f;

		// �V�F�C�N�ʂ��X�V����B
		//shakeAmount.x = GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		//shakeAmount.y = GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		shakeAmount.x = KuroFunc::GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		shakeAmount.y = KuroFunc::GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;

		//shakeAmount = { 0.0f,0.0f };
	}

	// �V�F�C�N�ʂ��Z�b�g����B
	inline void SetShake(const int& set) {

		if (maxShakeAmount < set) {

			maxShakeAmount = set;

		}

	}


};