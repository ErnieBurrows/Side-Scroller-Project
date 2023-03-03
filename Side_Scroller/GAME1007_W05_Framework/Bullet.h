#pragma once
#ifndef _BULLET_H_
#define _BULLET_H_
#include <SDL_rect.h>

class Bullet
{
public:
	Bullet(const SDL_FPoint spawn, const double speed, bool fromPlayer = true); //Constructor.
	void Update(const double dt); //Bullet will move itself. dt = deltatime
	//void Render(); //What info would this need from the engine.
	SDL_FRect* GetRect(); //This is a getter function.
	bool IsPlayerBullet();
	bool m_deleteMe;

private:
	bool m_isPlayerBullet; //Is this bullet object fired from the player?
	SDL_FRect m_dst;
	//SDL_Rect m_src; //To use all pixels from image make src NULL.
	double m_speed, m_dx, m_dy; // dx = deltaX dy = deltaY

};
#endif

