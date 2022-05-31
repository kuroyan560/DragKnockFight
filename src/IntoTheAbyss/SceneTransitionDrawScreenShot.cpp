#include "SceneTransitionDrawScreenShot.h"
#include"../Engine/DrawFunc.h"
#include"StageSelectTransfer.h"
#include"ClearInfoContainer.h"
#include"SelectStage.h"

SceneTransitionDrawScreenShot::SceneTransitionDrawScreenShot()
{
	screenDraw.transform.SetPos(Vec2<float>(0.0f, 0.0f));

	alpha = 0.0f;
	startFlag = false;
}

SceneTransitionDrawScreenShot::~SceneTransitionDrawScreenShot()
{
}

void SceneTransitionDrawScreenShot::OnStart()
{

	if (!startFlag)
	{
		alpha = 0.0f;
		screenDraw.SetTexture(StageSelectTransfer::Instance()->screenShot);
		screenDraw.transform.SetScale(Vec2<float>(20.0f, 11.0f));
		screenDraw.SetColor(Color(255, 255, 255, 0));
		startFlag = true;
	}
}

bool SceneTransitionDrawScreenShot::OnUpdate()
{
	if (startFlag)
	{
		screenDraw.SetColor(Color(255, 255, 255, alpha));
		alpha += 255.0f / 60.0f;
		if (255.0f <= alpha)
		{
			startFlag = false;
			return true;
		}
	}
	return false;
}

void SceneTransitionDrawScreenShot::OnDraw()
{
	if (startFlag)
	{
		screenDraw.Draw();
	}
}
