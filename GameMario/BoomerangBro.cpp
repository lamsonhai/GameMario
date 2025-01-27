#include "BoomerangBro.h"
#include "Mario.h"


CBoomerangBro::CBoomerangBro(CBoomerang* boom[BOOMERANG_AMOUNT])
{
	is_back = 0;
	is_idle = 0;
	is_right = 0;
	time_start = (DWORD)GetTickCount64();
	found_player = false;
	for (int i = 0; i < BOOMERANG_AMOUNT; i++)
	{
		boomerang[i] = boom[i];
	}
	is_shoot = 0;
}

void CBoomerangBro::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == BOOMERANGBRO_STATE_DIE)
	{
		left = x;
		top = y;
		right = 0;
		bottom = 0;
	}
	else
	{
		left = x;
		top = y;
		right = x + BOOMERANGBRO_BBOX_WIDTH;
		bottom = y + BOOMERANGBRO_BBOX_HEIGHT;
	}

}


void CBoomerangBro::FilterCollision(
	vector<LPCOLLISIONEVENT>& coEvents,
	vector<LPCOLLISIONEVENT>& coEventsResult,
	float& min_tx, float& min_ty,
	float& nx, float& ny, float& rdx, float& rdy)
{
	min_tx = 1.0f;
	min_ty = 1.0f;
	int min_ix = -1;
	int min_iy = -1;

	nx = 0.0f;
	ny = 0.0f;
	coEventsResult.clear();

	for (UINT i = 0; i < coEvents.size(); i++)
	{
		LPCOLLISIONEVENT c = coEvents[i];

		if (dynamic_cast<CMario*>(c->obj))
		{
			if (c->nx != 0)
			{
				if (c->t < 1.0f)
				{
					min_tx = c->t; nx = c->nx; rdx = c->dx;
					coEventsResult.push_back(coEvents[i]);
				}
			}

			if (c->ny != 0)
			{
				if (c->t < 1.0f)
				{
					min_ty = c->t; ny = c->ny; rdy = c->dy;
					coEventsResult.push_back(coEvents[i]);
				}
			}
		}
		if (dynamic_cast<CRoad*>(c->obj) || dynamic_cast<CPipe*>(c->obj)
			|| dynamic_cast<CKoopa*>(c->obj) || dynamic_cast<CFireBullet*>(c->obj)
			|| dynamic_cast<CGoomba*>(c->obj)
			|| dynamic_cast<CQuestionBlock*>(c->obj)) {
			if (c->nx != 0)
			{
				if (c->t < 1.0f)
				{
					min_tx = c->t; nx = c->nx; rdx = c->dx;
					coEventsResult.push_back(coEvents[i]);
				}
			}

			if (c->ny != 0)
			{
				if (c->t < 1.0f)
				{
					min_ty = c->t; ny = c->ny; rdy = c->dy;
					coEventsResult.push_back(coEvents[i]);
				}
			}
		}
		if (dynamic_cast<CHeadRoad*>(c->obj))
		{
			if (c->nx != 0) {
				if (c->t < 1.0f)
				{
					min_tx = c->t; nx = c->nx; rdx = c->dx;
					coEventsResult.push_back(coEvents[i]);
				}
			}
		}
	}
}

