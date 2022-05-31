#include "BackGroundParticle.h"
#include "KuroMath.h"
#include "KuroFunc.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"
#include "CharacterManager.h"
#include "DistanceCounter.h"

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

	// �e�ϐ����������B
	pos = Pos;
	stageSize = StageSize;
	exp = {};
	angle = {};
	isActive = true;
	timer = 0;
	desTimer = 0;
	status = Status;
	starIndex = KuroFunc::GetRand(0, 4);

	// ���ł���܂ł̃^�C�}�[�𐯂��ƂɃ����_���ɐݒ�B
	defDesTimer = KuroFunc::GetRand(MAX_DES_TIMER / 2, MAX_DES_TIMER);

	// �f�t�H���g�̃T�C�Y��ݒ�B
	float sizeBuff = KuroFunc::GetRand(1.0f, 3.0f);
	targetExp = { sizeBuff,sizeBuff };

	// �X�e�[�^�X�����ꐯ��������B
	if (status == STATUS::SHOOTING_STAR) {

		// ���ꐯ�̏ꍇ�͏��ł���܂ł̃^�C�}�[�����߂Đݒ肷��B
		defDesTimer = KuroFunc::GetRand(MAX_DES_TIMER / 4, MAX_DES_TIMER);

		// �ړ����x�������_���ɐݒ�B
		speed = KuroFunc::GetRand(MIN_SPEED, MAX_SPEED);

		// ���ꐯ�̏ꍇ�̓f�t�H���g�̃T�C�Y���Œ肷��B
		targetExp = { 3.0f,3.0f };

	}
	// �X�e�[�^�X�������Q��������
	else if (status == STATUS::METEOR_SWARM) {

		// �X�s�[�h���Ē���
		speed = KuroFunc::GetRand(MAX_SPEED, MAX_SPEED + 30.0f);

		// ���ꐯ�̏ꍇ�͏��ł���܂ł̃^�C�}�[�����߂Đݒ肷��B
		defDesTimer = KuroFunc::GetRand(MAX_DES_TIMER / 4, MAX_DES_TIMER / 2);

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
			generatePos.x = KuroFunc::GetRand(-stageSize.x * 2.0f, stageSize.x * 2.0f + stageSize.x);
			generatePos.y = KuroFunc::GetRand(-stageSize.y * 2.0f, stageSize.y * 2.0f + stageSize.y);

			generatePos += DistanceCounter::Instance()->lineCenterPos;

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
	case BackGroundParticle::STATUS::METEOR_SWARM:

		/*===== ���ꐯ =====*/

		// ������܂ł̃^�C�}�[���X�V
		++desTimer;
		if (defDesTimer < desTimer) {

			targetExp = {};

			// �傫�������ȉ��ɂȂ����珉�����B
			if (exp.x < 0.01f) {
				Init();
			}

		}

		// �傫�����f�t�H���g�ɋ߂Â���B
		exp.x += (targetExp.x - exp.x) / 5.0f;
		exp.y += (targetExp.y - exp.y) / 5.0f;

		// �ړ�������B
		pos += Vec2<float>(-1, 1) * speed;

		// ��]������B
		angle += 0.2f;


		break;
	default:
		break;
	}

}

#include "GameSceneCamerMove.h"

void BackGroundParticle::Draw()
{

	/*===== �`�揈�� =====*/

	static int firstOne = 0;
	static std::array<int, 5> STAR_GRAPH;

	if (firstOne == 0) {

		TexHandleMgr::LoadDivGraph("resource/ChainCombat/back_star.png", 5, Vec2<int>(5, 1), STAR_GRAPH.data());

		++firstOne;

	}

	// �`�悷�郊�T�[�W���Ȑ��̂��������߂�B
	float lissajousMove = 50.0f;
	Vec2<float> lissajousCurve = Vec2<float>(cosf(1.0f * lissajousTimer) * lissajousMove, sinf(2.0f * lissajousTimer) * lissajousMove);

	// ���݂̃Y�[����
	float zoom = ScrollMgr::Instance()->zoom;


	auto buff = TexHandleMgr::GetTexBuffer(STAR_GRAPH[starIndex])->GetGraphSize();

	// �ʏ킾������
	if (status == STATUS::STAY) {

		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos + lissajousCurve) + GameSceneCameraMove::Instance()->move, exp * Vec2<float>(zoom, zoom), angle, TexHandleMgr::GetTexBuffer(STAR_GRAPH[starIndex]));

	}
	// ���ꐯ��������
	else {

		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos) + GameSceneCameraMove::Instance()->move, exp * Vec2<float>(zoom, zoom), angle, TexHandleMgr::GetTexBuffer(STAR_GRAPH[starIndex]));

	}

}

