#include "Vec.h"
#include "Singleton.h"
#include "Color.h"
#include <array>

// �c�����o
class AfterImageMgr : public Singleton<AfterImageMgr> {

private:

	struct AfterImage {

		/*===== �����o�ϐ� =====*/

		Vec2<float> pos;	// �`����W
		Vec2<float> extRate;// �傫��
		Vec2<float> size;	// DrawExtendGraph�Ŏg�p����`��T�C�Y
		Color srcColor;		// �킹��F
		int handle;			// �g�p�摜�n���h��
		float alpha;		// �A���t�@�l
		float radian;		// �p�x
		bool isActive;		// �A�N�e�B�u�t���O
		bool isExtendGraph;	// DrawExtendGraph���g���t���O

		// �R���X�g���N�^
		AfterImage() :pos({}), handle(0), alpha(0), extRate({}), radian(0), isActive(false) {};

	};


private:

	/*===== �����o�ϐ� =====*/

	static const int AFTERIMAGE_COUNT = 50;
	std::array<AfterImage, AFTERIMAGE_COUNT> afterImages;
	int delay;				// �c���𐶐�����x��
	const int DELAY = 2;	// �c���𐶐�����x��


public:

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& Pos, const Vec2<float>& ExtRate, const float& Radian, const int& Handle, const Color& SrcColor, const bool& IsExtendGraph = false, const Vec2<float>& size = {});

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

};