#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"
#include "Utils.h"
#include "Goombas.h"
#include "Koopas.h"
#include "ColorBrick.h"
#include "Coin.h"
#include "QuestionBlock.h"
#include "TransObject.h"
#include "Road.h"
#include "Pipe.h"
#include "FireBullet.h"
#include "FirePlantBullet.h"
#include "Mushroom.h"
#include "Leaf.h"
#include "Curtain.h"
#include "Switch.h"
#include "GoalCard.h"
#include "BlueBrick.h"

CMario::CMario(float x, float y)
{
	level = MARIO_LEVEL_BIG;
	is_high = 0;
	untouchable = 0;
	run_state = 0;
	intro_state = 0;
	jump_state = 0;
	kick_state = 0;
	turn_state = 0;
	run_fast_state = 0;
	fall_state = 0;
	smoke_state = 0;

	streak_Kill = -1;

	checkSit = 0;
	walking_right_speech = 0;
	walking_right_speech = 0;
	take_tortoistate_state = 0;
	tortoiseshell = NULL;
	for (int i = 0; i < 2; i++)
		fire_bullet[i] = NULL;
	create_time = (DWORD)GetTickCount64();
	SetState(MARIO_STATE_IDLE);

	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;
}

int CMario::GetCurrentWidthMario()
{
	if (level == MARIO_LEVEL_SMALL)
		return MARIO_SMALL_BBOX_WIDTH;
	else if (level == MARIO_LEVEL_BIG)
		return MARIO_BIG_BBOX_WIDTH;
	else if (level == MARIO_LEVEL_TAIL)
		return MARIO_TAIL_BBOX_WIDTH;
	else if (level == MARIO_LEVEL_FIRE)
		return MARIO_FIRE_BBOX_WIDTH;
	else return 0;
}

int CMario::GetCurrentHeightMario()
{
	if (level == MARIO_LEVEL_SMALL)
		return MARIO_SMALL_BBOX_HEIGHT;
	else if (level == MARIO_LEVEL_BIG)
		return MARIO_BIG_BBOX_HEIGHT;
	else if (level == MARIO_LEVEL_TAIL)
		return MARIO_TAIL_BBOX_HEIGHT;
	else if (level == MARIO_LEVEL_FIRE)
		return MARIO_FIRE_BBOX_HEIGHT;
	else return 0;
}
double CMario::GetCenterWidthMario()
{
	return round(x + GetCurrentWidthMario() / 2);
}
double CMario::GetCenterHeightMario()
{
	return round(y + GetCurrentHeightMario() / 2);
}



