#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "PlayScence.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"


using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	tail = NULL;
	player = NULL;
	cam = NULL;
	grid = NULL;
	map = NULL;
	time = NULL;
	score = NULL;
	arrows = NULL;
	coinPlay = NULL;
	darkEnergy = NULL;
	lives = NULL;
	sb = NULL;
	for (int i = 0; i < 2; i++)
	{
		boomerang[i] = NULL;
	}
	for (int i = 0; i < 3; i++)
	{
		cards[i] = NULL;
	}
	for (int i = 0; i < 16; i++)
	{
		pieceBrick[i] = NULL;
	}
	for (int i = 0; i < 3; i++)
	{
		scoreEffect[i] = NULL;
	}



	cardT = NULL;
	cardText = NULL;

	key_handler = new CPlayScenceKeyHandler(this);
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)2
	See scene1.txt, scene2.txt for detail format specification
*/

#define SCENE_SECTION_UNKNOWN					-1
#define SCENE_SECTION_TEXTURES					2
#define SCENE_SECTION_SPRITES					3
#define SCENE_SECTION_ANIMATIONS				4
#define SCENE_SECTION_ANIMATION_SETS			5
#define SCENE_SECTION_OBJECTS					6
#define SCENE_SECTION_ITEM_QUESTION_OBJECTS		7
#define SCENE_SECTION_ENEMIES					8
#define SCENE_SECTION_MAP						9
#define SCENE_SECTION_GRID						10

#define OBJECT_TYPE_MARIO					0
#define OBJECT_TYPE_BRICK					3
#define OBJECT_TYPE_ROAD					2
#define OBJECT_TYPE_QUESTION_BLOCK			4
#define OBJECT_TYPE_BACKGROUND				6
#define OBJECT_TYPE_COLOR_BRICK				5
#define OBJECT_TYPE_PIPE					8
#define OBJECT_TYPE_WOOD_BLOCK				7
#define OBJECT_TYPE_GOOMBA					1
#define OBJECT_TYPE_COLOR_BRICK_TOP			9
#define OBJECT_TYPE_KOOPA					10
#define OBJECT_TYPE_COIN					11
#define OBJECT_TYPE_TRANS					12
#define OBJECT_TYPE_HEADROAD				13
#define OBJECT_TYPE_CLOUD_BRICK				14
#define OBJECT_TYPE_MARIO_FIRE_BULLET		15
#define OBJECT_TYPE_FIREPIRANHAPLANT		16
#define OBJECT_TYPE_FIRE_PLANT_BULLET		17
#define OBJECT_TYPE_MUSHROOM				18
#define OBJECT_TYPE_LEAF					19
#define OBJECT_TYPE_BLUE_BRICK				20
#define OBJECT_TYPE_SCORE_BOARD				21
#define OBJECT_TYPE_PIRANHA_PLANT			22
#define OBJECT_TYPE_TIME					23
#define OBJECT_TYPE_NUMBER					24
#define OBJECT_TYPE_SCORE_EFFECT			25
#define OBJECT_TYPE_SCORE					26
#define OBJECT_TYPE_ARROW					27
#define OBJECT_TYPE_ARROWS					28
#define OBJECT_TYPE_COINPLAY				29
#define OBJECT_TYPE_LIVES					30
#define OBJECT_TYPE_SWITCH					31
#define OBJECT_TYPE_GOALCARDS				32
#define OBJECT_TYPE_DARK_ENERGY				33
#define OBJECT_TYPE_CARD					34
#define OBJECT_TYPE_CARDTEXT				35
#define OBJECT_PIECE_BRICK					36
#define OBJECT_TYPE_LONG_WOODEN_BLOCK		37
#define OBJECT_TYPE_BOOMERANG_BRO			38
#define OBJECT_TYPE_BOOMERANG				39
#define OBJECT_TYPE_FIRE_FLOWER				40
#define OBJECT_TYPE_TAIL					41

#define OBJECT_TYPE_PORTAL					50

#define MAX_SCENE_LINE						1024


