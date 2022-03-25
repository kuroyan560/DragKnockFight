#include "EventBlock.h"

EventBlock::EventBlock() :initFlag(false)
{
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

bool EventBlock::HitBox(const Vec2<float> &PLAYER_POS)
{
	//‰Šú‰»‚µ‚Ä‚¢‚È‚¢ƒuƒƒbƒN‚Í“–‚½‚è”»’è‚Í•Ô‚³‚È‚¢
	if (initFlag)
	{
		return false;
	}


	return false;
}
