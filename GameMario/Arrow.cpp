#include "Arrow.h"

void CArrow::Render()
{
	int ani;
	if (!isBlack && !isP) ani = ARROW_ANI_WHITE;
	else if (isBlack && !isP) ani = ARROW_ANI_BLACK;
	else if (!isBlack && isP) ani = ARROW_ANI_WHITE_P;
	else if (isBlack && isP) ani = ARROW_ANI_BLACK_P;

	animation_set->at(ani)->Render(x, y);
}

void CArrow::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case ARROW_STATE_BLACK:
		isBlack = 1;
		isP = 0;
		break;
	case ARROW_STATE_WHITE:
		isBlack = 0;
		isP = 0;
		break;
	case ARROW_STATE_BLACK_P:
		isBlack = 1;
		isP = 1;
		break;
	case ARROW_STATE_WHITE_P:
		isBlack = 0;
		isP = 1;
		break;
	default:
		break;
	}
}

void CArrows::SetPositionArrows(float x, float y)
{
	arrows[0]->SetPosition(x, y);
	arrows[1]->SetPosition(x + ARROW_POS_X_PLUS_8, y);
	arrows[2]->SetPosition(x + ARROW_POS_X_PLUS_16, y);
	arrows[3]->SetPosition(x + ARROW_POS_X_PLUS_24, y);
	arrows[4]->SetPosition(x + ARROW_POS_X_PLUS_32, y);
	arrows[5]->SetPosition(x + ARROW_POS_X_PLUS_40, y);
	arrows[6]->SetPosition(x + ARROW_POS_X_PLUS_48, y);
}

void CArrows::SetWhiteArrows()
{
	for (unsigned int i = 0; i < arrows.size(); i++)
	{
		if (arrows[i]->GetIsBlack())
		{
			arrows[i]->SetWhiteState();
			break;
		}
	}
}

void CArrows::SetBlackArrows()
{
	for (int i = arrows.size() - 1; i >= 0; i--)
	{
		if (!arrows[i]->GetIsBlack())
		{
			arrows[i]->SetBlackState();
			break;
		}
	}
}

void CArrows::ResetArrows()
{
	for (int i = arrows.size() - 1; i >= 0; i--)
	{
		if (!arrows[i]->GetIsBlack())
		{
			arrows[i]->SetBlackState();
		}
	}
}

void CArrows::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);


	if (arrows[6] != NULL)
	{
		if (GetPState()) SetPStart((DWORD)GetTickCount64());
		else if (!GetPState())
		{
			if (GetTickCount64() - p_start > ARROW_RESET_P_TIME) ResetArrows();
		}
	}

}