void CMario::FilterCollision(
	vector<LPCOLLISIONEVENT>& coEvents,
	vector<LPCOLLISIONEVENT>& coEventsResult,
	float& min_tx, float& min_ty,
	float& nx, float& ny, float& rdx, float& rdy)
{
	min_tx = 1.0f;
	min_ty = 1.0f;

	nx = 0.0f;
	ny = 0.0f;
	coEventsResult.clear();

	for (UINT i = 0; i < coEvents.size(); i++)
	{
		LPCOLLISIONEVENT c = coEvents[i];

		if (dynamic_cast<CColorBrickTop*>(c->obj))
		{
			if (c->ny < 0 && c->t < 1.0f)
			{
				min_ty = c->t; ny = c->ny; rdy = c->dy;
				coEventsResult.push_back(coEvents[i]);
			}
		}
		else if (!dynamic_cast<CColorBrick*>(c->obj) && !dynamic_cast<CTransObject*>(c->obj) && !dynamic_cast<CFireBullet*>(c->obj))
		{
			if (c->nx != 0 && c->t < 1.0f)
			{
				min_tx = c->t; nx = c->nx; rdx = c->dx;
				coEventsResult.push_back(coEvents[i]);
			}

			if (c->ny != 0 && c->t < 1.0f)
			{
				min_ty = c->t; ny = c->ny; rdy = c->dy;
				coEventsResult.push_back(coEvents[i]);
			}

		}

	}
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	if (intro_state) CreateIntroAnimationMario();

	is_idle = 0;


	if (vy > 0) jump_state = 1;
	CGameObject::Update(dt);

	if (smoke_state)
	{
		if (GetTickCount64() - smoke_start < 300)
		{
			return;
		}
		else {
			smoke_state = 0;
			SetLevel(MARIO_LEVEL_TAIL);
			SetState(MARIO_STATE_IDLE);
		}
	}

	if (growup_state)
	{
		if (GetTickCount64() - growup_start < 1000)
		{
			return;
		}
		else {
			SetState(MARIO_STATE_IDLE);
			growup_state = 0;
			if (!intro_state) SetLevel(MARIO_LEVEL_BIG);
		}
	}
	// Simple fall down
	if(!pipe_down_state && !pipe_up_state)
		vy += MARIO_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	//float t;

	// turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		CMario::FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		float remainingTime = 1.0f - min_tx;
		
		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		if (rdx != 0 && rdx != dx)
			x += nx * abs(rdx);

		x += min_tx * dx + nx * 0.2f;
		y += min_ty * dy + ny * 0.2f;

		//if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		if (vy < 0.02 && vy >= 0) {
			jump_state = 0;

			vy = 0;
		}


		// reset untouchable timer if untouchable time has passed
		if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
		{
			untouchable_start = 0;
			untouchable = 0;
		}

		if (vy == 0) {

			fall_state = 0;
		}
		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			

			if (dynamic_cast<CGoomba*>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState() != GOOMBA_STATE_DIE)
					{
						streak_Kill++;
						if(!intro_state) DisplayScores(streak_Kill, goomba->x, goomba->y, GetTickCount64());
						if (goomba->GetForm() != PARAGOOMBA_BROWN_FORM)
							goomba->SetState(GOOMBA_STATE_DIE);
						else goomba->SetForm(GOOMBA_BROWN_FORM);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
						jump_state = 1;
					}
				}

				else if (e->nx != 0)
				{
					if (untouchable == 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							if (level > MARIO_LEVEL_SMALL && !fight_state && !intro_state)
							{
								SetLevel(GetLevel() - 1);
								StartUntouchable();
							}
							else if (fight_state)
							{
								goomba->SetState(GOOMBA_STATE_DEFLECT);
							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
				}
			} 

			if (dynamic_cast<CKoopa*>(e->obj)) // if e->obj is Koopa
			{
				CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
				if (koopa->GetState() != KOOPA_STATE_TAKEN)
				{
					if (fight_state && (koopa->GetState() != KOOPA_STATE_TORTOISESHELL_DOWN || koopa->GetState() != KOOPA_STATE_TORTOISESHELL_UP))
					{
						koopa->SetState(KOOPA_STATE_TORTOISESHELL_UP);
						koopa->SetDefectStart(GetTickCount64());
						koopa->SetDefectState(1);
					}

					else if (e->ny < 0)
					{

						if (koopa->GetState() != KOOPA_STATE_TORTOISESHELL_DOWN && koopa->GetState() != KOOPA_STATE_TORTOISESHELL_UP)
						{
							streak_Kill++;
							DisplayScores(streak_Kill, koopa->x, koopa->y, GetTickCount64());
							if (koopa->GetForm() == PARAKOOPA_GREEN_FORM)
								koopa->SetForm(KOOPA_GREEN_FORM);
							else if (koopa->GetIsDown()) koopa->SetState(KOOPA_STATE_TORTOISESHELL_DOWN);
							else koopa->SetState(KOOPA_STATE_TORTOISESHELL_UP);
							jump_state = 1;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}            
						else if (koopa->GetState() == KOOPA_STATE_TORTOISESHELL_DOWN || koopa->GetState() == KOOPA_STATE_TORTOISESHELL_UP)
						{

							if (round(x + CMario::GetCurrentWidthMario() / 2) < koopa->x + round(KOOPA_BBOX_WIDTH / 2)) koopa->SetState(KOOPA_STATE_SPIN_RIGHT);
							else koopa->SetState(KOOPA_STATE_SPIN_LEFT);
							jump_state = 1;
						}
					}

					else if (e->nx != 0)
					{
						if (untouchable == 0)
						{
							if (koopa->GetState() != KOOPA_STATE_TORTOISESHELL_DOWN && koopa->GetState() != KOOPA_STATE_TORTOISESHELL_UP && !fight_state)
							{
								if (level > MARIO_LEVEL_SMALL)
								{
									level = MARIO_LEVEL_SMALL;
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
							else
							{
								if (state != MARIO_STATE_RUNNING_LEFT && state != MARIO_STATE_RUNNING_RIGHT)
								{
									kick_start = (DWORD)GetTickCount64();
									if (e->nx < 0)
									{
										SetState(MARIO_STATE_KICK);

										koopa->SetState(KOOPA_STATE_SPIN_RIGHT);
									}
									else
									{
										SetState(MARIO_STATE_KICK);
										koopa->SetState(KOOPA_STATE_SPIN_LEFT);
									}
								}
								else if (!fight_state)
								{
									tortoiseshell = koopa;
									koopa->SetState(KOOPA_STATE_TAKEN);
									if (e->nx < 0) {
										SetState(MARIO_STATE_TAKE_TORTOISESHELL_RIGHT);
									}
									else SetState(MARIO_STATE_TAKE_TORTOISESHELL_LEFT);
								}
							}
						}
					}
				}
				
			}
			
			

			if (dynamic_cast<CCoin*>(e->obj)) // if e->obj is Koopa
			{
				CCoin* coin = dynamic_cast<CCoin*>(e->obj);

				if (coin->GetState() == COIN_STATE_NORMAL)
				{
					coinplay->AddCoin();
					coin->SetState(COIN_STATE_HIDEN);

				}			
			}


			if (dynamic_cast<CGoalCard*>(e->obj)) // if e->obj is Koopa
			{
				CGoalCard* goalCard = dynamic_cast<CGoalCard*>(e->obj);
				CGame* game = CGame::GetInstance();

				if (goalCard->GetState() != GOALCARD_STATE_HIDEN)
				{
					SetCardState(goalCard->GetState());
					game->SetItem(goalCard->GetState());
					goalCard->GetCardText()->SetState(1);
					goalCard->GetCardText()->GetCard()->SetState(goalCard->GetState());
					goalCard->SetState(COIN_STATE_HIDEN);

				}
			}

			if (dynamic_cast<CBrick*>(e->obj)) // if e->obj is Koopa
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);

				if (nx != 0)
				{
					is_idle = 1;
					if (brick->GetState() == BRICK_STATE_NORMAL && fight_state)
					{

						brick->SetState(BRICK_STATE_HIDEN);
						CreatePieceBrick(brick->x, brick->y, GetTickCount64());

					}
				}
				if (ny != 0)
				{
					is_idle = IsIdle(x, y, e->obj->x, e->obj->y, e->ny);
				}
			}


			if (dynamic_cast<CPipe*>(e->obj)) 
			{
				CPipe* pipe = dynamic_cast<CPipe*>(e->obj);

				if (e->nx != 0)
				{
					is_idle = 1;
					if (y < 149 - GetCurrentHeightMario() - 5) {
						x += vx * min_tx;
						y += vy;

						vx *= remainingTime;
						vy *= remainingTime;

						if (abs(nx) > 0.0001f)
							vx = -vx;
						if (abs(ny) > 0.0001f)
							vy = -vy;

						float dotprod = (vx * ny + vy * nx) * remainingTime;
						vx = dotprod * ny;
						vy = dotprod * nx;
					}

				}
				if (e->ny != 0 && sit_state && pipe->GetType() == PIPE_STATE_UP_DOWN)
				{
					is_idle = IsIdle(x, y, e->obj->x, e->obj->y, e->ny);
					if (!pipe_down_state)
					{
						DebugOut(L"da vao ong\n");
						SetState(MARIO_STATE_PIPE_DOWN);
						pipe_down_start = GetTickCount64();
					}
				}
				if (e->ny != 0 && pipe->GetType() == PIPE_STATE_DOWN_UP)
				{
					is_idle = IsIdle(x, y, e->obj->x, e->obj->y, e->ny);
					if (!pipe_up_state)
					{
						DebugOut(L"da vao ong\n");
						SetState(MARIO_STATE_PIPE_UP);
						pipe_up_start = GetTickCount64();
					}
				}

			}

			if (dynamic_cast<CQuestionBlock*>(e->obj)) // if e->obj is question block
			{

				CQuestionBlock* quesBlock = dynamic_cast<CQuestionBlock*>(e->obj);
				if (e->ny > 0)
				{
					fall_state = 1;
					vy = 0;
					if (quesBlock->GetState() == QUESTIONBLOCK_ITEM_STATE)
					{
						if(dynamic_cast<CCoin*>(quesBlock->GetItemObject()))
							coinplay->AddCoin();
						quesBlock->SetState(QUESTIONBLOCK_DEFLECT_STATE);
						quesBlock->SetStateItem(EFFECT_STATE);
					}
				}
			}

			if (dynamic_cast<CRoad*>(e->obj))
			{				

				CRoad* road = dynamic_cast<CRoad*>(e->obj);
				if (e->ny < 0)
				{
					vy = 0;
					fall_state = 0;
				}
			}

			if (dynamic_cast<CFirePlantBullet*>(e->obj))
			{
				CFirePlantBullet* fireplantbullet = dynamic_cast<CFirePlantBullet*>(e->obj);
				if (fireplantbullet->GetState() != FIREBULLET_DESTROY_STATE && fireplantbullet->GetState() != FIREBULLET_TRANSPARENT_STATE)
				{
					if (level > MARIO_LEVEL_SMALL)
					{
						SetLevel(GetLevel() - 1);
						fireplantbullet->SetState(FIREBULLET_DESTROY_STATE);
						StartUntouchable();
					}
					else
						SetState(MARIO_STATE_DIE);
				}
			}
			if (dynamic_cast<CSwitch*>(e->obj))
			{
				CSwitch* switchs = dynamic_cast<CSwitch*>(e->obj);
				if (e->ny < 0)
				{
					if (switchs->GetState() == EFFECT_STATE)
					{
						switchs->SetSwitch();
						switchs->SetState(SWITCH_STATE_ACTIVE);
						switchs->SetPosition(switchs->x, switchs->y + 9);
					}

				}

			}

			if (dynamic_cast<CMushroom*>(e->obj))
			{
				CMushroom* mushroom = dynamic_cast<CMushroom*>(e->obj);
				if (!mushroom->IsHidenState())
				{
					if (GetLevel() == MARIO_LEVEL_SMALL)
					{
						mushroom->SetState(MUSHROOM_STATE_HIDEN);
						SetState(MARIO_STATE_GROWUP);
						growup_start = (DWORD)GetTickCount64();
						SetPosition(x, y - MARIO_BIG_BBOX_HEIGHT + MARIO_SMALL_BBOX_HEIGHT - 1);
					}
				}

			}
			if (dynamic_cast<CLeaf*>(e->obj))
			{
				CLeaf* mushroom = dynamic_cast<CLeaf*>(e->obj);
				if (!mushroom->IsHidenState())
				{
					mushroom->SetState(LEAF_STATE_HIDEN);
					SetState(MARIO_STATE_SMOKE);
					SetPosition(x, y - 2);
					smoke_start = (DWORD)GetTickCount64();
					//SetLevel(GetLevel() + 1);
				}

			}

			if (!dynamic_cast<CGoomba*>(e->obj) && !dynamic_cast<CKoopa*>(e->obj))
				streak_Kill = -1;
		}
	}

	if (!intro_state)
	{
		if (GetMaxPower()) maxpower_state = 1;
		else maxpower_state = 0;
	}

	if (pipe_down_state)
	{
		if (GetTickCount64() - pipe_down_start < 400)
		{
			SetState(MARIO_STATE_PIPE_DOWN);
			y += 1;
		}
		else
		{
			pipe_down_state = 0;
			SetState(MARIO_STATE_IDLE);
			SetPosition(2260, 500);
		}
	}

	if (pipe_up_state)
	{
		if (GetTickCount64() - pipe_up_start < 400)
		{
			SetState(MARIO_STATE_PIPE_UP);
			y -= 2;
		}
		else if(GetTickCount64() - pipe_up_start < 410) SetPosition(2328, 384);
		else if(GetTickCount64() - pipe_up_start < 800)
		{
			SetState(MARIO_STATE_PIPE_UP);
			y -= 2;
		}
		else
		{
			pipe_up_state = 0;
			SetState(MARIO_STATE_IDLE);
		}
	}


	if (tortoiseshell != NULL) {
		if (level == MARIO_LEVEL_SMALL)
		{
			if (nx > 0)
			{
				SetState(MARIO_STATE_TAKE_TORTOISESHELL_RIGHT);
				tortoiseshell->x = x + 10;
				tortoiseshell->y = y - 2;
			}
			else
			{
				SetState(MARIO_STATE_TAKE_TORTOISESHELL_LEFT);
				tortoiseshell->x = x - 10;
				tortoiseshell->y = y - 2;
			}

		}

		else if (level == MARIO_LEVEL_BIG)
		{
			if (nx > 0)
			{
				SetState(MARIO_STATE_TAKE_TORTOISESHELL_RIGHT);
				tortoiseshell->x = x + 10;
				tortoiseshell->y = y + 10;
			}
			else
			{
				SetState(MARIO_STATE_TAKE_TORTOISESHELL_LEFT);
				tortoiseshell->x = x - 10;
				tortoiseshell->y = y + 10;
			}
		}
		else if (level == MARIO_LEVEL_TAIL)
		{
			if (nx > 0)
			{
				SetState(MARIO_STATE_TAKE_TORTOISESHELL_RIGHT);
				tortoiseshell->x = x + 15;
				tortoiseshell->y = y + 10;
			}
			else
			{
				SetState(MARIO_STATE_TAKE_TORTOISESHELL_LEFT);
				tortoiseshell->x = x - 10;
				tortoiseshell->y = y + 10;
			}
		}
		else if (level == MARIO_LEVEL_FIRE)
		{
			if (nx > 0)
			{
				SetState(MARIO_STATE_TAKE_TORTOISESHELL_RIGHT);
				tortoiseshell->x = x + 10;
				tortoiseshell->y = y + 10;
			}
			else
			{
				SetState(MARIO_STATE_TAKE_TORTOISESHELL_LEFT);
				tortoiseshell->x = x - 10;
				tortoiseshell->y = y + 10;
			}
		}
	}

	if (level == MARIO_LEVEL_DARK)
	{

		darkEnergy->x = x - 28;
		darkEnergy->y = y - 40;
	}

	if (GetTickCount64() - kick_start < 150 && !take_tortoistate_state)
		SetState(MARIO_STATE_KICK);

	if (GetTickCount64() - fight_start < 300)
		SetState(MARIO_STATE_FIGHT);
	else fight_state = 0;
	
	if (GetTickCount64() - shoot_fire_bullet_start < 200)
	{
		if(nx > 0) 	SetState(MARIO_STATE_SHOOT_FIRE_BULLET_RIGHT);
		else SetState(MARIO_STATE_SHOOT_FIRE_BULLET_LEFT);
	}

	else
	{
		shoot_fire_bullet_state = 0;
		CreateFireBullet(NULL);
	}

	if (state == MARIO_STATE_RUNNING_RIGHT && running_time_right == -1) running_time_right = (DWORD)GetTickCount64();
	else if (state == MARIO_STATE_RUNNING_LEFT && running_time_left == -1) running_time_left = (DWORD)GetTickCount64();

	if (GetTickCount64() - GetKickStart() < 200) SetState(MARIO_STATE_KICK);
	else kick_state = 0;

	if (GetTickCount64() - fly_low_start < 300 && !intro_state)
	{
		if (nx > 0) SetState(MARIO_STATE_FLYING_LOW_RIGHT);
		else SetState(MARIO_STATE_FLYING_LOW_LEFT);
	}
	else fly_low_state = 0;

	if (GetTickCount64() - fly_low_start < 1300 && intro_state)
	{
		if (nx > 0) SetState(MARIO_STATE_FLYING_LOW_RIGHT);
		else SetState(MARIO_STATE_FLYING_LOW_LEFT);
	}
	
	if (GetTickCount64() - fly_high_start < 1000)
	{
		if (nx > 0) SetState(MARIO_STATE_FLYING_HIGH_RIGHT);
		else SetState(MARIO_STATE_FLYING_HIGH_LEFT);
	}
	else fly_high_state = 0;

	if (fly_high_state && y < 28) is_high = 1;
	if (y >= 28) is_high = 0;

	if (turn_state == 0 && run_state == 0 && !kick_state)
	{
		if (nx > 0)
		{
			if (GetTickCount64() - GetWalkRightTime() < sliding_time_right)
			{
				SetState(MARIO_STATE_WALKING_RIGHT);
				slide_state = 1;
			}
			else slide_state = 0;
		}
		else
		{
			if (GetTickCount64() - GetWalkLeftTime() < sliding_time_left)
			{
				SetState(MARIO_STATE_WALKING_LEFT);
				slide_state = 1;
			}
			else slide_state = 0;
		}
	}
	else if (!turn_state && !run_fast_state && !kick_state)
	{
		if (nx > 0)
		{
			if (GetTickCount64() - GetWalkRightTime() < sliding_time_right)
			{
				SetState(MARIO_STATE_RUNNING_RIGHT);
				slide_state = 1;
			}
			else slide_state = 0;
		}
		else
		{
			if (GetTickCount64() - GetWalkLeftTime() < sliding_time_left)
			{
				SetState(MARIO_STATE_RUNNING_LEFT);
				slide_state = 1;
			}
			else slide_state = 0;
		}
	}
	else if (!turn_state && run_state && !kick_state)
	{
		if (nx > 0)
		{
			if (GetTickCount64() - GetWalkRightTime() < sliding_time_right)
			{
				SetState(MARIO_STATE_RUNNING_RIGHT_FAST);
				slide_state = 1;
			}
			else slide_state = 0;
		}
		else
		{
			if (GetTickCount64() - GetWalkLeftTime() < sliding_time_left)
			{
				SetState(MARIO_STATE_RUNNING_LEFT_FAST);
				slide_state = 1;
			}
			else slide_state = 0;
		}
	}

	if (!intro_state)
	{
		if (state == MARIO_STATE_RUNNING_LEFT || state == MARIO_STATE_RUNNING_RIGHT || state == MARIO_STATE_RUNNING_LEFT_FAST || state == MARIO_STATE_RUNNING_RIGHT_FAST)
		{
			if (GetTickCount64() - running_start > 200 && !is_idle)
			{
				arrows->SetWhiteArrows();
				running_start = (DWORD)GetTickCount64();
			}
			else if (is_idle && GetTickCount64() - running_start > 200)
			{
				arrows->SetBlackArrows();
				running_start = (DWORD)GetTickCount64();
			}

		}
		else {
			if (GetTickCount64() - running_start > 200)
			{
				arrows->SetBlackArrows();
				running_start = (DWORD)GetTickCount64();
			}

		}
	}

	if (x > 2800)
	{
		CGame* game = CGame::GetInstance();

		SetPosition(0, 0);
		//CGame::GetInstance()->SwitchScene(3);
	}


	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CMario::CreateFireBullet(CGameObject* fireBullet)
{
	for (int i = 0; i < 2; i++)
	{
		if (fire_bullet[i] == NULL)
		{
			fire_bullet[i] = fireBullet;
			break;
		}
		else if (fire_bullet[i]->GetState() == FIREBULLET_DESTROY_STATE)
		{
			fire_bullet[i]->SetState(FIREBULLET_TRANSPARENT_STATE);
		}
	}
}