void BackGroundParticle::CheckHit(const Vec2<float>& Pos, const float& Size)
{

	/*===== �����蔻�� =====*/

	// �`�悷�郊�T�[�W���Ȑ��̂��������߂�B
	float lissajousMove = 50.0f;
	Vec2<float> lissajousCurve = Vec2<float>(cosf(1.0f * lissajousTimer) * lissajousMove, sinf(2.0f * lissajousTimer) * lissajousMove);

	float distance = (pos + lissajousCurve - Pos).Length();
	float charaR = Size + 70.0f;
	if (distance <= charaR) {

		// �L�������琯�ւ̕���
		Vec2<float> charaDir = (pos + lissajousCurve - Pos).GetNormal();

		pos = pos + lissajousCurve + charaDir * (charaR - distance);

		pos -= lissajousCurve;

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

	for (auto& index : stayStar) {

		index.Init();

	}

	for (auto& index : shootingStar) {

		index.Init();

	}

}

void BackGroundParticleMgr::StageStartGenerate(const Vec2<float>& StageSize)
{

	/*===== �������� =====*/

	int counter = 0;

	stageSize = StageSize;

	// �X�e�[�W�T�C�Y��1000�ȉ���������1000�ɂ���B
	if (stageSize.x < 4000) stageSize.x = 4000;
	if (stageSize.y < 4000) stageSize.y = 4000;

	float mixStageSize = stageSize.x + stageSize.y;

	// �X�e�[�W�T�C�Y���琶�����鐯�̐������߂�B
	int generateCount = mixStageSize / 20.0f;

	for (auto& index : stayStar) {

		if (index.isActive) continue;

		Vec2<float> generatePos;
		generatePos.x = KuroFunc::GetRand(-stageSize.x * 2.0f, stageSize.x * 2.0f + stageSize.x);
		generatePos.y = KuroFunc::GetRand(-stageSize.y * 2.0f, stageSize.y * 2.0f + stageSize.y);

		index.Generate(generatePos, BackGroundParticle::STATUS::STAY, stageSize);

		++counter;
		if (generateCount < counter) break;

	}

	// �ϐ����������B
	meterSwarmTimer = 0;
	meterSwarmMode = false;

}

void BackGroundParticleMgr::Update()
{

	/*===== �X�V���� =====*/

	for (auto& index : stayStar) {

		if (!index.isActive) continue;

		index.Update();

	}
	for (auto& index : shootingStar) {

		if (!index.isActive) continue;

		index.Update();

	}

	// ���ꐯ�𐶐�����^�C�}�[���X�V�B
	++shootingStarGenerateTimer;
	if (defShootingStarGenerateTimer < shootingStarGenerateTimer) {

		shootingStarGenerateTimer = 0;

		// ����������܂ł̃^�C�}�[�������_���Ɍ��߂�B
		defShootingStarGenerateTimer = KuroFunc::GetRand(0, 30);

		for (auto& index : shootingStar) {

			if (index.isActive) continue;

			const float SHOOTING_STAR_OFFSET = 5000.0f;

			Vec2<float> generatePos = DistanceCounter::Instance()->lineCenterPos;;
			generatePos.x += KuroFunc::GetRand(-SHOOTING_STAR_OFFSET, SHOOTING_STAR_OFFSET);
			generatePos.y += KuroFunc::GetRand(-SHOOTING_STAR_OFFSET, SHOOTING_STAR_OFFSET);

			index.Generate(generatePos, BackGroundParticle::STATUS::SHOOTING_STAR, stageSize);

			break;

		}


	}


	// ���ɋH�ɗ����Q���o���B
	int meteorSwarm = KuroFunc::GetRand(0, 1000);
	if (meteorSwarm == 50 && !meterSwarmMode) {

		meterSwarmMode = true;
		meterSwarmTimer = 0;

		const float METER_SWARM_OFFSET = 1000.0f;

		// �����Q���o���ꏊ
		meterSwarmPos = DistanceCounter::Instance()->lineCenterPos;
		meterSwarmPos.x += METER_SWARM_OFFSET;
		meterSwarmPos.y -= METER_SWARM_OFFSET;

	}
	else {

		// �����Q��Ԃ������珉�������Ȃ��B
		if (!meterSwarmMode) {
			meterSwarmMode = false;
			meterSwarmTimer = 0;
		}

	}

	// �����Q��Ԃ�������B
	if (meterSwarmMode) {

		// ���Ԋu�ŗ����Q�𐶐�����B
		if (meterSwarmTimer % METER_SWARM_SPAN == 0) {

			// �����Q�𐶐�����ꏊ���炳��ɂ�����x���炷�B
			float meterSwarmSize = 2000.0f;
			Vec2<float> generatePos = meterSwarmPos;
			generatePos.x += KuroFunc::GetRand(-meterSwarmSize, meterSwarmSize);
			generatePos.y += KuroFunc::GetRand(-meterSwarmSize, meterSwarmSize);

			// �����Q�𐶐�����B
			for (auto& index : shootingStar) {

				if (index.isActive) continue;

				index.Generate(generatePos, BackGroundParticle::STATUS::METEOR_SWARM, stageSize);

				break;

			}

		}

		// �^�C�}�[���X�V���ď������B
		++meterSwarmTimer;
		if (METER_SWARM_TIMER <= meterSwarmTimer) {

			meterSwarmMode = false;
			meterSwarmTimer = 0;

		}

	}


	// �p�[�e�B�N���̃��T�[�W���Ȑ��̓������X�V�B static�ɂ���������̂ł����ōX�V���܂��B
	BackGroundParticle::lissajousTimer += 0.01f;

}

void BackGroundParticleMgr::Draw()
{

	/*===== �`�揈�� =====*/

	for (auto& index : stayStar) {

		if (!index.isActive) continue;

		index.Draw();

	}

	for (auto& index : shootingStar) {

		if (!index.isActive) continue;

		index.Draw();

	}

}

void BackGroundParticleMgr::CheckHit(const Vec2<float>& Pos, const float& Size)
{

	/*===== �����蔻�� =====*/

	for (auto& index : stayStar) {

		if (!index.isActive) continue;

		index.CheckHit(Pos, Size);

	}

}
