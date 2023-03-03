#include "Bullet.h"
#include "Engine.h"
#define WIDTH 1024
#define HEIGHT 768

Bullet::Bullet(const SDL_FPoint spawn, const double speed, bool fromPlayer)
{

	m_isPlayerBullet = fromPlayer;
	if (m_isPlayerBullet)
	{
		m_dst = { spawn.x - 10.0f,spawn.y + 10.0f,31.0f,16.0f }; // This is for player bullet
	}
	else {
		m_dst = { spawn.x,spawn.y + 2.0f,31.0f,32.0f };
		// This is for enemy bullet WILL NEED NEGATIVE SPEED FOR ENEMY	
	}
	// I will match width and height of bullet sprite here.
   //spawn.x -4 is to make sure the bullet spawns in the center of the ship
	this->m_speed = speed;
	//this-> is a pointer to the object that invokes the function.
	//this-> is optional and rarely used.
	m_dx = 1.0; //In unit vector 1.0 means full movement.
	m_dy = 0.0; //No movement on the y-axis
}

void Bullet::Update(const double dt)
{
	m_dst.x += m_dx * m_speed * dt;
	//m_dst.y += m_dy * m_speed * dt;

	if (m_dst.x > WIDTH + 32)
	{
		m_deleteMe = true;
	}
	if (IsPlayerBullet() == false && m_dst.x < 0 - 32)
	{
		m_deleteMe = true;
	}
}

SDL_FRect* Bullet::GetRect()
{
	return &m_dst;
}

bool Bullet::IsPlayerBullet()
{
	return m_isPlayerBullet;
}