void CMario::CreateScore(CScoreEffect* s)
{
	for (int i = 0; i < 3; i++)
	{
		if (score[i] == NULL) {
			score[i] = s;
			break;
		}
		else if (score[i]->GetState() != SCORE_STATE_HIDEN)
		{
			score[i]->SetState(SCORE_STATE_HIDEN);
		}
	}
}




void CMario::ShootFireBullet()
{
	for (int i = 0; i < 2; i++)
	{
		if (fire_bullet[i]->GetState() != FIREBULLET_SHOOTED_RIGHT_STATE &&
			fire_bullet[i]->GetState() != FIREBULLET_SHOOTED_LEFT_STATE)
		{
			if (nx > 0)
			{
				fire_bullet[i]->SetPosition(x + 14, y);
				fire_bullet[i]->SetState(FIREBULLET_SHOOTED_RIGHT_STATE);
			}

			else 
			{
				fire_bullet[i]->SetPosition(x, y);
				fire_bullet[i]->SetState(FIREBULLET_SHOOTED_LEFT_STATE);
			}


			break;
		}
	}

}


void CMario::Render()
{
	int ani = -1;
	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
	else
		if (level == MARIO_LEVEL_BIG)
		{
			if (vx == 0)
			{
				if (nx > 0)
				{
					if (smoke_state) ani = MARIO_ANI_SMOKE;
					else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_BIG_WORLDMAP;
					else if (headup_state == 1) ani = MARIO_ANI_HEADUP;
					else if (deflect_state == 1) ani = MARIO_ANI_DEFLECT;
					else if (kick_state == 1) ani = MARIO_ANI_BIG_KICK_RIGHT;
					else if (sit_state == 1) ani = MARIO_ANI_BIG_SIT_RIGHT;
					else if (take_tortoistate_state == 1) ani = MARIO_ANI_BIG_TAKE_TORTOISESHELL_IDLE_RIGHT;
					else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_BIG_JUMPING_RIGHT;
					else ani = MARIO_ANI_BIG_IDLE_RIGHT;
				}
				else
				{
					if (smoke_state) ani = MARIO_ANI_SMOKE;
					else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_BIG_WORLDMAP;
					else if (headup_state == 1) ani = MARIO_ANI_HEADUP;
					else if (deflect_state == 1) ani = MARIO_ANI_DEFLECT;
					else if (kick_state == 1) ani = MARIO_ANI_BIG_KICK_LEFT;
					else if (sit_state == 1) ani = MARIO_ANI_BIG_SIT_LEFT;
					else if (take_tortoistate_state == 1) ani = MARIO_ANI_BIG_TAKE_TORTOISESHELL_IDLE_LEFT;
					else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_BIG_MAX_POWER_LEFT;
					else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_BIG_JUMPING_LEFT;
					else ani = MARIO_ANI_BIG_IDLE_LEFT;
				}
			}
			else if (vx > 0)
			{
				if (smoke_state) ani = MARIO_ANI_SMOKE;
				else if (turn_state == 1) ani = MARIO_ANI_BIG_TURN_LEFT;
				else if (take_tortoistate_state == 1) ani = MARIO_ANI_BIG_TAKE_TORTOISESHELL_RIGHT;
				else if (kick_state == 1) ani = MARIO_ANI_BIG_KICK_RIGHT;
				else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_BIG_MAX_POWER_RIGHT;
				else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_BIG_JUMPING_RIGHT;
				else if (run_fast_state == 1) ani = MARIO_ANI_BIG_RUNNING_RIGHT_FAST;
				else if (run_state == 1)	ani = MARIO_ANI_BIG_RUNNING_RIGHT;
				else ani = MARIO_ANI_BIG_WALKING_RIGHT;
			}
			else
			{
				if (smoke_state) ani = MARIO_ANI_SMOKE;
				else if (turn_state == 1) ani = MARIO_ANI_BIG_TURN_RIGHT;
				else if (take_tortoistate_state == 1) ani = MARIO_ANI_BIG_TAKE_TORTOISESHELL_LEFT;
				else if (kick_state == 1) ani = MARIO_ANI_BIG_KICK_LEFT;
				else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_BIG_MAX_POWER_LEFT;
				else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_BIG_JUMPING_LEFT;
				else if (run_fast_state == 1) ani = MARIO_ANI_BIG_RUNNING_LEFT_FAST;
				else if (run_state == 1) ani = MARIO_ANI_BIG_RUNNING_LEFT;
				else ani = MARIO_ANI_BIG_WALKING_LEFT;
			}
		}
		else if (level == MARIO_LEVEL_DARK)
		{
			if (vx == 0)
			{
				if (nx > 0)
				{
					if (smoke_state) ani = MARIO_ANI_SMOKE;
					else if (headup_state == 1) ani = MARIO_ANI_HEADUP;
					else if (deflect_state == 1) ani = MARIO_ANI_DEFLECT;
					else if (kick_state == 1) ani = MARIO_ANI_DARK_KICK_RIGHT;
					//else if (sit_state == 1) ani = MARIO_ANI_DARK_SIT_RIGHT;
					else if (take_tortoistate_state == 1) ani = MARIO_ANI_DARK_TAKE_TORTOISESHELL_IDLE_RIGHT;
					else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_DARK_JUMPING_RIGHT;
					else ani = MARIO_ANI_DARK_IDLE_RIGHT;
				}
				else
				{
					if (smoke_state) ani = MARIO_ANI_SMOKE;
					else if (headup_state == 1) ani = MARIO_ANI_HEADUP;
					else if (deflect_state == 1) ani = MARIO_ANI_DEFLECT;
					else if (kick_state == 1) ani = MARIO_ANI_DARK_KICK_LEFT;
					//else if (sit_state == 1) ani = MARIO_ANI_DARK_SIT_LEFT;
					else if (take_tortoistate_state == 1) ani = MARIO_ANI_DARK_TAKE_TORTOISESHELL_IDLE_LEFT;
					else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_DARK_JUMPING_LEFT;
					else ani = MARIO_ANI_DARK_IDLE_LEFT;
				}
			}
			else if (vx > 0)
			{
				if (smoke_state) ani = MARIO_ANI_SMOKE;
				else if (turn_state == 1) ani = MARIO_ANI_DARK_TURN_LEFT;
				else if (take_tortoistate_state == 1) ani = MARIO_ANI_DARK_TAKE_TORTOISESHELL_RIGHT;
				else if (kick_state == 1) ani = MARIO_ANI_DARK_KICK_RIGHT;
				else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_DARK_JUMPING_RIGHT;
				//else if (run_fast_state == 1) ani = MARIO_ANI_DARK_RUNNING_RIGHT_FAST;
				else if (run_state == 1)	ani = MARIO_ANI_DARK_RUNNING_RIGHT;
				else ani = MARIO_ANI_DARK_WALKING_RIGHT;
			}
			else
			{
				if (smoke_state) ani = MARIO_ANI_SMOKE;
				else if (turn_state == 1) ani = MARIO_ANI_DARK_TURN_RIGHT;
				else if (take_tortoistate_state == 1) ani = MARIO_ANI_DARK_TAKE_TORTOISESHELL_LEFT;
				else if (kick_state == 1) ani = MARIO_ANI_DARK_KICK_LEFT;
				else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_DARK_JUMPING_LEFT;
				//else if (run_fast_state == 1) ani = MARIO_ANI_DARK_RUNNING_LEFT_FAST;
				else if (run_state == 1) ani = MARIO_ANI_DARK_RUNNING_LEFT;
				else ani = MARIO_ANI_DARK_WALKING_LEFT;
			}
		}

		else if (level == MARIO_LEVEL_SMALL)
		{
			if (vx == 0)
			{
				if (nx > 0)
				{
					if (growup_state) ani = MARIO_ANI_GROWUP_RIGHT;
					else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_SMALL_WORLDMAP;
					else if (kick_state == 1) ani = MARIO_ANI_SMALL_KICK_RIGHT;
					else if (take_tortoistate_state == 1) ani = MARIO_ANI_SMALL_TAKE_TORTOISESHELL_IDLE_RIGHT;
					else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_SMALL_MAX_POWER_RIGHT;
					else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_SMALL_JUMPING_RIGHT;
					else ani = MARIO_ANI_SMALL_IDLE_RIGHT;
				}
				else
				{
					if (growup_state) ani = MARIO_ANI_GROWUP_LEFT;
					else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_SMALL_WORLDMAP;
					else if (kick_state == 1) ani = MARIO_ANI_SMALL_KICK_LEFT;
					else if (take_tortoistate_state == 1) ani = MARIO_ANI_SMALL_TAKE_TORTOISESHELL_IDLE_LEFT;
					else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_SMALL_MAX_POWER_LEFT;
					else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_SMALL_JUMPING_LEFT;
					else ani = MARIO_ANI_SMALL_IDLE_LEFT;
				}
			}
			else if (vx > 0)
			{
				if (growup_state) ani = MARIO_ANI_GROWUP_RIGHT;
				else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_SMALL_WORLDMAP;
				else if (turn_state == 1) ani = MARIO_ANI_SMALL_TURN_LEFT;
				else if (take_tortoistate_state == 1) ani = MARIO_ANI_SMALL_TAKE_TORTOISESHELL_RIGHT;
				else if (kick_state == 1) ani = MARIO_ANI_SMALL_KICK_RIGHT;
				else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_SMALL_MAX_POWER_RIGHT;
				else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_SMALL_JUMPING_RIGHT;
				else if (run_fast_state == 1) ani = MARIO_ANI_SMALL_RUNNING_RIGHT_FAST;
				else if (run_state == 1)	ani = MARIO_ANI_SMALL_RUNNING_RIGHT;
				else ani = MARIO_ANI_SMALL_WALKING_RIGHT;
			}
			else
			{
				if (growup_state) ani = MARIO_ANI_GROWUP_LEFT;
				else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_SMALL_WORLDMAP;
				else if (turn_state == 1) ani = MARIO_ANI_SMALL_TURN_RIGHT;
				else if (take_tortoistate_state == 1) ani = MARIO_ANI_SMALL_TAKE_TORTOISESHELL_LEFT;
				else if (kick_state == 1) ani = MARIO_ANI_SMALL_KICK_LEFT;
				else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_SMALL_MAX_POWER_LEFT;
				else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_SMALL_JUMPING_LEFT;
				else if (run_fast_state == 1) ani = MARIO_ANI_SMALL_RUNNING_LEFT_FAST;
				else if (run_state == 1) ani = MARIO_ANI_SMALL_RUNNING_LEFT;
				else ani = MARIO_ANI_SMALL_WALKING_LEFT;
			}
		}

		else if (level == MARIO_LEVEL_TAIL)
		{
			if (vx == 0)
			{
				if (nx > 0)
				{
					if (fly_high_state == 1) ani = MARIO_ANI_FLY_HIGH_RIGHT;
					else if (fly_low_state == 1) ani = MARIO_ANI_FLY_LOW_RIGHT;
					else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_TAIL_WORLDMAP;
					else if (fight_state == 1) ani = MARIO_ANI_TAIL_FIGHT;
					else if (turn_state == 1) ani = MARIO_ANI_TAIL_TURN_RIGHT;
					else if (sit_state == 1) ani = MARIO_ANI_TAIL_SIT_RIGHT;
					else if (kick_state == 1) ani = MARIO_ANI_TAIL_KICK_RIGHT;
					else if (take_tortoistate_state == 1) ani = MARIO_ANI_TAIL_TAKE_TORTOISESHELL_IDLE_RIGHT;
					else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_TAIL_JUMPING_RIGHT;
					else ani = MARIO_ANI_TAIL_IDLE_RIGHT;
				}
				else
				{
					if (fly_high_state == 1) ani = MARIO_ANI_FLY_HIGH_LEFT;
					else if (fly_low_state == 1) ani = MARIO_ANI_FLY_LOW_LEFT;
					else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_TAIL_WORLDMAP;
					else if (fight_state == 1) ani = MARIO_ANI_TAIL_FIGHT;
					else if (turn_state == 1) ani = MARIO_ANI_TAIL_TURN_LEFT;
					else if (sit_state == 1) ani = MARIO_ANI_TAIL_SIT_LEFT;
					else if (kick_state == 1) ani = MARIO_ANI_TAIL_KICK_LEFT;
					else if (take_tortoistate_state == 1) ani = MARIO_ANI_TAIL_TAKE_TORTOISESHELL_IDLE_LEFT;
					else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_TAIL_JUMPING_LEFT;
					else ani = MARIO_ANI_TAIL_IDLE_LEFT;
				}
			}
			else if (vx > 0)
			{
				if (fly_high_state == 1) ani = MARIO_ANI_FLY_HIGH_RIGHT;
				else if (fly_low_state == 1) ani = MARIO_ANI_FLY_LOW_RIGHT;
				else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_TAIL_WORLDMAP;
				else if (fight_state == 1) ani = MARIO_ANI_TAIL_FIGHT;
				else if (turn_state == 1) ani = MARIO_ANI_TAIL_TURN_LEFT;
				else if (take_tortoistate_state == 1) ani = MARIO_ANI_TAIL_TAKE_TORTOISESHELL_RIGHT;
				else if (kick_state == 1) ani = MARIO_ANI_TAIL_KICK_RIGHT;
				else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_TAIL_JUMPING_RIGHT;
				else if (run_fast_state == 1) ani = MARIO_ANI_TAIL_RUNNING_RIGHT_FAST;
				else if (run_state == 1)	ani = MARIO_ANI_TAIL_RUNNING_RIGHT;
				else ani = MARIO_ANI_TAIL_WALKING_RIGHT;
			}
			else
			{
				if (fly_high_state == 1) ani = MARIO_ANI_FLY_HIGH_LEFT;
				else if (fly_low_state == 1) ani = MARIO_ANI_FLY_LOW_LEFT;
				else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_TAIL_WORLDMAP;
				else if (fight_state == 1) ani = MARIO_ANI_TAIL_FIGHT;
				else if (turn_state == 1) ani = MARIO_ANI_TAIL_TURN_RIGHT;
				else if (take_tortoistate_state == 1) ani = MARIO_ANI_TAIL_TAKE_TORTOISESHELL_LEFT;
				else if (kick_state == 1) ani = MARIO_ANI_TAIL_KICK_LEFT;
				else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_TAIL_JUMPING_LEFT;
				else if (run_fast_state == 1) ani = MARIO_ANI_TAIL_RUNNING_LEFT_FAST;
				else if (run_state == 1) ani = MARIO_ANI_TAIL_RUNNING_LEFT;
				else ani = MARIO_ANI_TAIL_WALKING_LEFT;
			}
		}
		else if (level == MARIO_LEVEL_FIRE)
		{
			if (vx == 0)
			{
				if (nx > 0)
				{
					if (shoot_fire_bullet_state) ani = MARIO_ANI_SHOOT_FIRE_BULLET_RIGHT;
					else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_FIRE_WORLDMAP;
					else if (kick_state == 1) ani = MARIO_ANI_FIRE_KICK_RIGHT;
					else if (sit_state == 1) ani = MARIO_ANI_FIRE_SIT_RIGHT;
					else if (take_tortoistate_state == 1) ani = MARIO_ANI_FIRE_TAKE_TORTOISESHELL_IDLE_RIGHT;
					else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_FIRE_MAX_POWER_RIGHT;
					else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_FIRE_JUMPING_RIGHT;
					else ani = MARIO_ANI_FIRE_IDLE_RIGHT;
				}
				else
				{
					if (shoot_fire_bullet_state) ani = MARIO_ANI_SHOOT_FIRE_BULLET_LEFT;
					else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_FIRE_WORLDMAP;
					else if (kick_state == 1) ani = MARIO_ANI_FIRE_KICK_LEFT;
					else if (sit_state == 1) ani = MARIO_ANI_FIRE_SIT_LEFT;
					else if (take_tortoistate_state == 1) ani = MARIO_ANI_FIRE_TAKE_TORTOISESHELL_IDLE_LEFT;
					else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_FIRE_MAX_POWER_LEFT;
					else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_FIRE_JUMPING_LEFT;
					else ani = MARIO_ANI_FIRE_IDLE_LEFT;
				}
			}
			else if (vx > 0)
			{
				if (shoot_fire_bullet_state) ani = MARIO_ANI_SHOOT_FIRE_BULLET_RIGHT;
				else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_FIRE_WORLDMAP;
				else if (turn_state == 1) ani = MARIO_ANI_FIRE_TURN_LEFT;
				else if (take_tortoistate_state == 1) ani = MARIO_ANI_FIRE_TAKE_TORTOISESHELL_RIGHT;
				else if (kick_state == 1) ani = MARIO_ANI_FIRE_KICK_RIGHT;
				else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_FIRE_MAX_POWER_RIGHT;
				else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_FIRE_JUMPING_RIGHT;
				else if (run_fast_state == 1) ani = MARIO_ANI_FIRE_RUNNING_RIGHT_FAST;
				else if (run_state == 1)	ani = MARIO_ANI_FIRE_RUNNING_RIGHT;
				else ani = MARIO_ANI_FIRE_WALKING_RIGHT;
			}
			else
			{
				if (shoot_fire_bullet_state) ani = MARIO_ANI_SHOOT_FIRE_BULLET_LEFT;
				else if (pipe_down_state || pipe_up_state) ani = MARIO_ANI_FIRE_WORLDMAP;
				else if (turn_state == 1) ani = MARIO_ANI_FIRE_TURN_RIGHT;
				else if (take_tortoistate_state == 1) ani = MARIO_ANI_FIRE_TAKE_TORTOISESHELL_LEFT;
				else if (kick_state == 1) ani = MARIO_ANI_FIRE_KICK_LEFT;
				else if (jump_state == 1 && maxpower_state == 1) ani = MARIO_ANI_FIRE_MAX_POWER_LEFT;
				else if (jump_state == 1 || jump_state == -1) ani = MARIO_ANI_FIRE_JUMPING_LEFT;
				else if (run_fast_state == 1) ani = MARIO_ANI_FIRE_RUNNING_LEFT_FAST;
				else if (run_state == 1) ani = MARIO_ANI_FIRE_RUNNING_LEFT;
				else ani = MARIO_ANI_FIRE_WALKING_LEFT;
			}
		}

	int alpha = 255;
	if (untouchable) alpha = 128;

	animation_set->at(ani)->Render(x, y, alpha);

	//RenderBoundingBox();
}

