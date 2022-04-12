#pragma once
#include "Singleton.h"
#include "KuroEngine.h"

class RenderTarget;
class GaussianBlur;

// GameScene‚ÌÅŒã‚Ì‰æ‘œ‚ğ‚Ú‚â‚©‚µ‚ÄResultScene‚Å•`‰æ‚·‚é‚½‚ß‚ÌƒNƒ‰ƒX
class ResultSceneBackGround : public Singleton<ResultSceneBackGround> {

	friend class Singleton<ResultSceneBackGround>;

public:

	/*===== ƒƒ“ƒo•Ï” =====*/

	std::shared_ptr<GaussianBlur>gaussianBlur;
	std::shared_ptr<TextureBuffer>backGround;

	void Init();
	void Draw();

};