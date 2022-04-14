#pragma once
#include "Singleton.h"
#include "KuroEngine.h"

class RenderTarget;
class GaussianBlur;

// GameScene�ł̏������U���g�V�[���֎󂯓n�����߂̃N���X
class ResultTransfer : public Singleton<ResultTransfer> {

	friend class Singleton<ResultTransfer>;
	ResultTransfer();

public:

	/*===== �����o�ϐ� =====*/

	std::shared_ptr<GaussianBlur>gaussianBlur;
	std::shared_ptr<RenderTarget>backGround;

	//�ŏI�I�ȃX�R�A
	int resultScore;

	//Break�̉�
	int leftBreakCount;
	int rightBreakCount;

	void Init();
	void Draw();

};