#pragma once
#include "GameObject.h"
#include "Collision.h"
#include "PlayScence.h"

#define KOOPA_GRAVITY									0.001f
#define PARAKOOPA_JUMP_SPEED							0.3f
#define KOOPA_JUMP_DEFLECT_SPEED						0.2f

#define KOOPA_RED_FORM									0
#define KOOPA_GREEN_FORM								1
#define PARAKOOPA_GREEN_FORM							2	
#define KOOPA_BLACK_FORM								3
#define PARAKOOPA_RED_FORM								4

#define KOOPA_WALKING_SPEED								0.03f
#define KOOPA_SPINNING_SPEED							0.25f
#define KOOPA_INTRO_SPEED_X								0.05f

#define KOOPA_BBOX_WIDTH								15
#define KOOPA_BBOX_HEIGHT								26

#define KOOPA_BBOX_HIDE_WIDTH							15
#define KOOPA_BBOX_HIDE_HEIGHT							16

#define KOOPA_BBOX_SPIN_WIDTH							13
#define KOOPA_BBOX_SPIN_HEIGHT							16


#define KOOPA_ANI_RED_WALKING_RIGHT						0
#define KOOPA_ANI_RED_WALKING_LEFT						1
#define KOOPA_ANI_RED_TORTOISESHELL_DOWN				2
#define KOOPA_ANI_RED_TORTOISESHELL_UP					14
#define KOOPA_ANI_RED_SPIN_DOWN							3
#define KOOPA_ANI_RED_TAKEN_DOWN						4
#define KOOPA_ANI_RED_TAKEN_UP							14
#define KOOPA_ANI_RED_SPIN_UP							12
#define KOOPA_ANI_RED_REBORN_DOWN						16
#define KOOPA_ANI_RED_REBORN_UP							18

#define KOOPA_ANI_GREEN_TORTOISESHELL_UP				15
#define KOOPA_ANI_GREEN_SPIN_UP							13
#define KOOPA_ANI_GREEN_WALKING_RIGHT					5
#define KOOPA_ANI_GREEN_WALKING_LEFT					6
#define KOOPA_ANI_GREEN_TORTOISESHELL_DOWN				7
#define KOOPA_ANI_GREEN_SPIN_DOWN						8
#define KOOPA_ANI_GREEN_TAKEN_DOWN						9
#define KOOPA_ANI_GREEN_TAKEN_UP						15
#define KOOPA_ANI_GREEN_REBORN_DOWN						17
#define KOOPA_ANI_GREEN_REBORN_UP						19

#define PARAKOOPA_ANI_GREEN_JUMPING_RIGHT				10
#define PARAKOOPA_ANI_GREEN_JUMPING_LEFT				11


#define KOOPA_STATE_WALKING_RIGHT						100
#define KOOPA_STATE_WALKING_LEFT						200
#define KOOPA_STATE_TORTOISESHELL_DOWN					300
#define KOOPA_STATE_TORTOISESHELL_UP					800
#define KOOPA_STATE_SPIN_RIGHT							400
#define KOOPA_STATE_SPIN_LEFT							500
#define KOOPA_STATE_TAKEN								600
#define KOOPA_STATE_REBORN								900

#define PARAKOOPA_FLYING_TIME							2000

#define KOOPA_DEAD_ZONE_Y								448

#define KOOPA_RESET_POSITION_X							5

#define PARAKOOPA_RED_FORM_ZONE_RIGHT					18
#define PARAKOOPA_RED_FORM_ZONE_LEFT					-1

#define KOOPA_STATE_DIE									1000

#define KOOPA_INTRO_HIDEN_TIME							6000
#define KOOPA_INTRO_TORTOISESHELL_DOWN					7500
#define KOOPA_INTRO_TAKEN_TIME_START					8600
#define KOOPA_INTRO_TAKEN_TIME_END						10000
#define KOOPA_INTRO_SPIN_RIGHT_TIME_START				12800
#define KOOPA_INTRO_SPIN_RIGHT_TIME_END					15000
#define KOOPA_INTRO_TORTOISESHELL_DOWN_TIME_START		15000
#define KOOPA_INTRO_TORTOISESHELL_DOWN_TIME_END			15050
#define KOOPA_INTRO_TORTOISESHELL_DOWN_POS_X			283
#define KOOPA_INTRO_TORTOISESHELL_DOWN_POS_Y			144
#define KOOPA_INTRO_SPIN_LEFT_TIME_START				16100
#define KOOPA_INTRO_SPIN_LEFT_TIME_END					16400
#define KOOPA_INTRO_TORTOISESHELL_DOWN_1_TIME_START		16550
#define KOOPA_INTRO_TORTOISESHELL_DOWN_1_TIME_END		18000
#define KOOPA_INTRO_SPIN_RIGHT_1_TIME_START				18200
#define KOOPA_INTRO_SPIN_RIGHT_1_TIME_END				20000
#define KOOPA_INTRO_SET_POS_1_START						20000
#define KOOPA_INTRO_SET_POS_1_END						20100
#define KOOPA_INTRO_SET_POS_2_START						20700
#define KOOPA_INTRO_SET_POS_2_END						20800
#define KOOPA_INTRO_POS_1_X								0
#define KOOPA_INTRO_POS_1_Y								136

#define KOOPA_INTRO_POS_PLUS_2_X						2

#define KOOPA_STATE_HIDEN								700

class CKoopa : public CollisionObject
{
	int form;
	int isSpin = 0;
	int isDown = 1;
	int isDie = 0;

	float preY;
	int hiden_state;
	int tortoiseshell_state;
	int intro_state;
	int isFlyDown = false;
	DWORD create_time;
	DWORD fly_time;
	int deflect_state = 0;
	DWORD defect_start;
	DWORD tortoiseshell_start;
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
	int GetIsDown() { return isDown; }
	void SetDefectStart(DWORD t) { defect_start = t; }
	void SetTimeFly() { 
		if(fly_time == 0)
			fly_time = (DWORD)GetTickCount64();
	}
	void SetDefectState(int d) { deflect_state = d; }
	int GetForm() { return form; }

	void SetIntroState(int s) { intro_state = s; }
	void CreateIntroAnimationKoopa();
};

