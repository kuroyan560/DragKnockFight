#include "Vec.h"
#include "Singleton.h"
#include <array>

// �c�����o
class AfterImageMgr : public Singleton<AfterImageMgr> {

private:

	struct AfterImage {

		/*===== �����o�ϐ� =====*/

		Vec2<float> pos;	// �`����W
		Vec2<float> extRate;// �傫��
		int srcHandle;		// �g�p�摜�n���h��
		int destHandle;		// �킹��悤�摜�n���h��
		float alpha;		// �A���t�@�l
		float radian;		// �p�x
		bool isActive;		// �A�N�e�B�u�t���O

		// �R���X�g���N�^
		AfterImage() :pos({}), srcHandle(0), destHandle(0), alpha(0), extRate({}), radian(0), isActive(false) {};

	};


private:

	/*===== �����o�ϐ� =====*/

	static const int AFTERIMAGE_COUNT = 50;
	std::array<AfterImage, AFTERIMAGE_COUNT> afterImages;
	int delay;				// �c���𐶐�����x��
	const int DELAY = 3;	// �c���𐶐�����x��


public:

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& Pos, const Vec2<float>& ExtRate, const float& Radian, const int& SrcHandle, const int& DestHandle);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

};