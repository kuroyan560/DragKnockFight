#pragma once
#include "Singleton.h"
#include "KuroEngine.h"

class RenderTarget;
class GaussianBlur;

// GameScene�̍Ō�̉摜���ڂ₩����ResultScene�ŕ`�悷�邽�߂̃N���X
class ResultSceneBackGround : public Singleton<ResultSceneBackGround> {

	friend class Singleton<ResultSceneBackGround>;

public:

	/*===== �����o�ϐ� =====*/

	std::shared_ptr<GaussianBlur>gaussianBlur;
	std::shared_ptr<TextureBuffer>backGround;

	void Init();
	void Draw();

};