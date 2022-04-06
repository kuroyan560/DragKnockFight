#include "Vec.h"

// �v���C���[�ƈ������荇���{�X�N���X
class Boss {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;
	Vec2<float> scale;
	Vec2<float> forwardVec;
	float vel;


public:

	/*===== �萔 =====*/

	const Vec2<float> SCALE = { 50.0f,50.0f };
	const float OFFSET_VEL = 5.0f;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	Boss();

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& generatePos);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

};