void CBoomerangBro::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	vy += BOOMERANGBRO_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();




	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;

	}
	else if (state != BOOMERANGBRO_STATE_DIE)
	{
		float min_tx, min_ty, nx, ny;
		float rdx = 0;
		float rdy = 0;

		CBoomerangBro::FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		x += min_tx * dx + nx * 0.1f;
		y += min_ty * dy + ny * 0.1f;

		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
		}

		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

		if (GetTickCount64() - time_start < BOOMERANGBRO_GOAHEAD_TIME)
		{
			is_back = 0;
			if (mario->x - x >= BOOMERANGBRO_RANGE_ATTACK_PLAYER_RIGHT)
			{
				is_right = 1;
				SetState(BOOMERANGBRO_STATE_WALKING);
				vx = BOOMERANG_WALKING_SPEED;
			}
			else if (mario->x - x <= BOOMERANGBRO_RANGE_ATTACK_PLAYER_LEFT)
			{
				is_right = 0;
				SetState(BOOMERANGBRO_STATE_WALKING);
				vx = -BOOMERANG_WALKING_SPEED;
			}

			//SetState(BOOMERANGBRO_STATE_THROWING);
			if (!is_shoot)
			{
				for (int i = 0; i < BOOMERANG_AMOUNT; i++)
				{
					if (boomerang[i]->GetState() == BOOMERANG_STATE_HIDEN)
					{
						if (is_right)
						{
							is_shoot = 1;
							boomerang[i]->SetPosition(x - BOOMERANGBRO_BOOMERANG_LEFT_X, y - BOOMERANGBRO_BOOMERANG_LEFT_Y);
							boomerang[i]->SetState(BOOMERANG_STATE_FLYING_RIGHT);
							boomerang[i]->SetShootStart((DWORD)GetTickCount64());
							break;
						}
						else
						{
							is_shoot = 1;
							boomerang[i]->SetPosition(x + BOOMERANGBRO_BOOMERANG_RIGHT_X, BOOMERANGBRO_BOOMERANG_RIGHT_Y - 8);
							boomerang[i]->SetState(BOOMERANG_STATE_FLYING_LEFT);
							boomerang[i]->SetShootStart((DWORD)GetTickCount64());
							break;
						}

					}
				}
			}

		}
		else if (GetTickCount64() - time_start < BOOMERANGBRO_GOBACK_TIME)
		{
			is_shoot = 0;
			is_back = 1;
			if (mario->x - x >= BOOMERANGBRO_RANGE_ATTACK_PLAYER_RIGHT)
			{
				is_right = 1;
				SetState(BOOMERANGBRO_STATE_WALKING);
				vx = -BOOMERANG_WALKING_SPEED;
			}
			else if (mario->x - x <= BOOMERANGBRO_RANGE_ATTACK_PLAYER_LEFT)
			{
				is_right = 0;
				SetState(BOOMERANGBRO_STATE_WALKING);
				vx = BOOMERANG_WALKING_SPEED;
			}
		}
		else if (GetTickCount64() - time_start < BOOMERANGBRO_THROWN_BOOMERANG_TIME)
		{

			SetState(BOOMERANGBRO_STATE_THROWING);
			if (!is_shoot)
			{
				for (int i = 0; i < BOOMERANG_AMOUNT; i++)
				{
					if (boomerang[i]->GetState() == BOOMERANG_STATE_HIDEN)
					{
						if (is_right)
						{
							is_shoot = 1;
							boomerang[i]->SetPosition(x - BOOMERANGBRO_BOOMERANG_LEFT_X, BOOMERANGBRO_BOOMERANG_LEFT_Y - 8);
							boomerang[i]->SetState(BOOMERANG_STATE_FLYING_RIGHT);
							boomerang[i]->SetShootStart((DWORD)GetTickCount64());
							break;
						}
						else
						{
							is_shoot = 1;
							boomerang[i]->SetPosition(x + BOOMERANGBRO_BOOMERANG_RIGHT_X, BOOMERANGBRO_BOOMERANG_RIGHT_Y - 8);
							boomerang[i]->SetState(BOOMERANG_STATE_FLYING_LEFT);
							boomerang[i]->SetShootStart((DWORD)GetTickCount64());
							break;
						}

					}
				}
			}

		}
		else if (GetTickCount64() - time_start < BOOMERANGBRO_RESET)
		{
			is_shoot = 0;
			time_start = (DWORD)GetTickCount64();
		}
		else
		{
			vx = 0;
			vy = 0;
			time_start = (DWORD)GetTickCount64();
		}
	}

	


}

void CBoomerangBro::Render()
{
	int ani = BOOMERANGBRO_ANI_WALKING_RIGHT;
	if (state == BOOMERANGBRO_STATE_DIE && nx > 0) ani = BOOMERANGBRO_ANI_DIE_LEFT;
	else if (state == BOOMERANGBRO_STATE_DIE && nx < 0) ani = BOOMERANGBRO_ANI_DIE_RIGHT;
	else if (vx < 0 && state == BOOMERANGBRO_STATE_WALKING && !is_idle && !is_back) ani = BOOMERANGBRO_ANI_WALKING_LEFT;
	else if (vx < 0 && state == BOOMERANGBRO_STATE_WALKING && !is_idle && is_back) ani = BOOMERANGBRO_ANI_WALKING_RIGHT;
	else if (vx > 0 && state == BOOMERANGBRO_STATE_WALKING && !is_idle && !is_back) ani = BOOMERANGBRO_ANI_WALKING_RIGHT;
	else if (vx > 0 && state == BOOMERANGBRO_STATE_WALKING && !is_idle && is_back) ani = BOOMERANGBRO_ANI_WALKING_LEFT;
	else if (!is_right) ani = BOOMERANGBRO_ANI_THROWING_LEFT;
	else if (is_right) ani = BOOMERANGBRO_ANI_THROWING_RIGHT;

	animation_set->at(ani)->Render(x, y);
}


void CBoomerangBro::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BOOMERANGBRO_STATE_WALKING:
		is_idle = 0;
		break;
	case BOOMERANGBRO_STATE_THROWING:
		is_idle = 1;
		vx = 0;
		break;
	case BOOMERANGBRO_STATE_DIE:
		is_idle = 1;
		break;
	default:
		break;
	}
}