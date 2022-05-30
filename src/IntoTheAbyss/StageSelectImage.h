#pragma once
#include"../KuroEngine.h"
#include"IStageSelectImage.h"
#include"StageSelectScreenShot.h"

/// <summary>
/// �X�e�[�W�I����ʂ̉摜
/// </summary>
class StageSelectImage
{
public:
	StageSelectImage();
	void Init(bool MOVE_FROM_GAME_FLAG);
	void Update();
	void Draw();

	void ImGuiDraw();

	inline const bool& GetZoomOutFlag() { return zoomOutFlag; }
	void SetZoomFlag(const bool& Zoom);

	// ���o�p�̊g�k���Z�b�g�B
	inline void SetExp(const Vec2<float>& expPos, const Vec2<float>& expRate) {
		expData.pos = expPos;
		expData.size = expRate;
		expData.timer = 0.0f;
	};

	inline LerpData& GetLerpData() { return backGroundLerpData; }

private:

	// �w�i
	int backGroundHandle;

	// �w�i�摜���g�k�����蓮�������肷�邽�߂̕ϐ�
	LerpData backGroundLerpData;

	// �I�����ꂽ���Ɋg�k���邽�߂̕ϐ�
	LerpData expData;

	// �Y�[���A�E�g���邩�ǂ����̃t���O true��������Y�[������B
	bool zoomOutFlag;

	//StageSelectScreenShot screenShot;[

	Vec2<float>startStageSelectSize;//�Q�[����ʂ���X�e�[�W�I����ʂɓ���ۂ̃Y�[������
};

