#pragma once
#include "GameObject.h"
#include "Collision.h"

#define KOOPA_GRAVITY					0.001f
#define PARAKOOPA_JUMP_SPEED			0.3f
#define KOOPA_JUMP_DEFLECT_SPEED		0.2f

#define KOOPA_RED_FORM					0
#define KOOPA_GREEN_FORM				1
#define PARAKOOPA_GREEN_FORM			2	
#define KOOPA_BLACK_FORM				3

#define KOOPA_WALKING_SPEED				0.03f;
#define KOOPA_SPINNING_SPEED			0.25f;

#define KOOPA_BBOX_WIDTH				15
#define KOOPA_BBOX_HEIGHT				26

#define KOOPA_BBOX_HIDE_WIDTH			15
#define KOOPA_BBOX_HIDE_HEIGHT			16

#define KOOPA_BBOX_SPIN_WIDTH			12
#define KOOPA_BBOX_SPIN_HEIGHT			15


#define KOOPA_ANI_RED_WALKING_RIGHT		0
#define KOOPA_ANI_RED_WALKING_LEFT		1
#define KOOPA_ANI_RED_HIDE				2
#define KOOPA_ANI_RED_SPIN				3
#define KOOPA_ANI_RED_TAKEN				4
#define KOOPA_ANI_GREEN_WALKING_RIGHT	5
#define KOOPA_ANI_GREEN_WALKING_LEFT	6
#define KOOPA_ANI_GREEN_HIDE			7
#define KOOPA_ANI_GREEN_SPIN			8
#define KOOPA_ANI_GREEN_TAKEN			9
#define PARAKOOPA_ANI_GREEN_JUMPING_RIGHT	10
#define PARAKOOPA_ANI_GREEN_JUMPING_LEFT	11


#define KOOPA_STATE_WALKING_RIGHT			100
#define KOOPA_STATE_WALKING_LEFT			200
#define KOOPA_STATE_HIDE					300
#define KOOPA_STATE_SPIN_RIGHT				400
#define KOOPA_STATE_SPIN_LEFT				500
#define KOOPA_STATE_TAKEN					600
#define KOOPA_STATE_HIDEN					700

class CKoopa : public CollisionObject
{
	int form;
	int isSpin = 0;
	int hiden_state;
	int intro_state;
	DWORD create_time;
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


public:
	CKoopa(int form);
	virtual void SetState(int state);
	virtual int GetState();
	void SetForm(int f) { form = f; }
	int GetForm() { return form; }

	void SetIntroState(int s) { intro_state = s; }
	void CreateIntroAnimationKoopa();
};

