#include "EventBlock.h"
#include"StageMgr.h"
#include"EventCollider.h"
#include"DrawFunc.h"
#include"TexHandleMgr.h"

int EventBlock::arrayNum = 0;
bool EventBlock::initLoadGraphFlag = false;

EventBlock::EventBlock() :initFlag(false)
{
	if (!initLoadGraphFlag)
	{
		//TexHandleMgr::LoadDivGraph("resource/.png", 10, Vec2<int>(50, 50), graphHandle.data());
		initLoadGraphFlag = true;
	}
	handle = arrayNum;
	++arrayNum;
}

void EventBlock::Init(const Vec2<float> &POS)
{
	pos = POS;
	initFlag = true;
}

void EventBlock::Finalize()
{
	initFlag = false;
}

bool EventBlock::HitBox(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE, const Vec2<float> &PLAYER_VEL, const Vec2<float> &PLAYER_PREV_POS)
{
	//初期化していないブロックはエラー処理を作る
	if (!initFlag)
	{
		//SizeData tmp = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_EVENT);
		//int nowHandle = tmp.min + handle;
		//std::string handleString = "イベントチップ" + std::to_string(handle) + "(チップ番号" + std::to_string(nowHandle) + ")" + "を配置していないのにもかかわらずギミック側で使おうとしています。\n設置をお願いします";
		//MessageBox(NULL, KuroFunc::GetWideStrFromStr(handleString).c_str(), TEXT("イベントチップ被り"), MB_OK);
		//assert(0);
		return false;
	}
	bool isDossunVel = EventCpllider::Instance()->CheckHitVel(PLAYER_POS, PLAYER_PREV_POS, PLAYER_VEL, SIZE, pos, { 50.0f,50.0f }) != INTERSECTED_NONE;
	bool topHitFlag = EventCpllider::Instance()->CheckHitSize(pos, { 50.0f,50.0f }, PLAYER_POS, SIZE, INTERSECTED_TOP);
	bool buttomFlag = EventCpllider::Instance()->CheckHitSize(pos, { 50.0f,50.0f }, PLAYER_POS, SIZE, INTERSECTED_BOTTOM);
	bool leftHitFlag = EventCpllider::Instance()->CheckHitSize(pos, { 50.0f,50.0f }, PLAYER_POS, SIZE, INTERSECTED_LEFT);
	bool rightHitFlag = EventCpllider::Instance()->CheckHitSize(pos, { 50.0f,50.0f }, PLAYER_POS, SIZE, INTERSECTED_RIGHT);

	if (topHitFlag || buttomFlag || leftHitFlag || rightHitFlag)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void EventBlock::Draw()
{
	//デバック用の画像
	//DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(1.0f, 1.0f), 0.0f, {});
}
