#include "Brick.h"
#include "Utils.h"

void CBrick::Render()
{
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
	DebugOut(L"[INFO] Render brick successfully! \n");
}

void CBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BRICK_BBOX_WIDTH;
	b = y + BRICK_BBOX_HEIGHT;
}