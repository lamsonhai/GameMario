#pragma once
#include "GameObject.h"
#include "Collision.h"

#define GOOMBA_WALKING_SPEED		0.03f;
#define GOONBA_JUMP_DEFLECT_SPEED	0.2f
#define GOONBA_JUMP_SPEED			0.08f

#define GOOMBA_BBOX_WIDTH			16
#define GOOMBA_BBOX_HEIGHT			15

#define PARAGOOMBA_BBOX_WIDTH		20
#define PARAGOOMBA_BBOX_HEIGHT		18

#define PARAGOOMBA_JUMPING_BBOX_WIDTH		20
#define PARAGOOMBA_JUMPING_BBOX_HEIGHT		24

#define GOOMBA_BBOX_HEIGHT_DIE		9

#define GOOMBA_YELLOW_FORM			0
#define GOOMBA_BROWN_FORM			1
#define PARAGOOMBA_BROWN_FORM		2

#define GOOMBA_STATE_WALKING		100
#define GOOMBA_STATE_DIE			200
#define GOOMBA_STATE_THROWN			300
#define GOOMBA_STATE_JUMPING		400

#define GOOMBA_ANI_YELLOW_WALKING			0
#define GOOMBA_ANI_BROWN_WALKING			1
#define PARAGOOMBA_ANI_BROWN_JUMPING		2
#define PARAGOOMBA_ANI_BROWN_WALKING		3
#define GOOMBA_ANI_YELLOW_DIE				4
#define GOOMBA_ANI_BROWN_DIE				5
#define GOOMBA_ANI_THROWN					0

#define GOOMBA_GRAVITY				0.001f

class CGoomba : public CGameObject
{
	bool isDestroy = false;
	int jump_state;

	DWORD time_start;
	DWORD timeDestroy = 0;
	int form;
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void FilterCollision(
		vector<LPCOLLISIONEVENT>& coEvents,
		vector<LPCOLLISIONEVENT>& coEventsResult,
		float& min_tx,
		float& min_ty,
		float& nx,
		float& ny,
		float& rdx,
		float& rdy);
	virtual LPCOLLISIONEVENT SweptAABBEx(LPGAMEOBJECT coO);
	virtual void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);

public:
	CGoomba(int f);
	virtual void SetState(int state);
	virtual int GetState();
	void SetForm(int f) { form = f; }
	int GetForm() { return form; }
};