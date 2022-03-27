#include "EventBlock.h"
#include"StageMgr.h"
#include"EventCollider.h"
#include"DrawFunc.h"
#include"TexHandleMgr.h"

bool EventBlock::initLoadGraphFlag = false;
std::array<int, 10> EventBlock::graphHandle = {};

EventBlock::EventBlock() :initFlag(false)
{
	if (!initLoadGraphFlag)
	{
		//TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/EventBlocks.png", 10, Vec2<int>(10, 1), graphHandle.data());
		initLoadGraphFlag = true;
	}
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
	//Vec2<float> hitBoxLeftUpPos = leftUpPos;
	//Vec2<float> hitBoxCentralPos = (leftUpPos + size / 2.0f);

	//bool isDossunVel = EventCpllider::Instance()->CheckHitVel(PLAYER_POS, PLAYER_PREV_POS, PLAYER_VEL, SIZE, hitBoxCentralPos, size / 2.0f) != INTERSECTED_NONE;
	//bool topHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, size / 2.0f, INTERSECTED_TOP) != INTERSECTED_NONE;
	//bool buttomFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, size / 2.0f, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
	//bool leftHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, size / 2.0f, INTERSECTED_LEFT) != INTERSECTED_NONE;
	//bool rightHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, size / 2.0f, INTERSECTED_RIGHT) != INTERSECTED_NONE;

	//if (isDossunVel || topHitFlag || buttomFlag || leftHitFlag || rightHitFlag)
	//{
	//	color = Color(255, 0, 0, 255);
	//	return true;
	//}
	//else
	//{
	//	color = Color(255, 255, 255, 255);
	//	return false;
	//}
}

void EventBlock::Draw()
{
	//デバック用の画像
	//DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(graphHandle[handle]));
}