void CPlayScene::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 5) return; // skip invalid lines

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	CTextures::GetInstance()->Add(texID, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void CPlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CPlayScene::_ParseSection_MAP(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 7) return; // skip invalid lines

	int idTileSet = atoi(tokens[0].c_str());
	int totalRowsTileSet = atoi(tokens[1].c_str());
	int totalColumnsTileSet = atoi(tokens[2].c_str());
	int totalRowsMap = atoi(tokens[3].c_str());
	int totalColumnsMap = atoi(tokens[4].c_str());
	int totalTiles = atoi(tokens[5].c_str());
	wstring file_path = ToWSTR(tokens[6]);

	map = new Map(idTileSet, totalRowsTileSet, totalColumnsTileSet, totalRowsMap, totalColumnsMap, totalTiles);
	map->LoadMap(file_path.c_str());
	map->ExtractTileFromTileSet();
}

void CPlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (unsigned int i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CPlayScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines - an animation set must at least id and one animation id

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new CAnimationSet();

	CAnimations* animations = CAnimations::GetInstance();

	for (unsigned int i = 1; i < tokens.size(); i++)
	{
		int ani_id = atoi(tokens[i].c_str());

		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}

	CAnimationSets::GetInstance()->Add(ani_set_id, s);
}

/*
	Parse a line in section [OBJECTS]
*/
void CPlayScene::_ParseSection_OBJECTS(string line)
{
	CGame* game = CGame::GetInstance();
	vector<string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	if (tokens.size() < 3) return; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	CGameObject* obj = NULL;


	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		if (player != NULL)
		{
			DebugOut(L"[ERROR] MARIO object was created before!\n");
			return;
		}
		obj = new CMario(x, y);
		player = (CMario*)obj;
		player->SetIntroState(0);

		DebugOut(L"[INFO] Player object created!\n");
		break;
	case OBJECT_TYPE_ROAD: 
		{
			int type = atoi(tokens[4].c_str());
			obj = new CRoad(type);
		}
		break;
	case OBJECT_TYPE_BACKGROUND: obj = new CBackgroundObject(); break;
	case OBJECT_TYPE_CARD:
		obj = new CCard();
		{
			int type = atoi(tokens[4].c_str());
			if (type == 0)
			{
				for (int i = 0; i < CARD_AMOUNT; i++)
				{
					if (cards[i] == NULL)
					{
						cards[i] = (CCard*)obj;
						break;
					}
				}
			}
			else if (type == 1)
			{
				cardT = (CCard*)obj;
			}
		}

		break;
	case OBJECT_TYPE_COLOR_BRICK: obj = new CColorBrick(); break;
	case OBJECT_TYPE_PIPE:
		{
			int type = atoi(tokens[4].c_str());
			obj = new CPipe(type);
		}
		break;
	case OBJECT_TYPE_WOOD_BLOCK: obj = new CWoodBlock(); break;
	case OBJECT_TYPE_COLOR_BRICK_TOP: obj = new CColorBrickTop(); break;
	case OBJECT_TYPE_TRANS: obj = new CTransObject(); break;
	case OBJECT_TYPE_HEADROAD: obj = new CHeadRoad(); break;
	case OBJECT_TYPE_CLOUD_BRICK: obj = new CCloudBrick(); break;
	case OBJECT_TYPE_BLUE_BRICK: obj = new CBlueBrick(); break;
	case OBJECT_TYPE_LONG_WOODEN_BLOCK: obj = new CLongWoodenBlock(); break;
	case OBJECT_TYPE_BOOMERANG: 

		obj = new CBoomerang(); 
		for (int i = 0; i < 2; i++)
		{
			if (boomerang[i] == NULL)
			{
				DebugOut(L"da tao dan cho rua\n");
				boomerang[i] = (CBoomerang*)obj;
				break;
			}
		}
		break;
	case OBJECT_TYPE_BOOMERANG_BRO:
		DebugOut(L"da tao rua\n");
		obj = new CBoomerangBro(boomerang); 
		break;
	case OBJECT_PIECE_BRICK:
		obj = new CPieceBrick();
		for (int i = 0; i < 16; i++)
		{
			if (pieceBrick[i] == NULL)
			{
				pieceBrick[i] = (CPieceBrick*)obj;
				break;
			}
		}
		break;
	case OBJECT_TYPE_GOALCARDS: 
		obj = new CGoalCard(cardText);

		break;
	case OBJECT_TYPE_DARK_ENERGY:
		obj = new CDarkEnergy();
		darkEnergy = (CDarkEnergy*)obj;
		break;
	case OBJECT_TYPE_CARDTEXT:
		obj = new CCardText(cardT);
		cardText = (CCardText*)obj;

		break;
	case OBJECT_TYPE_SCORE_BOARD: 
		obj = new CScoreBoard(); 
		sb = (CScoreBoard*)obj;
		sb->SetTime(time);
		sb->SetScore(score);
		sb->SetArrows(arrows);
		sb->SetCoinPlay(coinPlay);
		sb->SetLives(lives);
		sb->SetCards(cards);
		break;
	case OBJECT_TYPE_MARIO_FIRE_BULLET:	
		obj = new CFireBullet();
		player->CreateFireBullet(obj);
		break;
	case OBJECT_TYPE_TAIL:
		obj = new CTail();
		tail = (CTail*)obj;
		player->CreateTail(tail);
		break;
	case OBJECT_TYPE_NUMBER:
		{
			int type = atoi(tokens[4].c_str());
			obj = new CNumber();
			if (type == 0)
				num.push_back((CNumber*)obj);
			if (type == 1)
				numScore.push_back((CNumber*)obj);
			if (type == 2)
				numCoin.push_back((CNumber*)obj);
			if (type == 3)
				numLives.push_back((CNumber*)obj);
			break;
		}
	case OBJECT_TYPE_TIME:
		obj = new CTime(num);
		time = (CTime*)obj;
		break;
	case OBJECT_TYPE_SCORE:
		obj = new CScore(numScore);
		score = (CScore*)obj;
		break;
	case OBJECT_TYPE_SCORE_EFFECT:
		obj = new CScoreEffect();
		for (int i = 0; i < 3; i++)
		{
			if (scoreEffect[i] == NULL)
			{
				scoreEffect[i] = (CScoreEffect*)obj;
				scoreEffect[i]->SetScorePlay(score);
				player->CreateScore(scoreEffect[i]);
				break;
			}
		}

		break;
	case OBJECT_TYPE_ARROW:
		{
			int type = atoi(tokens[4].c_str());
			obj = new CArrow(type);
			arrow.push_back((CArrow*)obj);
			break;
		}
		break;
	case OBJECT_TYPE_ARROWS:
		obj = new CArrows(arrow);
		arrows = (CArrows*)obj;
		break;
	case OBJECT_TYPE_COINPLAY:
		obj = new CCoinPlay(numCoin);
		coinPlay = (CCoinPlay*)obj;
		break;
	case OBJECT_TYPE_LIVES:
		obj = new CLives(numLives);
		lives = (CLives*)obj;
		break;
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	if (obj != NULL)
	{
		obj->SetPosition(x, y);
		obj->SetAnimationSet(ani_set);
		obj->SetOrigin(x, y, obj->GetState());
		obj->SetisOriginObj(true);
		objects.push_back(obj);
	}

}

