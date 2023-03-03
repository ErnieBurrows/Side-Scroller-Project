#include "States.h"

#include "StateManager.h"
#include "EventManager.h"
#include "CollisionManager.h"
#include <iostream>;


using namespace std;

vector<Bullet*> GameState::m_enemyBulletVec; // Enemies bullets
bool GameState::gameSceneActive = false;

void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}

/*TITLE STATE*/
TitleState::TitleState(){}
void TitleState::Enter()
{
	m_bgMusic.emplace("titleMusic", Mix_LoadMUS("../Assets/aud/SaveYourTears.mp3"));

	m_pTitleScreen = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/TitleScreen.bmp"); //The '.' takes you back 1 folder directory.
	m_bTitleScreen = { 0.0f,0.0f,1024.0f,768.0f };

	m_pStartButton = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/Start.bmp"); //The '.' takes you back 1 folder directory.
	m_bStartButton = { 896.0f-256.0f*2,672.0f-92.0f,256.0f,92.0f };
	
	Mix_PlayMusic(m_bgMusic["titleMusic"], -1); // 0-n for # of loops, or -1 for infinite looping.
	Mix_VolumeMusic(50); // 0-128.

}

void TitleState::Update()
{
	if (EVMA::MousePressed(1) && COMA::PointAABBCheck(EVMA::GetMousePos(), m_bStartButton))
	{
		STMA::ChangeState(new GameState());
	}
}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 255, 255); //THIS IS JUST RENDERING 1 COLOUR ON THE SCREEN.
	SDL_RenderClear(Engine::Instance().GetRenderer());
	//ANY UNIQUE RENDERING GOES HER

	SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pTitleScreen, NULL, &m_bTitleScreen);
	SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pStartButton, NULL, &m_bStartButton);

	
	State::Render(); //This invokes SDL_RenderPresent
}

void TitleState::Exit()
{
	//Freeing the music when leaving the scene.
	for (auto const& i : m_bgMusic)
	{
		Mix_FreeMusic(i.second); //Second is the mix chunck pointer
	}
}


/*GAME STATE*/
GameState::GameState() {}
void GameState::Enter()
{
	enemiesKilled = 0;
	cout << "Welcome to this super fun game. Your goal is to kill 10 enemies without them killing you first\nBelow we will count how many enemies you have killed.\n";
	cout << "Enemies killed: " << enemiesKilled << endl;
	playerIsDead = false;
	m_pPauseInstructions = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/Pause.bmp"); //The '.' takes you back 1 folder directory.
	m_bPauseInstructions = { 1024.0f - 192.0f ,768.0f - 69.0f,192.0f,69.0f };


	/*Needed so the game doesn't crash when scene is changed*/
	gameSceneActive = true;

	//Loading sound fx
	m_sfx.emplace("fx1", Mix_LoadWAV("../Assets/aud/laser.wav")); 
	m_sfx.emplace("fx2", Mix_LoadWAV("../Assets/aud/laser2.wav")); 
	m_sfx.emplace("explosion", Mix_LoadWAV("../Assets/aud/explosion.wav"));
	m_bgMusic2.emplace("bgMusic2", Mix_LoadMUS("../Assets/aud/BlindingLights.mp3"));
	Mix_PlayMusic(m_bgMusic2["bgMusic2"], -1); // 0-n for # of loops, or -1 for infinite looping.
	Mix_VolumeMusic(50); // 0-128.

	/*LOADING SPRITES*/
	m_pShipTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/ship.png"); //The '.' takes you back 1 folder directory.
	m_pBGTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/background.png");
	m_pMissile = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/missile.png");
	m_pEnemyTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/enemy.png");

	/*Setting Ship location*/
	m_dstShip = { WIDTH / 10.0f - 64.0f, HEIGHT / 2.0f - 64.0f / 2.0,64.0f,64.0f }; //x, y, w, h

	//Set BG1 and BG2 POS
	m_bg1 = { 0.0f,0.0f,1024.0f,768.0f };
	m_bg2 = { 1024.0f,0.0f,1024.0f,768.0f };
	m_scrollSpeed = -500;
}

