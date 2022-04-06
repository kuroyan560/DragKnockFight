#include "Vec.h"
#include <vector>
#include "Intersected.h"

using namespace std;

// �v���C���[�ƈ������荇���{�X�N���X
class Boss {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;			// ���W
	Vec2<float> prevPos;		// �O�t���[���̍��W
	Vec2<float> scale;			// �傫��
	Vec2<float> vel;			// �ړ���
	int stuckWindowTimer;		// �E�B���h�E�ɋ��܂����^�C�}�[
	INTERSECTED_LINE prevIntersectedLine;


public:

	/*===== �萔 =====*/

	const Vec2<float> SCALE = { 50.0f,50.0f };
	const float OFFSET_VEL = 10.0f;
	const int STRUCK_WINDOW_TIMER = 120.0f;


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

	// �����蔻��
	void CheckHit(const vector<vector<int>>& mapData, bool& isHitMapChip, const Vec2<float>& playerPos);

};