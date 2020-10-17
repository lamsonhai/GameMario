#pragma once
#include "GameObject.h"

#define WHITE_BRICK 0
#define GREEN_BRICK 1
#define PINK_BRICK 2
#define BLUE_BRICK 3

#define COLOR_BRICK_BBOX_HEIGHT 16
#define COLOR_BRICK_BBOX_WIDTH	16

class CColorBrick : public CGameObject
{
	int w;
	int h;

public:
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void renderBlock(int color, int w, int h, int x, int y);
};