void GameState::Update()
{
	/*TO ENTER PAUSESTATE*/
	if (Engine::Instance().KeyDown(SDL_SCANCODE_P) || EVMA::MousePressed(1) && COMA::PointAABBCheck(EVMA::GetMousePos(), m_bPauseInstructions)) //THIS WILL TAKE US TO THE PAUSE STATE
	{
		Mix_PauseMusic();
		STMA::PushState(new PauseState());
	}

	/*TOO ENTER ENDSTATE*/
	if (playerIsDead)
	{
		STMA::ChangeState(new EndState());
	}

	if (enemiesKilled > 9)
	{
		STMA::ChangeState(new WinState());
	}

	//Scroll backgrounds
	m_bg1.x += m_scrollSpeed * Engine::Instance().deltaTime;
	m_bg2.x += m_scrollSpeed * Engine::Instance().deltaTime;

	//Check if backgrounds need to reset
	if (m_bg1.x <= -1024.0f) //Checks if bg1 is beyond its limit and resets the y value for both bg's
	{
		m_bg1.x = 0.0f;
		m_bg2.x = 1024.0f;
	}

	for (auto enemy : m_enemyVec)
	{
		enemy->Update(Engine::Instance().deltaTime);
	}
	for (auto bullet : m_bulletVec) // For all elements of vector
	{
		bullet->Update(Engine::Instance().deltaTime); // -> is the dot (.) opertator for pointers.
	}
	for (auto bullet : m_enemyBulletVec)
	{
		bullet->Update(Engine::Instance().deltaTime);
	}

	/*Player Movement*/
	if (Engine::Instance().KeyDown(SDL_SCANCODE_W) && m_dstShip.y > 0)
	{
		m_dstShip.y -= speed * Engine::Instance().deltaTime; //Speed per second, not per frame.
	}
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_S) && m_dstShip.y < HEIGHT - 64) //64 is this ship length
	{
		m_dstShip.y += speed * Engine::Instance().deltaTime;
	}
	if (Engine::Instance().KeyDown(SDL_SCANCODE_A) && m_dstShip.x > 0)
	{
		m_dstShip.x -= speed * Engine::Instance().deltaTime;
	}
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_D) && m_dstShip.x < WIDTH / 2 - 64) //64 is the ship width
	{
		m_dstShip.x += speed * Engine::Instance().deltaTime;
	}

	/*Player Shoot Function i is to prevent spam*/
	if (!Engine::Instance().KeyDown(SDL_SCANCODE_SPACE))		
	i = 0;
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_SPACE) && !playerIsDead && i ==0)
	{
		m_bulletVec.push_back(new Bullet({ m_dstShip.x + m_dstShip.w / 2,m_dstShip.y + m_dstShip.h / 2 }, 400));
		Mix_PlayChannel(-1, m_sfx["fx1"], 0);
		i = 1;
	}

	//Deleting player bullets when off screen
	for (unsigned i = 0; i < m_bulletVec.size(); i++)
	{
		if (m_bulletVec[i]->m_deleteMe == true)
		{
			delete m_bulletVec[i];
			m_bulletVec[i] = nullptr;
			m_bulletVec.erase(m_bulletVec.begin() + i);
			break;
		}
	}
	//Deleting enemy ships when off Screen
	for (unsigned i = 0; i < m_enemyVec.size(); i++)
	{
		if (m_enemyVec[i]->m_enemyDeleteMe == true)
		{
			delete m_enemyVec[i];
			m_enemyVec[i] = nullptr;
			m_enemyVec.erase(m_enemyVec.begin() + i);
			break;
		}
	}
	//Delete enemy bullets when off screen
	for (unsigned i = 0; i < m_enemyBulletVec.size(); i++)
	{
		if (m_enemyBulletVec[i]->m_deleteMe == true)
		{
			delete m_enemyBulletVec[i];
			m_enemyBulletVec[i] = nullptr;
			m_enemyBulletVec.erase(m_enemyBulletVec.begin() + i);
			break;
		}
	}
	// Delete enemy ship on collision from player bullet
	for (unsigned i = 0; i < m_bulletVec.size(); i++)
	{
		for (unsigned j = 0; j < m_enemyVec.size(); j++)
		{
			if (!playerIsDead && SDL_HasIntersectionF(m_enemyVec[j]->GetRect(), m_bulletVec[i]->GetRect()))
			{
				
				Mix_PlayChannel(-1, m_sfx["explosion"], 0);
				delete m_bulletVec[i];
				m_bulletVec[i] = nullptr;
				m_bulletVec.erase(m_bulletVec.begin() + i);
				delete m_enemyVec[j];
				m_enemyVec[j] = nullptr;
				m_enemyVec.erase(m_enemyVec.begin() + j);
				enemiesKilled++;
				cout << "Enemies killed: " << enemiesKilled << endl;
				goto end;
			}
		}
	}
