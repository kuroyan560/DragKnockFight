#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <memory>
#include <vector>
using namespace std;
class MovingBlock;

// ���I�u���b�N�Ǘ��N���X
class MovingBlockMgr : public Singleton<MovingBlockMgr> {

public:

	/*===== �����o�ϐ� =====*/

	vector<shared_ptr<MovingBlock>> movingBlock;	// ���I�u���b�N


public:

	/*===== �����o�֐� =====*/

	// ���I�u���b�N�̃��[�h
	void Load(const vector<vector<int>>& mapData);

	// ����������
	void Init();

	// �X�V����
	void Update(const Vec2<float>& playerPos);

	// �`�揈��
	void Draw(const int& graphHandle);

	// �}�b�v�`�b�v�Ƃ̓����蔻��
	void CheckHit(const vector<vector<int>>& mapData);

};