void CPlayScene::_ParseSection_GRID(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return;

	wstring file_path = ToWSTR(tokens[0]);

	if (grid == NULL)
		grid = new CGrid(file_path.c_str());
}

void CPlayScene::_ParseSection_ENEMIES(string line)
{
	vector<string> tokens = split(line);


	if (tokens.size() < 5) return; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());
	int state = atoi(tokens[4].c_str());

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	CGameObject* obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_GOOMBA:
		{
			obj = new CGoomba(state);
			break;
		}

	case OBJECT_TYPE_KOOPA: obj = new CKoopa(state); break;
	default:
		break;
	}

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	if (obj != NULL)
	{
		obj->SetPosition(x, y);
		obj->SetAnimationSet(ani_set);
		obj->SetOrigin(x, y, obj->GetState());
		obj->SetisOriginObj(true);
		objects.push_back(obj);
	}
}

bool CPlayScene::IsInUseArea(float Ox, float Oy)
{
	float CamX, CamY;

	CamX = cam->GetCx();

	CamY = cam->GetCy();

	if (((CamX - 10 < Ox) && (Ox < CamX + 320)) && ((CamY < Oy) && (Oy < CamY + 320)))
		return true;
	return false;
}

void CPlayScene::Load()
{
	//DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[TEXTURES]") { section = SCENE_SECTION_TEXTURES; continue; }
		if (line == "[MAP]") { section = SCENE_SECTION_MAP; continue; }
		if (line == "[GRID]") { section = SCENE_SECTION_GRID; continue; }
		if (line == "[SPRITES]") {
			section = SCENE_SECTION_SPRITES; continue;
		}
		if (line == "[ANIMATIONS]") {
			section = SCENE_SECTION_ANIMATIONS; continue;
		}
		if (line == "[ANIMATION_SETS]") {
			section = SCENE_SECTION_ANIMATION_SETS; continue;
		}
		if (line == "[OBJECTS]") {
			section = SCENE_SECTION_OBJECTS; continue;
		}
		if (line == "[ITEM_QUESTION_OBJECTS]") {
			section = SCENE_SECTION_ITEM_QUESTION_OBJECTS; continue;
		}
		if (line == "[ENEMIES]") {
			section = SCENE_SECTION_ENEMIES; continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		case SCENE_SECTION_ENEMIES: _ParseSection_ENEMIES(line); break;
		case SCENE_SECTION_MAP: _ParseSection_MAP(line); break;
		case SCENE_SECTION_GRID: _ParseSection_GRID(line); break;
		}
	}

	f.close();

	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));

	//DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
}

