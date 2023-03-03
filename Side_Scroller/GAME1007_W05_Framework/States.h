#pragma once

//LAB2 VIDEO 2

#include "Enemy.h"
#include "Bullet.h"
#include "Engine.h"


#ifndef __STATES_H__
#define __STATES_H__

class State //Abstract base class.
{
public:
	virtual void Enter() = 0; //Pure virtual method
	virtual void Update() = 0;
	virtual void Render();
	virtual void Exit() = 0;
	virtual void Resume() {};

	
protected: //Private but inherited
	State() = default; //default is similar to = {};
};

class TitleState : public State
{
public:
	TitleState();

	virtual void Enter();
	virtual void Update();
	virtual void Render();
	virtual void Exit();

	
	

private:
	map<string, Mix_Music*>m_bgMusic;

	SDL_Texture* m_pTitleScreen, *m_pStartButton;
	SDL_FRect m_bTitleScreen, m_bStartButton;
	
};	

class PauseState : public State
{
public:
	PauseState();
	virtual void Enter();
	virtual void Update();
	virtual void Render();
	virtual void Exit();
	virtual void Resume();

private:
	
	SDL_Texture * m_pResumeButton;
	SDL_FRect m_bResumeButton;
};


class GameState : public State
{
public:
	GameState();
	virtual void Enter();
	virtual void Update();
	virtual void Render();
	virtual void Exit();
	virtual void Resume();

	static vector<Bullet*> m_enemyBulletVec;
	static bool gameSceneActive;
private:

	map<string, Mix_Music*>m_bgMusic2;
	map<string, Mix_Chunk*>m_sfx;

	vector<Bullet*> m_bulletVec; // Vector of bullet addresses held by pointers.
	vector<Enemy*> m_enemyVec; // Enemies Vectors

	SDL_Texture* m_pShipTexture, * m_pBGTexture, * m_pMissile, *m_pEnemyTexture, * m_pPauseInstructions;

	SDL_Rect m_srcShip, m_srcBG, m_dstBG;

	SDL_FRect m_bg1, m_bg2 , m_bPauseInstructions, m_dstShip; //One dest rect for each rendered background.

	SDL_FRect m_bResumeButton;
	float m_scrollSpeed; //This will be used for variable scroll speed.
	float shootTimer = 1.0f;
	double speed = 200, angle;

	int m_enemyCtr = 0, m_enemyMax = 300; //Enemy timer properties. Frame based.
	int i = 0;
	int enemiesKilled;

	bool playerIsDead;
	
};

class EndState : public State
{
public:
	EndState();
	virtual void Enter();
	virtual void Update();
	virtual void Render();
	virtual void Exit();
	virtual void Resume();

private:
	SDL_Texture* m_pMainMenuButton, *m_pEndScreen;
	SDL_FRect m_bMainMenuButton, m_bEndScreen;
	map<string, Mix_Music*>m_bgMusic;
};

class WinState : public State
{
public:
	WinState();
	virtual void Enter();
	virtual void Update();
	virtual void Render();
	virtual void Exit();
	virtual void Resume();

private:
	SDL_Texture* m_pMainMenuButton, * m_pWinScreen;
	SDL_FRect m_bMainMenuButton, m_bWinScreen;

	map<string, Mix_Music*>m_bgMusic;
};
#endif