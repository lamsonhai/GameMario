#include "QuestionBlock.h"
#include "Coin.h"
#include "Mushroom.h"
#include "Leaf.h"
#include "Utils.h"
#include "Switch.h"
#include "FireFlower.h"

void CQuestionBlock::Render()
{
	if (form && state == QUESTIONBLOCK_ITEM_STATE) animation_set->at(2)->Render(x, y);
	else if (state == QUESTIONBLOCK_ITEM_STATE) 
		animation_set->at(0)->Render(x, y);
	else animation_set->at(1)->Render(x, y);
}

void CQuestionBlock::AddItemQuestionBlock(CGameObject* itemqusblock)
{
	for (int i = 0; i < ITEM_AMOUNT; i++)
	{
		if (item[i] == NULL)
		{
			item[i] = itemqusblock;
			return;
		}
	}
}

void CQuestionBlock::SetStateItem(int state)
{
	for (int i = 0; i < ITEM_AMOUNT; i++)
	{
		if (item[i] != NULL)
		{
			if (item[i]->GetState() == COIN_STATE_HIDEN && dynamic_cast<CCoin*>(item[i]))
			{
				item[i]->SetPosition(x + QUESTIONBLOCK_POS_PLUS_4, y - QUESTIONBLOCK_POS_PLUS_16);
				item[i]->SetState(state);
				item[i] = NULL;
				break;
			}
			else if (item[i]->GetState() == LEAF_STATE_HIDEN && dynamic_cast<CLeaf*>(item[i]))
			{
				item[i]->SetPosition(x, y - QUESTIONBLOCK_POS_PLUS_4);
				item[i]->SetState(state);
				item[i] = NULL;
				break;
			}
			else if (item[i]->GetState() == MUSHROOM_STATE_HIDEN && dynamic_cast<CMushroom*>(item[i]))
			{
				item[i]->SetPosition(x, y - QUESTIONBLOCK_POS_PLUS_4);
				item[i]->SetState(state);
				item[i] = NULL;
				break;
			}
			else if (item[i]->GetState() == SWITCH_STATE_HIDEN && dynamic_cast<CSwitch*>(item[i]))
			{
				item[i]->SetPosition(x, y - QUESTIONBLOCK_POS_PLUS_16);
				item[i]->SetState(state);
				item[i] = NULL;
				break;
			}
			else if (item[i]->GetState() == FIRE_FLOWER_STATE_HIDEN && dynamic_cast<CFireFlower*>(item[i]))
			{
				item[i]->SetPosition(x, y - QUESTIONBLOCK_POS_PLUS_16);
				item[i]->SetState(state);
				item[i] = NULL;
				break;
			}
		}
		

	}
}

void CQuestionBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	if (!check_init_location)
	{
		init_location_y = y;
		check_init_location = true;
	}

	if (state != QUESTIONBLOCK_ITEM_STATE)
	{
		if (state == QUESTIONBLOCK_DEFLECT_STATE)
		{
			if (y > init_location_y - QUESTIONBLOCK_POS_PLUS_8) y += dy;
			else
			{
				SetState(QUESTIONBLOCK_FALL_STATE);
				if (y < init_location_y) {
					y = init_location_y;
					if(!CheckItem()) SetState(QUESTIONBLOCK_NORMAL_STATE);
					else SetState(QUESTIONBLOCK_ITEM_STATE);
				}
				else y += dy;
			}
		}
		else SetState(QUESTIONBLOCK_NORMAL_STATE);
	}

	
}

void CQuestionBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + QUESTIONBLOCK_BBOX_WIDTH;
	b = y + QUESTIONBLOCK_BBOX_HEIGHT;
}

void CQuestionBlock::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case QUESTIONBLOCK_ITEM_STATE:
		vy = 0;
		break;
	case QUESTIONBLOCK_DEFLECT_STATE:
		vy = -QUESTIONBLOCK_DEFLECT_SPEECH;
		break;
	case QUESTIONBLOCK_FALL_STATE:
		vy = QUESTIONBLOCK_FALL_SPEECH;
		break;
	case QUESTIONBLOCK_NORMAL_STATE:
		vy = 0;
		break;
	default:
		break;
	}
}