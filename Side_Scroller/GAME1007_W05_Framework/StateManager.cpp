#include "StateManager.h"

void StateManager::Update()
{
	if (!s_states.empty()) //If state vector is not empty invoke the update of the current state;
		s_states.back()->Update();
}

void StateManager::Render()
{
	if (!s_states.empty()) 
		s_states.back()->Render();
}

//Used when there needs to be two states in the vector, e.g Game and Pause
void StateManager::PushState(State* pState)
{
	s_states.push_back(pState);
	s_states.back()->Enter();//Enter is used for initizalization
}

//Used when there needs to be two states in the vector, e.g Game and Pause
void StateManager::PopState()
{
	if (s_states.size() <= 1) return; //Exit this function if there are less than two states in the vector

	s_states.back()->Exit();
	delete s_states.back();
	s_states.back() = nullptr;
	s_states.pop_back();

	s_states.back()->Resume();
}

void StateManager::ChangeState(State* pState)
{
	GameState::gameSceneActive = false;

	if (!s_states.empty()) //Changing state with a vector with one state
	{
		s_states.back()->Exit();
		delete s_states.back();
		s_states.back() = nullptr;
		s_states.pop_back();
	}

	s_states.push_back(pState);
	s_states.back()->Enter();
}

void StateManager::Quit()
{
	while (!s_states.empty())
	{
		s_states.back()->Exit();
		delete s_states.back();
		s_states.back() = nullptr;
		s_states.pop_back();
	}
}

std::vector<State*>& StateManager::GetStates()
{
	return s_states;
}

std::vector<State*> StateManager::s_states; //NOW this static object will be allocated; 
		