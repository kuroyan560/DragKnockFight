#include "Vec.h"
#include <vector>
#include "Intersected.h"

using namespace std;

// �v���C���[�ƈ������荇���{�X�N���X
class Boss {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;
	Vec2<float> prevPos;
	Vec2<float> scale;
	Vec2<float> vel;
	INTERSECTED_LINE prevIntersectedLine;


public:

	/*===== �萔 =====*/

	const Vec2<float> SCALE = { 50.0f,50.0f };
	const float OFFSET_VEL = 10.0f;


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