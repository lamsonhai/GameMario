#pragma once
#include "GameObject.h"
#include "Number.h"
#include "Utils.h"
#include "Game.h"

#define SCORE_X_PlUS_8		8
#define SCORE_X_PlUS_16		16
#define SCORE_X_PlUS_24		24
#define SCORE_X_PlUS_32		32
#define SCORE_X_PlUS_40		40
#define SCORE_X_PlUS_48		48

class CScore : public CGameObject
{
	vector<CNumber*> numbers;
	int score;

public:
	CScore();
	CScore(vector<CNumber*> num) {
		score = 0;
		for (unsigned int i = 0; i < num.size(); i++)
		{
			numbers.push_back(num[i]);
		}

	}
	void PushNumbers(CNumber* num);
	void SetPositionNumbers(float x, float y);
	void SetScore(int s) { 
		CGame* game = CGame::GetInstance();
		game->SetScore(game->GetScore() + s);
	}

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b) {};
};

