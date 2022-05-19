#pragma once
#include"../KuroEngine.h"
#include"CharacterManager.h"
#include"IStageSelectImage.h"

class StageSelectCharacterImage
{
public:
	StageSelectCharacterImage();
	void Init(const Vec2<float>& ZoomInPos, const Vec2<float>& ZoomOutPos, const int& GraphHandle);
	void Update();
	void Draw();

	inline void SetIsZoomOut(const bool& Zoom) {
		isZoomOut = Zoom;
		pos.timer = 0.0f;
	}

private:

	/*===== �����o�ϐ� =====*/

	LerpData zoomInPos;		// �Y�[���C�����Ă���Ƃ��̍��W
	LerpData zoomOutPos;	// �Y�[���A�E�g���Ă���Ƃ��̍��W
	LerpData pos;			// �`����

	bool isZoomOut;			// �Y�[���A�E�g���Ă���̃t���O

	int graphHandle;		// �g�p����摜�̃n���h��

};

