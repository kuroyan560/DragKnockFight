#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <vector>

using namespace std;

// �f�[�^�N���X
struct SightCollisionData {

	Vec2<float> pos;		// ���W�̃|�C���^
	Vec2<float> scale;		// �傫��

	SightCollisionData(Vec2<float>& pos, const Vec2<float>& scale) {

		this->pos = pos;
		this->scale = scale;

	}
	SightCollisionData() {

		this->pos = {};
		this->scale = {};

	}

};

// �Ə��̔���Ɏg�p����f�[�^��ۑ�����N���X
class SightCollisionStorage : public Singleton<SightCollisionStorage> {

public:

	/*===== �����o�ϐ� =====*/

	vector<SightCollisionData*> data;		// �����̓����蔻��Ɏg�p����f�[�^�̔z��B


public:

	/*===== �����o�֐� =====*/

	// �Z�b�g���鏈���B
	inline void SetData(Vec2<float>* pos, const Vec2<float>& scale) {

		// nullptr�������珈�����΂��B
		//if (pos == nullptr) return;

		//// �f�[�^��ǉ��B
		//data.push_back(SightCollisionData(pos, scale));

	}


};