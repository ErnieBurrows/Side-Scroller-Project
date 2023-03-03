#include "Enemy.h"
#include "Engine.h" //For static vector access
#include <iostream>
#include <SDL_mixer.h>
#include "States.h"

Enemy::Enemy(const SDL_FPoint spawn)
{
	m_src = { 0,0,64,64 };
	//First 2 numbers are location on sprite sheet, 2nd two are size of spawn
	m_dst = { spawn.x - 20.0f,spawn.y - 20.0f,64.0f,64.0f };
	//This is where is will spawn and what size
	m_bulletCtr = 0;
	m_bulletMax = 60 + rand() % 121;
	//Every 60 + 0-121 frames a bullet will spawn, etc 1-3 seconds
	m_laserEnemy2 = Mix_LoadWAV("../Assets/aud/laser2.wav");
}

void Enemy::Update(const double dt)
{
	//Enemy spawn code
	if (m_bulletCtr++ % m_bulletMax == 0) //Spawn a new enemy bullet
	{
		Mix_PlayChannel(-1, m_laserEnemy2, 0);
		GameState::m_enemyBulletVec.push_back(new Bullet({ m_dst.x + m_dst.w / 2,m_dst.y + m_dst.h / 2 }, -400, false));
	}
	//Ship movement speed

	m_dst.x += -50 * dt;

	if (m_dst.x < 0 - 64)
	{
		m_enemyDeleteMe = true;
	}

	//Update Timer and spawn Bullet.
}

SDL_Rect* Enemy::GetSrc()
{
	return &m_src;
}

SDL_FRect* Enemy::GetRect()
{
	return &m_dst;
}