end:
	// Delete Player and enemy bullet on enemy bullet collision
	for (unsigned i = 0; i < m_enemyBulletVec.size(); i++)
	{
		if (!playerIsDead && SDL_HasIntersectionF(m_enemyBulletVec[i]->GetRect(), &m_dstShip))
		{
			Mix_PlayChannel(-1, m_sfx["explosion"], 0);
			delete m_enemyBulletVec[i];
			m_enemyBulletVec[i] = nullptr;
			m_enemyBulletVec.erase(m_enemyBulletVec.begin() + i);
			SDL_DestroyTexture(m_pShipTexture);
			playerIsDead = true;
			goto end2;
		}
	}
end2:
	// Player Die on collision with enemy
	for (unsigned i = 0; i < m_enemyVec.size(); i++)
	{
		if (!playerIsDead && SDL_HasIntersectionF(m_enemyVec[i]->GetRect(), &m_dstShip))
		{
			SDL_DestroyTexture(m_pShipTexture);
			Mix_PlayChannel(-1, m_sfx["explosion"], 0);
			playerIsDead = true;
			goto end3;
		}
	}
end3:

	if (gameSceneActive)
	{
		if (m_enemyCtr++ % m_enemyMax == 0) //Spawn a new enemy
		{
			m_enemyVec.push_back(new Enemy({ WIDTH,(float)(rand() % 711) })); // Random spawn ADD +64 TO WIDTH
		}
	}
	//Enemy spawn code
}

void GameState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 255, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());

	//Render the backgrounds
	SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pBGTexture, NULL, &m_bg1);
	SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pBGTexture, NULL, &m_bg2);

	/*Rendering The Ship*/
	SDL_RenderCopyExF(Engine::Instance().GetRenderer(), m_pShipTexture, NULL, &m_dstShip, angle, NULL, SDL_FLIP_NONE);
	//SDL_RenderPresent(Engine::Instance().GetRenderer()); // Flips the buffers.

	SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pPauseInstructions, NULL, &m_bPauseInstructions);

	//Render all enemies
	SDL_SetTextureColorMod(m_pEnemyTexture, 0, 0, 255);
	for (auto enemy : m_enemyVec)
	{
		SDL_RenderCopyExF(Engine::Instance().GetRenderer(), m_pEnemyTexture, enemy->GetSrc(), enemy->GetRect(), angle, NULL, SDL_FLIP_HORIZONTAL);
	}
	for (auto bullet : m_bulletVec) // For all elements of vector
	{
		SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pMissile, NULL, bullet->GetRect()); //This is BULLET image we are rendering. 
	}
	for (auto bullet : m_enemyBulletVec) // For all elements of vector
	{
		SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), rand() % 256, rand() % 256, rand() % 256, 255);
		SDL_RenderCopyExF(Engine::Instance().GetRenderer(), m_pMissile, NULL, bullet->GetRect(), NULL, NULL, SDL_FLIP_HORIZONTAL); //This is BULLET image we are rendering. 
	}

	//if The current state is the gamestate 
	if (dynamic_cast<GameState*>(STMA::GetStates().back()))
		State::Render();
}

void GameState::Exit()
{
	cout << endl << endl;
	for (auto const& i : m_sfx)
	{
		Mix_FreeChunk(i.second); //Second is the mix chunck pointer
	}
	//Freeing the music when leaving the scene.
	for (auto const& i : m_bgMusic2)
	{
		Mix_FreeMusic(i.second); //Second is the mix chunck pointer
	}


	SDL_DestroyTexture(m_pShipTexture);
	SDL_DestroyTexture(m_pBGTexture);
	SDL_DestroyTexture(m_pMissile);
	SDL_DestroyTexture(m_pEnemyTexture);

	for (auto bullet : m_bulletVec)
	{
		delete bullet; // Deallocate bullet element;
		bullet = nullptr; //Restting our dangline pointer.
	}
	m_bulletVec.clear(); //Clear all elements and sets the size to 0
	m_bulletVec.shrink_to_fit(); // Shrink capacity to size. capacity=0
	
	for (auto enemy : m_enemyVec)
	{
		delete enemy; // Deallocate bullet element;
		enemy = nullptr; //Restting our dangline pointer.
	}
	m_enemyVec.clear(); //Clear all elements and sets the size to 0
	m_enemyVec.shrink_to_fit(); // Shrink capacity to size. capacity=0
	
	for (auto bullet : m_enemyBulletVec)
	{
		delete bullet; // Deallocate bullet element;
		bullet = nullptr; //Restting our dangline pointer.
	}
	m_enemyBulletVec.clear(); //Clear all elements and sets the size to 0
	m_enemyBulletVec.shrink_to_fit(); // Shrink capacity to size. capacity=0
}