void CPlayScene::Update(DWORD dt)
{
	if (id == 1 || id == 4)
	{
		if (player->GetIsDead())
		{
			player->SetIsDead(false);
			CGame::GetInstance()->SetCamPos(0, 0);
			CGame::GetInstance()->SwitchScene(3);

		}
	}

	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	vector<LPGAMEOBJECT> effObjects;


	//// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;


	// Update camera to follow mario
	if (cam == NULL) {
		DebugOut(L"id: %d", id);
		cam = new CCamera(player,id);
		sb->SetCam(cam);
	}

	if (player->GetSmokeState() || player->GetGrowupState()) {
		player->Update(dt, &coObjects);
		return;
	}


	if (player->GetArrows() == NULL && !player->GetIntroState())
	{
		player->SetArrows(arrows);
	}
	if (player->GetCard() == NULL && !player->GetIntroState())
	{
		player->SetCards(cards);
	}
	if (player->GetPieceBrick() == NULL && !player->GetIntroState())
	{
		player->SetPieceBrick(pieceBrick);
	}
	if (player->GetDarkEnergy() == NULL && !player->GetIntroState())
	{
		player->SetDarkEnergy(darkEnergy);
	}
	if (player->GetCoinPlay() == NULL && !player->GetIntroState())
	{
		player->SetCoinPlay(coinPlay);
	}
	if (player->GetLives() == NULL && !player->GetIntroState())
	{
		player->SetLives(lives);
	}

	CGame* game = CGame::GetInstance();

	for (int i = 0; i < 3; i++)
	{
		cards[i]->SetState(game->GetItem(i));
	}

	for (size_t i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		float Ox, Oy;
		objects[i]->GetPosition(Ox, Oy);
		if (!IsInUseArea(Ox, Oy) && !objects[i]->GetisOriginObj() && objects[i]->GetAlive())
		{
			objects[i]->SetActive(false);
			objects.erase(objects.begin() + i);
		}
	}

	objects[0]->Update(dt, &objects);

	cam->UpdateCam(dt);

	grid->GetObjects(objects, (int)cam->GetCx(), (int)cam->GetCy());



	//objects.clear();
	
	for (size_t i = 1; i < objects.size(); i++)
	{

		objects[i]->Update(dt, &objects);

	}


	for (size_t i = 1; i < effectObjects.size(); i++)
	{
		effObjects.push_back(effectObjects[i]);
	}
	for (size_t i = 0; i < effectObjects.size(); i++)
	{
		effectObjects[i]->Update(dt, &effObjects);
	}






}