void CMario::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:

		deflect_state = 0;
		run_state = 0;
		//sit_state = 0;
		turn_state = 0;
		speech_Jump = 0;
		kick_state = 0;
		run_fast_state = 0;
		running_time_right = -1;
		running_time_left = -1;
		vx = walking_right_speech;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		deflect_state = 0;
		turn_state = 0;
		//sit_state = 0;
		run_state = 0;
		speech_Jump = 0;
		kick_state = 0;
		run_fast_state = 0;
		running_time_right = -1;
		running_time_left = -1;
		vx = -walking_left_speech;
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		// TODO: need to check if Mario is *current* on a platform before allowing to jump again
		deflect_state = 0;
		jump_state = 1;
		fall_state = 0;
		headup_state = 0;
		kick_state = 0;
		turn_state = 0;
		fight_state = 0;
		//run_fast_state = 0;
		running_time_right = -1;
		running_time_left = -1;
		if (maxpower_state) vy = -MARIO_JUMMP_SPEED_Y_MAX_POWER - speech_Jump;
		else vy = -MARIO_JUMP_SPEED_Y - speech_Jump;
		if (slide_state)
		{
			if (nx > 0) vx = walking_right_speech;
			else vx = -walking_left_speech;
		}
		break;
	case MARIO_STATE_IDLE:
		//streak_Kill = -1;
		deflect_state = 0;
		//pipe_state = 0;
		fly_low_state = 0;
		if (slide_state)
		{
			if (nx > 0) vx = walking_right_speech;
			else vx = -walking_left_speech;
		}
		else vx = 0;
		turn_state = 0;
		speech_Jump = 0;
		kick_state = 0;
		fight_state = 0;
		run_fast_state = 0;
		running_time_right = -1;
		running_time_left = -1;
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_STATE_RUNNING_RIGHT:
		deflect_state = 0;
		run_state = 1;
		turn_state = 0;
		speech_Jump = 0;
		kick_state = 0;
		fight_state = 0;
		sit_state = 0;
		run_fast_state = 0;
		vx = MARIO_RUNNING_SPEED;
		nx = 1;
		break;
	case MARIO_STATE_RUNNING_LEFT:
		deflect_state = 0;
		run_state = 1;
		turn_state = 0;
		speech_Jump = 0;
		kick_state = 0;
		sit_state = 0;
		run_fast_state = 0;
		vx = -MARIO_RUNNING_SPEED;
		nx = -1;
		break;
	case MARIO_STATE_KICK:
		deflect_state = 0;
		fly_high_state = 0;
		fly_low_state = 0;
		kick_state = 1;
		turn_state = 0;
		sit_state = 0;
		run_fast_state = 0;
		if (tortoiseshell != NULL) 
		{
			if (nx > 0) {
				tortoiseshell->x = x + GetCurrentWidthMario();
				tortoiseshell->SetState(KOOPA_STATE_SPIN_RIGHT);
			}
			else
			{
				tortoiseshell->x = x - KOOPA_BBOX_HIDE_WIDTH;
				tortoiseshell->SetState(KOOPA_STATE_SPIN_LEFT);
			}

			take_tortoistate_state = 0;

			tortoiseshell = NULL;
		}

		if (nx > 0) vx = MARIO_WALKING_SPEED;
		else vx = -MARIO_WALKING_SPEED;

		break;
	case MARIO_STATE_TAKE_TORTOISESHELL_RIGHT:
		deflect_state = 0;
		fly_high_state = 0;
		fly_low_state = 0;
		take_tortoistate_state = 1;
		turn_state = 0;
		speech_Jump = 0;
		sit_state = 0;
		kick_state = 0;
		fight_state = 0;
		run_fast_state = 0;
		break;
	case MARIO_STATE_TAKE_TORTOISESHELL_LEFT:
		deflect_state = 0;
		fly_high_state = 0;
		fly_low_state = 0;
		take_tortoistate_state = 1;
		turn_state = 0;
		speech_Jump = 0;
		sit_state = 0;
		kick_state = 0;
		fight_state = 0;
		run_fast_state = 0;
		break;
	case MARIO_STATE_TURN_LEFT:
		deflect_state = 0;
		fly_low_state = 0;
		turn_state = 1;
		running_time_right = -1;
		running_time_left = -1;
		if (run_state == 0)	vx = -walking_left_speech;
		else vx = -MARIO_RUNNING_SPEED;
		break;
	case MARIO_STATE_TURN_RIGHT:
		deflect_state = 0;
		fly_low_state = 0;
		turn_state = 1;
		running_time_right = -1;
		running_time_left = -1;
		if (run_state == 0)	vx = walking_right_speech;
		else vx = MARIO_RUNNING_SPEED;
		break;
	case MARIO_STATE_FIGHT:
		deflect_state = 0;
		fight_state = 1;
		if (nx > 0) vx = MARIO_WALKING_SPEED;
		else vx = -MARIO_WALKING_SPEED;
		break;
	case MARIO_STATE_RUNNING_RIGHT_FAST:
		deflect_state = 0;
		run_fast_state = 1;
		nx = 1;
		turn_state = 0;
		speech_Jump = 0;
		kick_state = 0;
		vx = MARIO_RUNNING_FAST_SPEED;
		break;
	case MARIO_STATE_RUNNING_LEFT_FAST:
		deflect_state = 0;
		run_fast_state = 1;
		nx = 0;
		turn_state = 0;
		speech_Jump = 0;
		kick_state = 0;
		vx = -MARIO_RUNNING_FAST_SPEED;
		break;
	case MARIO_STATE_FLYING_LOW_RIGHT:
		deflect_state = 0;
		nx = 1;
		fly_low_state = 1;
		fight_state = 0;
		vy = MARIO_FLYING_SPEED_Y;

		break;
	case MARIO_STATE_FLYING_LOW_LEFT:
		deflect_state = 0;
		nx = 0;
		fly_low_state = 1;
		fight_state = 0;
		vy = MARIO_FLYING_SPEED_Y;
		break;
	case MARIO_STATE_FLYING_HIGH_RIGHT:
		deflect_state = 0;
		nx = 1;
		fly_high_state = 1;
		fly_low_state = 0;
		vy = -MARIO_FLYING_SPEED_Y;
		break;
	case MARIO_STATE_FLYING_HIGH_LEFT:
		deflect_state = 0;
		nx = 0;
		fly_high_state = 1;
		fly_low_state = 0;
		vy = -MARIO_FLYING_SPEED_Y;
		break;
	case MARIO_STATE_SIT:
		deflect_state = 0;
		if (level != MARIO_LEVEL_SMALL)
		{
			if (!sit_state)
			{
				y = y + 9;
			}
		}
		sit_state = 1;

		break;
	case MARIO_STATE_SHOOT_FIRE_BULLET_RIGHT:
		deflect_state = 0;
		nx = 1;
		shoot_fire_bullet_state = 1;
		break;
	case MARIO_STATE_SHOOT_FIRE_BULLET_LEFT:
		deflect_state = 0;
		nx = 0;
		shoot_fire_bullet_state = 1;
		break;
	case MARIO_STATE_DEFLECT:
		deflect_state = 1;
		break;
	case MARIO_STATE_HEADUP:
		headup_state = 1;
		break;
	case MARIO_STATE_SMOKE:
		smoke_state = 1;
		vy = 0;
		vx = 0;
		break;
	case MARIO_STATE_GROWUP:
		growup_state = 1;
		vx = 0;
		vy = 0;
		break;
	case MARIO_STATE_PIPE_DOWN:
		pipe_down_state = 1;
		break;
	case MARIO_STATE_PIPE_UP:
		pipe_up_state = 1;
		break;
	}

	
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;

	if (level == MARIO_LEVEL_BIG)
	{
		if (sit_state)
		{
			right = x + MARIO_BIG_SIT_BBOX_WIDTH;
			bottom = y + MARIO_BIG_SIT_BBOX_HEIGHT;
		}
		else
		{
			right = x + MARIO_BIG_BBOX_WIDTH;
			bottom = y + MARIO_BIG_BBOX_HEIGHT;
		}
	}
	else if (level == MARIO_LEVEL_TAIL)
	{
		if (sit_state)
		{
			right = x + MARIO_TAIL_SIT_BBOX_WIDTH;
			bottom = y + MARIO_TAIL_SIT_BBOX_HEIGHT;
		}
		else if (fight_state)
		{
			top = y + 14;
			right = x + MARIO_TAIL_BBOX_WIDTH;
			bottom = y + MARIO_TAIL_BBOX_HEIGHT;

		}
		else
		{
			right = x + MARIO_TAIL_BBOX_WIDTH;
			bottom = y + MARIO_TAIL_BBOX_HEIGHT;
		}
	}
	else if (level == MARIO_LEVEL_FIRE)
	{
		if (sit_state)
		{
			right = x + MARIO_FIRE_SIT_BBOX_WIDTH;
			bottom = y + MARIO_FIRE_SIT_BBOX_HEIGHT;
		}
		else
		{
			right = x + MARIO_FIRE_BBOX_WIDTH;
			bottom = y + MARIO_FIRE_BBOX_HEIGHT;
		}
	}
	else if (level == MARIO_LEVEL_DARK)
	{

		if (sit_state)
		{
			right = x + MARIO_DARK_SIT_BBOX_WIDTH;
			bottom = y + MARIO_DARK_SIT_BBOX_HEIGHT;
		}
		else
		{
			right = x + MARIO_DARK_BBOX_WIDTH;
			bottom = y + MARIO_DARK_BBOX_HEIGHT;
		}
	}
	else
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void CMario::DisplayScores(int s, float x, float y, DWORD t)
{
	for (int i = 0; i < 3; i++)
	{
		if (score[i]->GetState() == SCORE_STATE_HIDEN)
		{
			score[i]->DisplayScore(s, x, y, t);
			break;
		}
	}
}