void GameState::Resume()
{
	Mix_ResumeMusic();
}


/*END STATE*/
EndState::EndState(){}
void EndState::Enter()
{

	m_bgMusic.emplace("thehills", Mix_LoadMUS("../Assets/aud/TheHills.mp3"));
	Mix_PlayMusic(m_bgMusic["thehills"], -1); // 0-n for # of loops, or -1 for infinite looping.
	Mix_VolumeMusic(50); // 0-128.

	m_pEndScreen = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/EndScreen.bmp"); //The '.' takes you back 1 folder directory.
	m_bEndScreen = { 0.0f,0.0f,1024.0f,768.0f };

	m_pMainMenuButton = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/MainMenu.bmp"); //The '.' takes you back 1 folder directory.
	m_bMainMenuButton = { (1024/2)-(256/2), (768 / 2) + (92),256.0f,92.0f};
}

void EndState::Update()
{

	if (EVMA::MousePressed(1) && COMA::PointAABBCheck(EVMA::GetMousePos(), m_bMainMenuButton))
	{
		STMA::ChangeState(new TitleState());
	}
}

void EndState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 0, 255); //THIS IS JUST RENDERING 1 COLOUR ON THE SCREEN.
	
	SDL_RenderClear(Engine::Instance().GetRenderer());
	//ANY UNIQUE RENDERING GOES HER
	SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pEndScreen, NULL, &m_bEndScreen);
	SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pMainMenuButton, NULL, &m_bMainMenuButton);

	State::Render(); //This invokes SDL_RenderPresent
}

void EndState::Exit()
{
}

void EndState::Resume()
{
}


/*PAUSE STATE*/
PauseState::PauseState(){}
void PauseState::Enter()
{
	

	m_pResumeButton = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/Resume.bmp"); //The '.' takes you back 1 folder directory.
	m_bResumeButton = { 896.0f - 256.0f * 2,672.0f - 92.0f,256.0f,92.0f };
}

void PauseState::Update()
{
	if (EVMA::MousePressed(1) && COMA::PointAABBCheck(EVMA::GetMousePos(), m_bResumeButton))
	{
		STMA::PopState();
	}
}

void PauseState::Render()
{
	//Render the scene at the fron of the vector i.e game scene
	STMA::GetStates().front()->Render();

	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 30, 40, 100, 100);
	SDL_Rect pauseRect = { 0.0f, 0.0f, 1024.0f, 768.0f};
	SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pResumeButton, NULL, &m_bResumeButton);
	SDL_SetRenderDrawBlendMode(Engine::Instance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &pauseRect);

	State::Render();	
}

void PauseState::Exit()
{
}

void PauseState::Resume()
{
}

/*WIN STATE*/
WinState::WinState() {}
void WinState::Enter()
{
	m_bgMusic.emplace("starboy", Mix_LoadMUS("../Assets/aud/StarBoy.mp3"));
	Mix_PlayMusic(m_bgMusic["starboy"], -1); // 0-n for # of loops, or -1 for infinite looping.
	Mix_VolumeMusic(50); // 0-128.


	m_pWinScreen = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/WinScreen.bmp"); //The '.' takes you back 1 folder directory.
	m_bWinScreen = { 0.0f,0.0f,1024.0f,768.0f };

	m_pMainMenuButton = IMG_LoadTexture(Engine::Instance().GetRenderer(), "../Assets/img/MainMenu.bmp"); //The '.' takes you back 1 folder directory.
	m_bMainMenuButton = { (1024 / 2) - (256 / 2), (768 / 2) + (92),256.0f,92.0f };
}

void WinState::Update()
{
	if (EVMA::MousePressed(1) && COMA::PointAABBCheck(EVMA::GetMousePos(), m_bMainMenuButton))
	{
		STMA::ChangeState(new TitleState());
	}
}

void WinState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 0, 255); //THIS IS JUST RENDERING 1 COLOUR ON THE SCREEN.

	SDL_RenderClear(Engine::Instance().GetRenderer());
	//ANY UNIQUE RENDERING GOES HER
	SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pWinScreen, NULL, &m_bWinScreen);
	SDL_RenderCopyF(Engine::Instance().GetRenderer(), m_pMainMenuButton, NULL, &m_bMainMenuButton);


	State::Render(); //This invokes SDL_RenderPresent
}

void WinState::Exit()
{
}

void WinState::Resume()
{
}