void CPlayScene::Render()
{
	if (map) map->Render();

	for (int i = objects.size() - 1; i >= 0; i--)
	{
		if (dynamic_cast<CColorBrickTop*>(objects[i]))
			objects[i]->Render();
	}
	for (int i = objects.size() - 1; i >= 0; i--)
	{
		if (!dynamic_cast<CPipe*>(objects[i]) && !dynamic_cast<CBoomerang*>(objects[i]) && !dynamic_cast<CNumber*>(objects[i])
			&& !dynamic_cast<CScoreBoard*>(objects[i]) && !dynamic_cast<CCard*>(objects[i]) && !dynamic_cast<CArrow*>(objects[i]) && !dynamic_cast<CColorBrickTop*>(objects[i]))
			objects[i]->Render();
	}
	for (int i = objects.size() - 1; i >= 0; i--)
	{
		if (dynamic_cast<CPipe*>(objects[i]))
			objects[i]->Render();
	}
	for (int i = objects.size() - 1; i >= 0; i--)
	{
		if (dynamic_cast<CBoomerang*>(objects[i]))
			objects[i]->Render();
	}
	for (int i = objects.size() - 1; i >= 0; i--)
	{
		if (dynamic_cast<CNumber*>(objects[i]) || dynamic_cast<CScoreBoard*>(objects[i]) || dynamic_cast<CCard*>(objects[i]) || dynamic_cast<CArrow*>(objects[i]))
			objects[i]->Render();
	}

}

