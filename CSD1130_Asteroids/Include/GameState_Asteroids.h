/* Start Header **************************************************************/
/*!
\file	GameState_Asteroids.h
\author Jee Jia Min , j.jiamin, and 2002144
\par	j.jiamin@digipen.edu
\date	Feb 11, 2021
\brief
This header declares functions of collision between active game objects.

The functions include:

\li \c void GameStateAsteroidsLoad(void);
	\n Create the asteroid game object (shape)
	\n Create the bullet game object (shape)
	\n Create normalized shapes, which means all the vertices' coordinates in the [-0.5;0.5] range. Use the object instances' scale values to resize the shape. 

\li \c void GameStateAsteroidsInit(void);
	\n Create 4 initial asteroids instances with defined positions and velocities.
	\n Every asteroid have a different size, position, and velocity.

\li \c void GameStateAsteroidsUpdate(void);
	\n Update the ship's acceleration/velocity/orientation according to user input.
	\n Uses : AEInputCheckCurr: Checks pressed keys.
	\n		: AEInputCheckTriggered: Checks triggered keys.

\li \c void GameStateAsteroidsDraw(void);
	\n Draw the object instance using the "AEGfxMeshDraw" function.

\li \c void GameStateAsteroidsFree(void);
	\n Kill each active game object instance using the "gameObjInstDestroy" function.

\li \c void GameStateAsteroidsUnload(void);
	\n Free each used game object (shape) using the "AEGfxMeshFree" function.

Copyright (C) 20xx DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header ****************************************************************/


#ifndef CS230_GAME_STATE_PLAY_H_
#define CS230_GAME_STATE_PLAY_H_

// ---------------------------------------------------------------------------

void GameStateAsteroidsLoad(void);
void GameStateAsteroidsInit(void);
void GameStateAsteroidsUpdate(void);
void GameStateAsteroidsDraw(void);
void GameStateAsteroidsFree(void);
void GameStateAsteroidsUnload(void);

// ---------------------------------------------------------------------------

#endif // CS230_GAME_STATE_PLAY_H_


