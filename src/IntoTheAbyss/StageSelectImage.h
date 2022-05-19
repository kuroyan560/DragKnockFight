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
	void Init();
	void Update();
	void Draw();

	void ImGuiDraw();

	inline const bool& GetZoomOutFlag() { return zoomOutFlag; }
	inline void SetZoomFlag(const bool& Zoom) { zoomOutFlag = Zoom; }

	// ���o�p�̊g�k���Z�b�g�B
	inline void SetExp(const Vec2<float>& expPos, const Vec2<float>& expRate) { expData.Init(expPos, expRate); };

private:

	// �w�i
	int backGroundHandle;

	// �w�i�摜���g�k�����蓮�������肷�邽�߂̕ϐ�
	LerpData backGroundLerpData;

	// �I�����ꂽ���Ɋg�k���邽�߂̕ϐ�
	LerpData expData;

	// �Y�[���A�E�g���邩�ǂ����̃t���O true��������Y�[������B
	bool zoomOutFlag;

	//StageSelectScreenShot screenShot;
};

