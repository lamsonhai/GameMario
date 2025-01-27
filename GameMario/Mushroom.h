#pragma once
#include "GameObject.h"
#include "Collision.h"

#define MUSHROOM_BBOX_WIDTH 16
#define MUSHROOM_BBOX_HEIGHT 16

#define MUSHROOM_SPEED_Y	0.015f
#define MUSHROOM_SPEED_X	0.035f
#define MUSHROOM_GRAVITY	0.0006f

#define MUSHROOM_STATE_HIDEN	0
#define EFFECT_STATE	1
#define MUSHROOM_STATE_NORMAL	2

#define MUSHROOM_EFFECT_TIME		1010
#define MUSHROOM_EFFECT_MOVE_TIME	200

#define MUSHROOM_RED_ANI		0

class CMushroom : public CollisionObject
{
	int effect_state;
	int hiden_state;
	int intro_state;
	DWORD create_time;
	DWORD effect_time_start;
public:
	CMushroom();
	void SetState(int state);
	int IsHidenState() { return hiden_state; }
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void CreateIntroAnimationMushroom();
	void SetIntroState(int s) { intro_state = s; }

	virtual void FilterCollision(
		vector<LPCOLLISIONEVENT>& coEvents,
		vector<LPCOLLISIONEVENT>& coEventsResult,
		float& min_tx,
		float& min_ty,
		float& nx,
		float& ny,
		float& rdx,
		float& rdy);
};

