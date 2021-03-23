/* Start Header **************************************************************/
/*!
\file	GameStateList.h
\author Jee Jia Min , j.jiamin, and 390002420
\par	j.jiamin@digipen.edu
\date	Feb 11, 2021
\brief
List of all game states.
*/
/* End Header ****************************************************************/


#ifndef CS230_GAME_STATE_LIST_H_
#define CS230_GAME_STATE_LIST_H_

// ---------------------------------------------------------------------------
// game state list

enum
{
	// list of all game states 
	GS_ASTEROIDS = 0, 

	// special game state. Do not change
	GS_RESTART,
	GS_QUIT, 
	GS_NONE
};

// ---------------------------------------------------------------------------

#endif // CS230_GAME_STATE_LIST_H_