/*
	Unload current scene
*/
void CPlayScene::Unload()
{
	for (unsigned int i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();

	player = NULL;
	cam = NULL;
	num.clear();
	numScore.clear();
	numCoin.clear();
	numLives.clear();
	arrow.clear();
	coin.clear();
	time = NULL;
	score = NULL;
	arrows = NULL;
	coinPlay = NULL;
	darkEnergy = NULL;
	lives = NULL;
	sb = NULL;
	cardT = NULL;
	cardText = NULL;

	grid->Unload();

	grid = nullptr;

	delete grid;

	for (int i = 0; i < CARD_AMOUNT; i++)
		cards[i] = NULL;
	for (int i = 0; i < 16; i++)
		pieceBrick[i] = NULL;
	for (int i = 0; i < 3; i++)
		scoreEffect[i] = NULL;
	for (int i = 0; i < 2; i++)
		boomerang[i] = NULL;
	//DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void CPlayScenceKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);

	CMario* mario = ((CPlayScene*)scence)->GetPlayer();

	CCamera* camera = ((CPlayScene*)scence)->GetCamera();
	if (mario->GetIsDead()) return;

	if (mario->GetState() == MARIO_STATE_DIE || mario->GetState() == MARIO_STATE_GROWUP || mario->GetState() == MARIO_STATE_SMOKE) return;
	switch (KeyCode)
	{
	case DIK_1:
		mario->SetPosition(50, 384);
		break;
	case DIK_2:
		mario->SetPosition(500, 336);
		break;
	case DIK_3:
		mario->SetPosition(1152, 384);
		break;
	case DIK_4:
		mario->SetPosition(2064, 320);
		break;
	case DIK_5:
		mario->SetPosition(2257, 80);
		break;
	case DIK_6:
		mario->SetPosition(1936, 310);
		break;
	case DIK_C:
		if (camera->GetAuto() == true)
			camera->SetAuto(false);
		else camera->SetAuto(true);
		break;
	case DIK_R:
		CGame::GetInstance()->SetCamPos(0, 0);
		CGame::GetInstance()->SwitchScene(3);
		break;
	case DIK_S:
		if (mario->GetFloorWood()) 	mario->SetJumpStart((DWORD)GetTickCount64());
		else if (mario->GetJumpState() == 0 && mario->GetKickState() == 0 && mario->GetFlyLowState() == 0) {
			mario->SetJumpStart((DWORD)GetTickCount64());
		}
		if (mario->GetLevel() == MARIO_LEVEL_TAIL)
		{
			if (mario->GetMaxPower())
			{
				mario->SetFlyHighStart((DWORD)GetTickCount64());
				if(mario->nx > 0)
					mario->SetState(MARIO_STATE_FLYING_HIGH_RIGHT);
				else mario->SetState(MARIO_STATE_FLYING_HIGH_LEFT);
			}

			else if (mario->vy != 0 && mario->GetJumpState())
			{
				mario->SetFlyLowStart((DWORD)GetTickCount64());
				if (mario->nx > 0)
					mario->SetState(MARIO_STATE_FLYING_LOW_RIGHT);
				else mario->SetState(MARIO_STATE_FLYING_LOW_LEFT);
			}

		}		
		break;
	case DIK_A:
		if (mario->GetLevel() == MARIO_LEVEL_TAIL && mario->GetFightState() == 0 &&
			(DWORD)GetTickCount64() - mario->GetFightStart() > 400)
		{
			mario->SetFightStart((DWORD)GetTickCount64());
			mario->SetState(MARIO_STATE_FIGHT);
		}
		if (mario->GetLevel() == MARIO_LEVEL_FIRE && !mario->GetShootFireBulletState())
		{
			mario->SetShootFireBulletStart((DWORD)GetTickCount64());
			mario->ShootFireBullet();
			if(mario->nx > 0)
				mario->SetState(MARIO_STATE_SHOOT_FIRE_BULLET_RIGHT);
			else mario->SetState(MARIO_STATE_SHOOT_FIRE_BULLET_LEFT);
		}
		break;
	case DIK_X:
		if (mario->GetLevel() == MARIO_LEVEL_SMALL) {
			mario->SetPosition(mario->x, mario->y - MARIO_SMALL_BBOX_HEIGHT);
			mario->SetLevel(MARIO_LEVEL_BIG);
		}
		else if (mario->GetLevel() == MARIO_LEVEL_BIG) {
			mario->SetPosition(mario->x, mario->y - 1);
			mario->SetLevel(MARIO_LEVEL_TAIL);
		}
		else if (mario->GetLevel() == MARIO_LEVEL_TAIL) {
			mario->SetPosition(mario->x, mario->y - MARIO_SMALL_BBOX_HEIGHT);
			mario->SetLevel(MARIO_LEVEL_FIRE);
		}
		else if (mario->GetLevel() == MARIO_LEVEL_FIRE) {
			mario->SetPosition(mario->x, mario->y);
			mario->SetLevel(MARIO_LEVEL_DARK);
			mario->GetDarkEnergy()->SetPosition(mario->x, mario->y);
			mario->GetDarkEnergy()->SetState(1);
		}
		else
		{
			mario->SetLevel(MARIO_LEVEL_SMALL);
			mario->GetDarkEnergy()->SetState(0);
		}

		break;
	case DIK_Z:
		mario->Reset();
		break;
	case DIK_RIGHT:
		if (!mario->GetIdle())
		{
			if (mario->GetWalkingRightSpeech() != 0) mario->ResetWalkingRightSpeech();
			if (mario->GetSlidingTimeRight() != 0) mario->ResetSlidingTimeRight();
		}
		break;
	case DIK_LEFT:
		if (!mario->GetIdle())
		{
			if (mario->GetWalkingLeftSpeech() != 0) mario->ResetWalkingLeftSpeech();
			if (mario->GetSlidingTimeLeft() != 0) mario->ResetSlidingTimeLeft();
		}
		break;

		
	}
}
void CPlayScenceKeyHandler::OnKeyUp(int KeyCode)
{
	CMario* mario = ((CPlayScene*)scence)->GetPlayer();

	if (mario->GetIsDead()) return;

	switch (KeyCode)
	{
	case DIK_A:
		if (mario->GetStateTakeTortoiseshell() == 1)
		{
			mario->SetKickStart((DWORD)GetTickCount64());
			mario->SetState(MARIO_STATE_KICK);
		}
		break;
	case DIK_S:
		mario->SetJumpState(-1);
		break;
	case DIK_DOWN:
		if (mario->GetSitState())
		{
			if(mario->GetLevel() != MARIO_LEVEL_SMALL) mario->y -= 10;
			mario->SetSitState(0);
		}
		break;
	default:
		break;
	}
}

void CPlayScenceKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();
	CMario* mario = ((CPlayScene*)scence)->GetPlayer();

	if (mario->GetIsDead()) return;

	// disable control key when Mario die 
	if (mario->GetState() == MARIO_STATE_DIE || mario->GetState() == MARIO_STATE_GROWUP || mario->GetState() == MARIO_STATE_SMOKE) return;
	if (game->IsKeyDown(DIK_DOWN) && mario->GetState() != MARIO_STATE_PIPE_DOWN)
	{
		if (mario->GetState() == MARIO_STATE_IDLE)
		{
			mario->SetState(MARIO_STATE_SIT);
			
		}
	}
	//if key right is down, disable key left and key down
	if (game->IsKeyDown(DIK_RIGHT) && !game->IsKeyDown(DIK_LEFT) && !game->IsKeyDown(DIK_DOWN) && !mario->GetIdle())
	{
		mario->SetWalkRightTime((DWORD)GetTickCount64());
		//mario doesn't turn left
		if ((DWORD)GetTickCount64() - mario->GetWalkLeftTime() > 200)
		{
			//mario runs right or runs fast right
			if (game->IsKeyDown(DIK_A) || game->IsKeyDown(DIK_A) && game->IsKeyDown(DIK_S))
			{
				//mario run fast right
				if (mario->GetMaxPower())
					mario->SetState(MARIO_STATE_RUNNING_RIGHT_FAST);
				else mario->SetState(MARIO_STATE_RUNNING_RIGHT); 					//mario run right
			}
			else //mario walks right
			{
				mario->ResetWalkingLeftSpeech();
				if (mario->GetWalkingRightSpeech() < MARIO_WALKING_SPEED)
					mario->SetWalkingRightSpeech();
				mario->SetState(MARIO_STATE_WALKING_RIGHT);
			}
			mario->ResetSlidingTimeLeft();
			if (mario->GetSlidingTimeRight() <= 500)
				mario->SetSlidingTimeRight();
		}
		else mario->SetState(MARIO_STATE_TURN_LEFT); 			//mario turns left

	}
	//if key left is down, disable key right and key down
	else if (game->IsKeyDown(DIK_LEFT) && !game->IsKeyDown(DIK_RIGHT) && !game->IsKeyDown(DIK_DOWN) && !mario->GetIdle())
	{
		mario->SetWalkLeftTime((DWORD)GetTickCount64());
		if (GetTickCount64() - mario->GetWalkRightTime() > 200)
		{
			if (game->IsKeyDown(DIK_A) || game->IsKeyDown(DIK_A) && game->IsKeyDown(DIK_S))
			{
				if (mario->GetMaxPower())
					mario->SetState(MARIO_STATE_RUNNING_LEFT_FAST);
				else mario->SetState(MARIO_STATE_RUNNING_LEFT);
			}
			else
			{
				mario->ResetWalkingRightSpeech();
				if (mario->GetWalkingLeftSpeech() < MARIO_WALKING_SPEED)
					mario->SetWalkingLeftSpeech();
				mario->SetState(MARIO_STATE_WALKING_LEFT);
			}
			mario->ResetSlidingTimeRight();
			if (mario->GetSlidingTimeLeft() <= 500)
				mario->SetSlidingTimeLeft();
		}
		else mario->SetState(MARIO_STATE_TURN_RIGHT);
	}
	else if (!mario->GetSlideState()) mario->SetState(MARIO_STATE_IDLE);

	if (game->IsKeyDown(DIK_S)) {
		if (GetTickCount64() - mario->GetJumpStart() < 380 && mario->GetFloorWood())
		{
			if (mario->GetSpeechJump() < 0.2)
				mario->SetSpeechJump();
			if (mario->GetJumpState() != -1 && mario->vy <= 0)
				mario->SetState(MARIO_STATE_JUMP);
		}
		else if (GetTickCount64() - mario->GetJumpStart() < 380 && !mario->GetFallState())
		{
			if (mario->GetSpeechJump() < 0.2)
				mario->SetSpeechJump();
			if (mario->GetJumpState() != -1 && mario->vy <= 0)
				mario->SetState(MARIO_STATE_JUMP);
		}
	}
}
