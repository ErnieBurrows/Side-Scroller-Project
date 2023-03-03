#pragma once
#ifndef __STATEMANAGER_H__
#define __STATEMANAGER_H__

#include "States.h"
#include <vector>

class StateManager //This is our Finite state machine
{ // Static class not a singleton.
public:
	static void Update();
	static void Render();
	/*Specific StateManafer methods:*/
	static void PushState(State* pState);
	static void PopState(); //Used to go from Pause back to the game
	static void ChangeState(State* pState);
	static void Quit();

	/*Accessor for states*/
	static std::vector<State*>& GetStates();
private:
	StateManager() = default;
	static std::vector<State*> s_states; //use s_name to show it is static
	//NOTE: The "back" of the vector will be the current state.
};

#endif
typedef StateManager STMA; //This is similar to setting a macro.