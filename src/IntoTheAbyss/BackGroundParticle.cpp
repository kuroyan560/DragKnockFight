#include "BackGroundParticle.h"
#include "KuroMath.h"
#include "KuroFunc.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"

float BackGroundParticle::lissajousTimer = 0;

BackGroundParticle::BackGroundParticle()
{

	/*===== �R���X�g���N�^ =====*/

	pos = {};
	exp = {};
	angle = {};
	isActive = false;
	timer = 0;

}

void BackGroundParticle::Init()
{

	/*===== ���������� =====*/

	pos = {};
	exp = {};
	angle = {};
	isActive = false;
	timer = 0;
	desTimer = 0;

}

void BackGroundParticle::Generate(const Vec2<float>& Pos, const STATUS& Status, const Vec2<float>& StageSize)
{

	/*===== ���������� =====*/

	pos = Pos;
	stageSize = StageSize;
	exp = {};
	angle = {};
	isActive = true;
	targetExp = { 1.0f,1.0f };
	timer = 0;
	desTimer = 0;
	defDesTimer = KuroFunc::GetRand(MAX_DES_TIMER / 2, MAX_DES_TIMER);
	status = Status;

	// �X�e�[�^�X�����ꐯ��������B
	if (status == STATUS::SHOOTING_STAR) {

		defDesTimer = KuroFunc::GetRand(MAX_DES_TIMER / 4, MAX_DES_TIMER);
		speed = KuroFunc::GetRand(MIN_SPEED, MAX_SPEED);

	}

}

void BackGroundParticle::Update()
{

	/*===== �X�V���� =====*/

	static const int INIT_TIMER = 100000;	// �e�^�C�}�[���f�J���Ȃ�߂����珉��������p


	// �F�X�֗��Ɏg����^�C�}�[
	++timer;
	if (INIT_TIMER < timer) timer = 0;

	switch (status)
	{
	case BackGroundParticle::STATUS::STAY:

		/*===== ���̏�ɂƂǂ܂��� =====*/

		// ���T�[�W���Ȑ��Ɏg�p����^�C�}�[���X�V�B
		if (INIT_TIMER < lissajousTimer) lissajousTimer = 0;

		// �^�C�}�[������l�ɒB�������]������B
		if (timer % 180 == 0) {

			angle = DirectX::XM_2PI * 10.0f;

		}

		// �Đ�������܂ł̃^�C�}�[���X�V�B
		++desTimer;
		if (defDesTimer < desTimer) {

			Vec2<float> generatePos = {};
			generatePos.x = KuroFunc::GetRand(-stageSize.x, stageSize.x * 4.0f);
			generatePos.y = KuroFunc::GetRand(-stageSize.y, stageSize.y * 2.0f);

			Generate(generatePos, STATUS::STAY, stageSize);

		}
		// �Đ�������܂ł̃^�C�}�[�ɂ�����x�߂Â�����T�C�Y������������B
		if (defDesTimer - 5.0f < desTimer) {

			targetExp = {};

		}

		// �傫�����f�t�H���g�ɋ߂Â���B
		exp.x += (targetExp.x - exp.x) / 5.0f;
		exp.y += (targetExp.y - exp.y) / 5.0f;
		angle -= angle / 5.0f;

		break;
	case BackGroundParticle::STATUS::SHOOTING_STAR:

		/*===== ���ꐯ =====*/

		// ������܂ł̃^�C�}�[���X�V
		++desTimer;
		if (defDesTimer < desTimer) {

			targetExp = {};

			// �傫�������ȉ��ɂȂ����珉�����B
			if (exp.x < 0.01f) Init();

		}

		// �傫�����f�t�H���g�ɋ߂Â���B
		exp.x += (targetExp.x - exp.x) / 5.0f;
		exp.y += (targetExp.y - exp.y) / 5.0f;

		// �ړ�������B
		pos += Vec2<float>(-1, 1) * speed;

		// ��]������B
		angle += 0.1f;


		break;
	default:
		break;
	}

}

void BackGroundParticle::Draw()
{

	/*===== �`�揈�� =====*/

	static const int STAR_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/star.png");

	// �`�悷�郊�T�[�W���Ȑ��̂��������߂�B
	float lissajousMove = 50.0f;
	Vec2<float> lissajousCurve = Vec2<float>(cosf(1.0f * lissajousTimer) * lissajousMove, sinf(2.0f * lissajousTimer) * lissajousMove);

	// ���݂̃Y�[����
	float zoom = ScrollMgr::Instance()->zoom;

	// �ʏ킾������
	if (status == STATUS::STAY) {

		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos + lissajousCurve), exp * Vec2<float>(zoom, zoom), angle, TexHandleMgr::GetTexBuffer(STAR_GRAPH));

	}
	// ���ꐯ��������
	else {

		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), exp * Vec2<float>(zoom, zoom), angle, TexHandleMgr::GetTexBuffer(STAR_GRAPH));

	}

}

BackGroundParticleMgr::BackGroundParticleMgr()
{

	defShootingStarGenerateTimer = 0;
	shootingStarGenerateTimer = 0;

}

void BackGroundParticleMgr::Init()
{

	/*===== ���������� =====*/

	for (auto& index : particles) {

		index.Init();

	}

}

void BackGroundParticleMgr::StageStartGenerate(const Vec2<float>& StageSize)
{

	/*===== �������� =====*/

	int generateCount = 150;
	int counter = 0;

	stageSize = StageSize;

	for (auto& index : particles) {

		if (index.isActive) continue;

		Vec2<float> generatePos;
		generatePos.x = KuroFunc::GetRand(-stageSize.x, stageSize.x * 4.0f);
		generatePos.y = KuroFunc::GetRand(-stageSize.y, stageSize.y * 2.0f);

		index.Generate(generatePos, BackGroundParticle::STATUS::STAY, StageSize);

		++counter;
		if (generateCount < counter) break;

	}

}

void BackGroundParticleMgr::Update()
{

	/*===== �X�V���� =====*/

	for (auto& index : particles) {

		if (!index.isActive) continue;

		index.Update();

	}

	// ���ꐯ�𐶐�����^�C�}�[���X�V�B
	++shootingStarGenerateTimer;
	if (defShootingStarGenerateTimer < shootingStarGenerateTimer) {

		shootingStarGenerateTimer = 0;

		// ����������܂ł̃^�C�}�[�������_���Ɍ��߂�B
		defShootingStarGenerateTimer = KuroFunc::GetRand(0, 180);

		for (auto& index : particles) {

			if (index.isActive) continue;

			Vec2<float> generatePos;
			generatePos.x = KuroFunc::GetRand(-stageSize.x, stageSize.x * 4.0f) + stageSize.x;
			generatePos.y = KuroFunc::GetRand(-stageSize.y, stageSize.y * 2.0f) - stageSize.y;

			index.Generate(generatePos, BackGroundParticle::STATUS::SHOOTING_STAR, stageSize);

			break;

		}


	}


	// �p�[�e�B�N���̃��T�[�W���Ȑ��̓������X�V�B static�ɂ���������̂ł����ōX�V���܂��B
	BackGroundParticle::lissajousTimer += 0.01f;

}

void BackGroundParticleMgr::Draw()
{

	/*===== �`�揈�� =====*/

	for (auto& index : particles) {

		if (!index.isActive) continue;

		index.Draw();

	}

}