void CMario::CreateIntroAnimationMario()
{

	if (GetTickCount64() - create_time < 2500) nx = 0;
	if (GetTickCount64() - create_time < 4150 && GetTickCount64() - create_time > 2500) {
		SetState(MARIO_STATE_WALKING_LEFT);
		if (GetWalkingLeftSpeech() < MARIO_WALKING_SPEED)
			SetWalkingLeftSpeech();
	}
	if (GetTickCount64() - create_time < 4900 && GetTickCount64() - create_time > 4150) {
		SetState(MARIO_STATE_SIT);
		vx = 0;
		vy = 0;
		sit_state = 1;
	}
	if (GetTickCount64() - create_time > 4900 && GetTickCount64() - create_time < 5000) {
		if (!checkSit) {
			y -= 10;
			checkSit = 1;

		}
		SetState(MARIO_STATE_IDLE);
		SetSitState(0);
	}
	if (GetTickCount64() - create_time > 5000 && GetTickCount64() - create_time < 8400)
	{
		SetPosition(x, 138);
		vy = 0;

	}
	if (GetTickCount64() - create_time > 8400 && GetTickCount64() - create_time < 8800)
	{
		vy = 0;
		SetState(MARIO_STATE_IDLE);
	}
	if (GetTickCount64() - create_time > 8800 && GetTickCount64() - create_time < 9800)
	{
		vy = 0;
		SetState(MARIO_STATE_HEADUP);
	}
	if (GetTickCount64() - create_time > 9800 && GetTickCount64() - create_time < 10300)
	{
		SetState(MARIO_STATE_JUMP);
	}
	if (GetTickCount64() - create_time > 10300 && GetTickCount64() - create_time < 11300)
	{
		if (fly_low_state == 0) fly_low_start = (DWORD)GetTickCount64();
		vx = -MARIO_WALKING_SPEED + 0.015;
		SetState(MARIO_STATE_FLYING_LOW_LEFT);
	}
	if (GetTickCount64() - create_time > 11300 && GetTickCount64() - create_time < 11600)
	{
		fly_low_state = 0;
		SetState(MARIO_STATE_IDLE);
	}
	if (GetTickCount64() - create_time > 11600 && GetTickCount64() - create_time < 11900)
	{
		vy = -MARIO_JUMP_DEFLECT_SPEED + 0.15;
		vx = -MARIO_WALKING_SPEED + 0.04;
	}
	if (GetTickCount64() - create_time > 11900 && GetTickCount64() - create_time < 12100)
	{
		SetState(MARIO_STATE_TURN_LEFT);
	}
	if (GetTickCount64() - create_time > 12100 && GetTickCount64() - create_time < 12800)
	{
		SetState(MARIO_STATE_WALKING_RIGHT);
		if (GetWalkingRightSpeech() < MARIO_WALKING_SPEED)
			SetWalkingRightSpeech();
	}
	if (GetTickCount64() - create_time > 12800 && GetTickCount64() - create_time < 13000)
	{
		if (!kick_state) SetKickStart(GetTickCount64());
		SetState(MARIO_STATE_KICK);
	}
	if (GetTickCount64() - create_time > 14000 && GetTickCount64() - create_time < 15800)
	{
		SetState(MARIO_STATE_IDLE);
	}
	if (GetTickCount64() - create_time > 15800 && GetTickCount64() - create_time < 16200)
	{
		SetState(MARIO_STATE_WALKING_LEFT);
		if (GetWalkingLeftSpeech() < MARIO_WALKING_SPEED)
			SetWalkingLeftSpeech();
	}
	if (GetTickCount64() - create_time > 16200 && GetTickCount64() - create_time < 16450)
	{
		SetState(MARIO_STATE_JUMP);		
	}
	if (GetTickCount64() - create_time > 16450 && GetTickCount64() - create_time < 16900) {
		jump_state = 0;
	}
	if (GetTickCount64() - create_time > 16900 && GetTickCount64() - create_time < 17000) {
		SetState(MARIO_STATE_JUMP);
	}
	if (GetTickCount64() - create_time > 17000 && GetTickCount64() - create_time < 17500) {
		jump_state = 0;
		vx = 0; 
		nx = 1;
	}
	if (GetTickCount64() - create_time > 17500 && GetTickCount64() - create_time < 17800) {
		SetState(MARIO_STATE_RUNNING_RIGHT);
		//SetState(MARIO_STATE_TAKE_TORTOISESHELL_LEFT);
		if (GetWalkingRightSpeech() < MARIO_WALKING_SPEED)
			SetWalkingRightSpeech();
	}
	if (GetTickCount64() - create_time > 17800 && GetTickCount64() - create_time < 18000)
	{
		SetState(MARIO_STATE_IDLE);
		vx = 0;
		run_state = 0;
		take_tortoistate_state = 0;
		tortoiseshell = NULL;
	}
	else if (GetTickCount64() - create_time > 18200 && GetTickCount64() - create_time < 18400)
	{
		tortoiseshell = NULL;
		if (!kick_state) SetKickStart(GetTickCount64());
		SetState(MARIO_STATE_KICK);
		vx = 0;
	}
	else if (GetTickCount64() - create_time > 18400 && GetTickCount64() - create_time < 20000)
	{
		vx = 0;
	}
	else if (GetTickCount64() - create_time > 20500 && GetTickCount64() - create_time < 20700)
	{
		vx = 0;
		if (growup_state == 0)
		{
			SetLevel(MARIO_LEVEL_SMALL);
			SetState(MARIO_STATE_GROWUP);
			growup_start = (DWORD)GetTickCount64();
		}
	}
	else if (GetTickCount64() - create_time > 20800 && GetTickCount64() - create_time < 21000)
	{
		growup_state = 0;
		vx = 0;
		SetState(MARIO_STATE_IDLE);
		SetLevel(MARIO_LEVEL_SMALL);
	}
	else if (GetTickCount64() - create_time > 21000 && GetTickCount64() - create_time < 21300)
	{
		nx = -1;
	}
	else if (GetTickCount64() - create_time > 21300 && GetTickCount64() - create_time < 23000)
	{
		SetState(MARIO_STATE_WALKING_RIGHT);
		if (GetWalkingRightSpeech() < MARIO_WALKING_SPEED)
			SetWalkingRightSpeech();
	}
	//else if (GetTickCount64() - create_time > 23000 && GetTickCount64() - create_time < 23700)
	//{
	//	SetState(MARIO_STATE_WALKING_LEFT);
	//	if (GetWalkingLeftSpeech() < MARIO_WALKING_SPEED)
	//		SetWalkingLeftSpeech();
	//}



}

/*
	Reset Mario status to the beginning state of a scene
*/
void CMario::Reset()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
}

