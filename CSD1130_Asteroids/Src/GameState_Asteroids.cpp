/* Start Header **************************************************************/
/*!
\file	GameState_Asteroids.cpp
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

#include "main.h"
#include "GameStateMgr.h"
#include <iostream>

/******************************************************************************/
/*!
	Defines
*/
/******************************************************************************/
const unsigned int	GAME_OBJ_NUM_MAX		= 32;			//The total number of different objects (Shapes)
const unsigned int	GAME_OBJ_INST_NUM_MAX	= 2048;			//The total number of different game object instances


const unsigned int	SHIP_INITIAL_NUM		= 3;			// initial number of ship lives
const float			SHIP_SIZE				= 60.0f;		// ship size
const float			SHIP_ACCEL_FORWARD		= 1.5f;			// ship forward acceleration (in m/s^2)
const float			SHIP_ACCEL_BACKWARD		= 3.0f;			// ship backward acceleration (in m/s^2)
const float			SHIP_ROT_SPEED			= (2.0f * PI);	// ship rotation speed (degree/second)

const float			BULLET_SPEED			= 10.0f;			// bullet speed (m/s)
const float			BULLET_SIZE				= 15.0f;			

const float			ASTEROID_SIZE			= 100.0f;
float				ASTEROID_FRAME			= 1.0f;
float				ASTEROID_FRAME_MAX		= 1.0f;
int					ASTEROID_COUNTER		= 10;

bool				PRINT					= false;
bool				GAMEOVER				= true;
bool				RESTART					= false;

float				GAMEOVERTIMER			= 0.0f;
bool				SPAWN1					= false;
bool				SPAWN2					= false;
bool				SPAWN3					= false;
bool				SPAWN4					= false;

char fontId = 0;
f32 TextWidth, TextHeight;




// -----------------------------------------------------------------------------
enum TYPE
{
	// list of game object types
	TYPE_SHIP = 0, 
	TYPE_BULLET,
	TYPE_ASTEROID,
	TYPE_GAMEOVER1,
	TYPE_GAMEOVER2,
	TYPE_GAMEOVER3,
	TYPE_GAMEOVER4,

	TYPE_NUM
};

// -----------------------------------------------------------------------------
// object flag definition

const unsigned long FLAG_ACTIVE				= 0x00000001;

/******************************************************************************/
/*!
	Struct/Class Definitions
*/
/******************************************************************************/

//Game object structure
struct GameObj
{
	unsigned long		type;		// object type
	AEGfxVertexList *	pMesh;		// This will hold the triangles which will form the shape of the object
	AEGfxTexture*		pTex;
};


GameObj* pObj_Asteroid;
GameObj* pObj_Bullet;
GameObj* pObj_Ship;


// ---------------------------------------------------------------------------

//Game object instance structure
struct GameObjInst
{
	GameObj *			pObject;	// pointer to the 'original' shape
	unsigned long		flag;		// bit flag or-ed together
	float				scale;		// scaling value of the object instance
	AEVec2				posCurr;	// object current position
	AEVec2				velCurr;	// object current velocity
	float				dirCurr;	// object current direction
	AABB				boundingBox;// object bouding box that encapsulates the object
	AEMtx33				transform;	// object transformation matrix: Each frame, 
									// calculate the object instance's transformation matrix and save it here

	//void				(*pfUpdate)(void);
	//void				(*pfDraw)(void);
};

/******************************************************************************/
/*!
	Static Variables
*/
/******************************************************************************/

// list of original object
static GameObj				sGameObjList[GAME_OBJ_NUM_MAX];				// Each element in this array represents a unique game object (shape)
static unsigned long		sGameObjNum;								// The number of defined game objects

// list of object instances
static GameObjInst			sGameObjInstList[GAME_OBJ_INST_NUM_MAX];	// Each element in this array represents a unique game object instance (sprite)
static unsigned long		sGameObjInstNum;							// The number of used game object instances

// pointer to the ship object
static GameObjInst *			spShip;									// Pointer to the "Ship" game object instance
// number of ship available (lives 0 = game over)
static long					sShipLives;									// The number of lives left

// the score = number of asteroid destroyed
static unsigned long		sScore;										// Current score



// ---------------------------------------------------------------------------

// functions to create/destroy a game object instance
GameObjInst *		gameObjInstCreate (unsigned long type, float scale, 
											   AEVec2 * pPos, AEVec2 * pVel, float dir);
void				gameObjInstDestroy(GameObjInst * pInst);


/******************************************************************************/
/*!
	"Load" function of this state
*/
/******************************************************************************/
void GameStateAsteroidsLoad(void)
{
	// zero the game object array
	memset(sGameObjList, 0, sizeof(GameObj) * GAME_OBJ_NUM_MAX);
	// No game objects (shapes) at this point
	sGameObjNum = 0;

	// zero the game object instance array
	memset(sGameObjInstList, 0, sizeof(GameObjInst) * GAME_OBJ_INST_NUM_MAX);
	// No game object instances (sprites) at this point
	sGameObjInstNum = 0;

	// The ship object instance hasn't been created yet, so this "spShip" pointer is initialized to 0
	spShip = nullptr;

	// load/create the mesh data (game objects / Shapes)

	// =====================
	// create the ship shape
	// =====================

	pObj_Ship		= sGameObjList + sGameObjNum++;

	pObj_Ship->type	= TYPE_SHIP;
	
	pObj_Ship->pTex	= AEGfxTextureLoad("../Resources/Textures/Ship.png");

	AE_ASSERT_MESG(pObj_Ship->pTex, "Failed to create texture1!!");

	AEGfxMeshStart();

	AEGfxTriAdd(
		-0.5f, -0.5f, 0x00FF00FF, 0.0f, 1.0f,
		0.5f, -0.5f, 0x00FFFF00, 1.0f, 1.0f,
		-0.5f, 0.5f, 0x0000FFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0x00FFFFFF, 1.0f, 1.0f,
		0.5f, 0.5f, 0x00FFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0x00FFFFFF, 0.0f, 0.0f);

	pObj_Ship->pMesh = AEGfxMeshEnd();

	AE_ASSERT_MESG(pObj_Ship->pMesh, "fail to create object!!");


	// =======================
	// create the bullet shape
	// =======================


	// load/create the mesh data (game objects / Shapes)

	pObj_Bullet = sGameObjList + sGameObjNum++;

	pObj_Bullet->type = TYPE_BULLET;

	pObj_Bullet->pTex	= AEGfxTextureLoad("../Resources/Textures/Bullet.png");

	AE_ASSERT_MESG(pObj_Bullet->pTex, "Failed to create texture1!!");

	AEGfxMeshStart();

	AEGfxTriAdd(
		-0.5f, -0.5f, 0x00FF00FF, 0.0f, 1.0f,
		0.5f, -0.5f, 0x00FFFF00, 1.0f, 1.0f,
		-0.5f, 0.5f, 0x0000FFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0x00FFFFFF, 1.0f, 1.0f,
		0.5f, 0.5f, 0x00FFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0x00FFFFFF, 0.0f, 0.0f);

	pObj_Bullet->pMesh = AEGfxMeshEnd();

	AE_ASSERT_MESG(pObj_Bullet->pMesh, "fail to create object!!");


	// =========================
	// create the asteroid shape
	// =========================
	


	pObj_Asteroid = sGameObjList + sGameObjNum++;

	pObj_Asteroid->type = TYPE_ASTEROID ;

	pObj_Asteroid->pTex = AEGfxTextureLoad("../Resources/Textures/Asteroid.png");

	AE_ASSERT_MESG(pObj_Asteroid->pTex, "Failed to create texture1!!");

	AEGfxMeshStart();

	AEGfxTriAdd(
		-0.5f, -0.5f, 0x00FF00FF, 0.0f, 1.0f,
		0.5f, -0.5f, 0x00FFFF00, 1.0f, 1.0f,
		-0.5f, 0.5f, 0x0000FFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0x00FFFFFF, 1.0f, 1.0f,
		0.5f, 0.5f, 0x00FFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0x00FFFFFF, 0.0f, 0.0f);

	pObj_Asteroid->pMesh = AEGfxMeshEnd();

	AE_ASSERT_MESG(pObj_Asteroid->pMesh, "fail to create object!!");

	// =========================
	// create the GAMEOVER shape
	// =========================
	// GAMEOVER #1
	pObj_Asteroid = sGameObjList + sGameObjNum++;

	pObj_Asteroid->type = TYPE_GAMEOVER1;

	pObj_Asteroid->pTex = AEGfxTextureLoad("../Resources/Textures/Gameover1.png");

	AE_ASSERT_MESG(pObj_Asteroid->pTex, "Failed to create texture1!!");

	AEGfxMeshStart();

	AEGfxTriAdd(
		-0.5f, -0.5f, 0x00FF00FF, 0.0f, 1.0f,
		0.5f, -0.5f, 0x00FFFF00, 1.0f, 1.0f,
		-0.5f, 0.5f, 0x0000FFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0x00FFFFFF, 1.0f, 1.0f,
		0.5f, 0.5f, 0x00FFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0x00FFFFFF, 0.0f, 0.0f);

	pObj_Asteroid->pMesh = AEGfxMeshEnd();

	AE_ASSERT_MESG(pObj_Asteroid->pMesh, "fail to create object!!");
	// GAMEOVER #2

	pObj_Asteroid = sGameObjList + sGameObjNum++;

	pObj_Asteroid->type = TYPE_GAMEOVER2;

	pObj_Asteroid->pTex = AEGfxTextureLoad("../Resources/Textures/Gameover2.png");

	AE_ASSERT_MESG(pObj_Asteroid->pTex, "Failed to create texture1!!");

	AEGfxMeshStart();

	AEGfxTriAdd(
		-0.5f, -0.5f, 0x00FF00FF, 0.0f, 1.0f,
		0.5f, -0.5f, 0x00FFFF00, 1.0f, 1.0f,
		-0.5f, 0.5f, 0x0000FFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0x00FFFFFF, 1.0f, 1.0f,
		0.5f, 0.5f, 0x00FFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0x00FFFFFF, 0.0f, 0.0f);

	pObj_Asteroid->pMesh = AEGfxMeshEnd();

	AE_ASSERT_MESG(pObj_Asteroid->pMesh, "fail to create object!!");
	// GAMEOVER #3

	pObj_Asteroid = sGameObjList + sGameObjNum++;

	pObj_Asteroid->type = TYPE_GAMEOVER3;

	pObj_Asteroid->pTex = AEGfxTextureLoad("../Resources/Textures/Gameover3.png");

	AE_ASSERT_MESG(pObj_Asteroid->pTex, "Failed to create texture1!!");

	AEGfxMeshStart();

	AEGfxTriAdd(
		-0.5f, -0.5f, 0x00FF00FF, 0.0f, 1.0f,
		0.5f, -0.5f, 0x00FFFF00, 1.0f, 1.0f,
		-0.5f, 0.5f, 0x0000FFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0x00FFFFFF, 1.0f, 1.0f,
		0.5f, 0.5f, 0x00FFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0x00FFFFFF, 0.0f, 0.0f);

	pObj_Asteroid->pMesh = AEGfxMeshEnd();

	AE_ASSERT_MESG(pObj_Asteroid->pMesh, "fail to create object!!");
	// GAMEOVER #4

	pObj_Asteroid = sGameObjList + sGameObjNum++;

	pObj_Asteroid->type = TYPE_GAMEOVER4;

	pObj_Asteroid->pTex = AEGfxTextureLoad("../Resources/Textures/Gameover4.png");

	AE_ASSERT_MESG(pObj_Asteroid->pTex, "Failed to create texture1!!");

	AEGfxMeshStart();

	AEGfxTriAdd(
		-0.5f, -0.5f, 0x00FF00FF, 0.0f, 1.0f,
		0.5f, -0.5f, 0x00FFFF00, 1.0f, 1.0f,
		-0.5f, 0.5f, 0x0000FFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0x00FFFFFF, 1.0f, 1.0f,
		0.5f, 0.5f, 0x00FFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0x00FFFFFF, 0.0f, 0.0f);

	pObj_Asteroid->pMesh = AEGfxMeshEnd();

	AE_ASSERT_MESG(pObj_Asteroid->pMesh, "fail to create object!!");




	fontId = AEGfxCreateFont("../Resources/Fonts/Strawberry_Muffins_Demo.ttf", 20);

}

/******************************************************************************/
/*!
	"Initialize" function of this state
*/
/******************************************************************************/
void GameStateAsteroidsInit(void)
{
	// create the main ship
	spShip		= gameObjInstCreate(TYPE_SHIP, SHIP_SIZE, nullptr, nullptr, 0.0f);
	AE_ASSERT(spShip);	

	// CREATE THE INITIAL ASTEROIDS INSTANCES USING THE "gameObjInstCreate" FUNCTION

	// reset the score and the number of ships
	sScore      = 0;
	sShipLives  = SHIP_INITIAL_NUM;

	GAMEOVER = true;
	RESTART = false;
	ASTEROID_COUNTER = 10;
	SPAWN1 = false;
	SPAWN2 = false;
	SPAWN3 = false;
	SPAWN4 = false;
	GAMEOVERTIMER = 0.0f;
}

/******************************************************************************/
/*!
	"Update" function of this state
*/
/******************************************************************************/
void GameStateAsteroidsUpdate(void)
{

	// =========================
	// update according to input
	// =========================

	// This input handling moves the ship without any velocity nor acceleration
	// It should be changed when implementing the Asteroids project
	//
	// Updating the velocity and position according to acceleration is 
	// done by using the following:
	// Pos1 = 1/2 * a*t*t + v0*t + Pos0
	//
	// In our case we need to divide the previous equation into two parts in order 
	// to have control over the velocity and that is done by:
	//
	// v1 = a*t + v0		//This is done when the UP or DOWN key is pressed 
	// Pos1 = v1*t + Pos0

if(GAMEOVER)
{ 
	if (AEInputCheckCurr(AEVK_UP))
	{
		AEVec2 added;
		AEVec2Set(&added, cosf(spShip->dirCurr), sinf(spShip->dirCurr));  // current orientation α to compute the normalized acceleration vector
		AEVec2Scale(&added, &added, SHIP_ACCEL_FORWARD); // Scaling by a predefined value will give the full acceleration vector
		AEVec2Scale(&added, &added, (float)(AEFrameRateControllerGetFrameTime())); //a*dt
		AEVec2Add(&spShip->velCurr, &added, &spShip->velCurr); //newVel = a*dt + currVel
	}

	if (AEInputCheckCurr(AEVK_DOWN))
	{
		AEVec2 added;
		AEVec2Set(&added, -cosf(spShip->dirCurr), -sinf(spShip->dirCurr));  // current orientation α to compute the normalized acceleration vector
		AEVec2Scale(&added, &added, SHIP_ACCEL_BACKWARD); // Scaling by a predefined value will give the full acceleration vector
		AEVec2Scale(&added, &added, (float)(AEFrameRateControllerGetFrameTime())); //a*dt
		AEVec2Add(&spShip->velCurr, &added, &spShip->velCurr); //newVel = a*dt + currVel
	}

	if (AEInputCheckCurr(AEVK_LEFT))
	{
		spShip->dirCurr += SHIP_ROT_SPEED * (float)(AEFrameRateControllerGetFrameTime ());
		spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
	}

	if (AEInputCheckCurr(AEVK_RIGHT))
	{
		spShip->dirCurr -= SHIP_ROT_SPEED * (float)(AEFrameRateControllerGetFrameTime ());
		spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
	}
			
	
	// Shoot a bullet if space is triggered (Create a new object instance)
	if (AEInputCheckTriggered(AEVK_SPACE))
	{
		GameObjInst* spBullet;
		spBullet = gameObjInstCreate(TYPE_BULLET, BULLET_SIZE, nullptr, nullptr, 0.0f);
		AE_ASSERT(spBullet);
		spBullet->dirCurr = spShip->dirCurr;
		spBullet->posCurr = spShip->posCurr;
		AEVec2 added;
		AEVec2Set(&added, cosf(spShip->dirCurr), sinf(spShip->dirCurr));
		AEVec2Scale(&spBullet->velCurr,&added, BULLET_SPEED);
		// Get the bullet's direction according to the ship's direction
		// Set the velocity
		// Create an instance
	}

	if ((ASTEROID_FRAME -= (float)AEFrameRateControllerGetFrameTime())<0)
	{
		ASTEROID_FRAME = ASTEROID_FRAME_MAX;

		if (ASTEROID_COUNTER-- >= 0)
		{

			float random = AERandFloat();
			AEVec2 pos;
			if (AERandFloat() < 0.5f) {
				// horizontal
				if (AERandFloat() < 0.5f) {
					// negative
					AEVec2Set(&pos, AEGfxGetWinMinX() - ASTEROID_SIZE, AERandFloat() * AEGetWindowHeight());
				}
				else { // positive
					AEVec2Set(&pos, AEGfxGetWinMaxX() + ASTEROID_SIZE, AERandFloat() * AEGetWindowHeight());
				}
			}
			else {
				// vertical
				if (AERandFloat() < 0.5f) {
					AEVec2Set(&pos, AERandFloat() * AEGetWindowWidth(), AEGfxGetWinMinY() - ASTEROID_SIZE);
				}
				else {
					AEVec2Set(&pos, AERandFloat() * AEGetWindowWidth(), AEGfxGetWinMaxY() + ASTEROID_SIZE);
				}
			}
			GameObjInst* spAsteroid;
			spAsteroid = gameObjInstCreate(TYPE_ASTEROID, ASTEROID_SIZE, &pos, nullptr, 0.0f);
			AE_ASSERT(spAsteroid);
			AEVec2 added;
			AEVec2Set(&added, -cosf(random * 360), -sinf(random * 360));
			AEVec2Scale(&spAsteroid->velCurr, &added, 1.0f);
			AEVec2Add(&spAsteroid->posCurr , &spAsteroid->velCurr, &spAsteroid->posCurr);
		}
	}
}

	// ======================================================
	// update physics of all active game object instances
	//	-- Positions are updated here with the computed velocity
	//  -- Get the bounding rectangle of every active instance:
	//		boundingRect_min = -BOUNDING_RECT_SIZE * instance->scale + instance->pos
	//		boundingRect_max = BOUNDING_RECT_SIZE * instance->scale + instance->pos
	// ======================================================

	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		pInst->boundingBox.min.x = -0.5f * pInst->scale + pInst->posCurr.x;
		pInst->boundingBox.min.y = -0.5f * pInst->scale + pInst->posCurr.y;
		pInst->boundingBox.max.x = 0.5f * pInst->scale + pInst->posCurr.x;
		pInst->boundingBox.max.y = 0.5f * pInst->scale + pInst->posCurr.y;


		if (pInst->pObject->type == TYPE_BULLET)
		{
			for (unsigned long j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pInstj = sGameObjInstList + j;

				// skip non-active object
				if ((pInstj->flag & FLAG_ACTIVE) == 0)
					continue;

				if (pInstj->pObject->type == TYPE_ASTEROID)
				{
					if (CollisionIntersection_RectRect(pInstj->boundingBox, pInstj->velCurr, pInst->boundingBox, pInst->velCurr))
					{
						gameObjInstDestroy(pInstj);
						gameObjInstDestroy(pInst);
						sScore++;
						if (sScore >= 5000)
						{
							std::cout << "You Rock!" << std::endl;
						}
						ASTEROID_COUNTER ++;
					}
				}
			}
		}

		if (pInst->pObject->type == TYPE_ASTEROID)
		{
			if (CollisionIntersection_RectRect(spShip->boundingBox, spShip->velCurr, pInst->boundingBox, pInst->velCurr))
			{
				gameObjInstDestroy(pInst);
				sShipLives--;
				AEVec2Set(&spShip->posCurr,0.0f,0.0f);
				AEVec2Set(&spShip->velCurr, 0.0f, 0.0f);
				PRINT = true ;
				ASTEROID_COUNTER++;
			}
		}
	}




	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;


		// check if the object is a ship
		if (pInst->pObject->type == TYPE_BULLET) 
		{
			AEVec2Add(&pInst->posCurr,&pInst->posCurr,&pInst->velCurr);
		}

		if (pInst->pObject->type == TYPE_ASTEROID)
		{
			AEVec2Add(&pInst->posCurr, &pInst->posCurr, &pInst->velCurr);
		}
	}

	
	spShip->velCurr.x *= 0.99f;
	spShip->velCurr.y *= 0.99f;
	
	AEVec2Add(&spShip->posCurr, &spShip->velCurr, &spShip->posCurr);
	

	// ===================================
	// update active game object instances
	// Example:
	//		-- Wrap specific object instances around the world (Needed for the assignment)
	//		-- Removing the bullets as they go out of bounds (Needed for the assignment)
	//		-- If you have a homing missile for example, compute its new orientation 
	//			(Homing missiles are not required for the Asteroids project)
	//		-- Update a particle effect (Not required for the Asteroids project)
	// ===================================
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		// check if the object is a ship
		if (pInst->pObject->type == TYPE_SHIP)
		{
			// warp the ship from one end of the screen to the other
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, AEGfxGetWinMinX() - SHIP_SIZE, 
														AEGfxGetWinMaxX() + SHIP_SIZE);
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, AEGfxGetWinMinY() - SHIP_SIZE, 
														AEGfxGetWinMaxY() + SHIP_SIZE);
		}

		// Wrap asteroids here
		if (pInst->pObject->type == TYPE_ASTEROID)
		{
			// warp the ship from one end of the screen to the other
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, AEGfxGetWinMinX() - ASTEROID_SIZE,
				AEGfxGetWinMaxX() + ASTEROID_SIZE);
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, AEGfxGetWinMinY() - ASTEROID_SIZE,
				AEGfxGetWinMaxY() + ASTEROID_SIZE);
		}
			

		// Remove bullets that go out of bounds
		if (pInst->pObject->type == TYPE_BULLET)
		{
			if (pInst->posCurr.x + BULLET_SIZE <= AEGfxGetWinMinX() || pInst->posCurr.x - BULLET_SIZE >= AEGfxGetWinMaxX() ||
				pInst->posCurr.y + BULLET_SIZE <= AEGfxGetWinMinY() || pInst->posCurr.y - BULLET_SIZE >= AEGfxGetWinMaxY())
			{
				gameObjInstDestroy(pInst);
			}
		}

	}



	// ====================
	// check for collision
	// ====================
	
	/*
	for each object instance: oi1
		if oi1 is not active
			skip

		if oi1 is an asteroid
			for each object instance oi2
				if(oi2 is not active or oi2 is an asteroid)
					skip

				if(oi2 is the ship)
					Check for collision between ship and asteroids (Rectangle - Rectangle)
					Update game behavior accordingly
					Update "Object instances array"
				else
				if(oi2 is a bullet)
					Check for collision between ship and asteroids (Rectangle - Rectangle)
					Update game behavior accordingly
					Update "Object instances array"
	*/



	// =====================================
	// calculate the matrix for all objects
	// =====================================

	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * pInst = sGameObjInstList + i;
		AEMtx33		 trans, rot, scale;

		/*UNREFERENCED_PARAMETER(trans);
		UNREFERENCED_PARAMETER(rot);
		UNREFERENCED_PARAMETER(scale);*/

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		// Compute the scaling matrix
		AEMtx33Scale(&scale , pInst->scale , pInst->scale);

		// Compute the rotation matrix 
		AEMtx33Rot(&rot, pInst->dirCurr);

		// Compute the translation matrix
		AEMtx33Trans(&trans, pInst->posCurr.x, pInst->posCurr.y);

		// Concatenate the 3 matrix in the correct order in the object instance's "transform" matrix
		AEMtx33Concat(&pInst->transform, &rot ,&scale);

		AEMtx33Concat(&pInst->transform, &trans , &pInst->transform);
	}

}

/******************************************************************************/
/*!
	
*/
/******************************************************************************/
void GameStateAsteroidsDraw(void)
{
	char strBuffer[1024];
	
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);
	AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);

	// draw all object instances in the list
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		// Set the current object instance's transform matrix using "AEGfxSetTransform"
		AEGfxTextureSet(pInst->pObject->pTex, 0,0);

		// Set Transformation
		AEGfxSetTransform(pInst->transform.m);

		// Draw the shape used by the current object instance using "AEGfxMeshDraw"
		AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
	}



	sprintf_s(strBuffer, "Ship Left: %d", sShipLives >= 0 ? sShipLives : 0);
	AEGfxGetPrintSize(fontId, strBuffer, 1.0f, TextWidth, TextHeight);
	AEGfxPrint(fontId, strBuffer, 0.99f - TextWidth, 0.99f - TextHeight, 1.0f, 1.f, 1.f, 1.f);
	
	sprintf_s(strBuffer, "Score: %d", sScore);
	AEGfxGetPrintSize(fontId, strBuffer, 1.0f, TextWidth, TextHeight);
	AEGfxPrint(fontId, strBuffer, 0.66f - TextWidth, 0.99f - TextHeight, 1.0f, 1.f, 1.f, 1.f);




	//You can replace this condition/variable by your own data.
	//The idea is to display any of these variables/strings whenever a change in their value happens
	static bool onValueChange = true;
	if(PRINT)
	{
		// display the game over message
		if (sShipLives <= 0)
		{
			GAMEOVER = false;
			RESTART = true;
		}

		PRINT = false;
	}
	if (RESTART) {
		if (AEInputCheckTriggered(AEVK_RETURN))
		{
			gGameStateNext = GS_RESTART;
		}
		GAMEOVERTIMER += (float)AEFrameRateControllerGetFrameTime();
		AEVec2 zero;
		AEVec2Set(&zero, 0.0f,0.0f);
		if (GAMEOVERTIMER > 3.0f && !SPAWN4) {
			gameObjInstCreate(TYPE_GAMEOVER4, (float)AEGetWindowWidth(), &zero, &zero, 0.0f);
			SPAWN4 = true;
		}
		if (GAMEOVERTIMER > 2.0f && !SPAWN3) {
			gameObjInstCreate(TYPE_GAMEOVER3, (float)AEGetWindowWidth(), &zero, &zero, 0.0f);
			SPAWN3 = true;
		}
		if (GAMEOVERTIMER > 1.0f && !SPAWN2) {
			gameObjInstCreate(TYPE_GAMEOVER2, (float)AEGetWindowWidth(), &zero, &zero, 0.0f);
			SPAWN2 = true;
		}
		if (GAMEOVERTIMER > 0.5f && !SPAWN1) {
			gameObjInstCreate(TYPE_GAMEOVER1, (float)AEGetWindowWidth(), &zero, &zero, 0.0f);
			SPAWN1 = true;
		}
	}
}

/******************************************************************************/
/*!
	
*/
/******************************************************************************/
void GameStateAsteroidsFree(void)
{
	// kill all object instances in the array using "gameObjInstDestroy"

	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		gameObjInstDestroy(pInst);
	}
}

/******************************************************************************/
/*!
	
*/
/******************************************************************************/
void GameStateAsteroidsUnload(void)
{
	// free all mesh data (shapes) of each object using "AEGfxTriFree"
	//AEGfxMeshFree(_mesh);
	//AEGfxTextureUnload(_texture);



	for (unsigned long i = 0; i < sGameObjNum; i++)
	{
		GameObj* pObj = sGameObjList + i;
		AEGfxMeshFree(pObj->pMesh);
		AEGfxTextureUnload(pObj->pTex);
	}

	/*AEGfxTextureUnload(pObj_Asteroid->pTex);
	AEGfxTextureUnload(pObj_Ship->pTex);
	AEGfxTextureUnload(pObj_Bullet->pTex);*/
	

	AEGfxDestroyFont(fontId);
	
}

/******************************************************************************/
/*!
	
*/
/******************************************************************************/
GameObjInst * gameObjInstCreate(unsigned long type, 
							   float scale, 
							   AEVec2 * pPos, 
							   AEVec2 * pVel, 
							   float dir)
{
	AEVec2 zero;
	AEVec2Zero(&zero);

	AE_ASSERT_PARM(type < sGameObjNum);
	
	// loop through the object instance list to find a non-used object instance
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * pInst = sGameObjInstList + i;

		// check if current instance is not used
		if (pInst->flag == 0)
		{
			// it is not used => use it to create the new instance
			pInst->pObject	= sGameObjList + type;
			pInst->flag		= FLAG_ACTIVE;
			pInst->scale	= scale;
			pInst->posCurr	= pPos ? *pPos : zero;
			pInst->velCurr	= pVel ? *pVel : zero;
			pInst->dirCurr	= dir;
			
			// return the newly created instance
			return pInst;
		}
	}

	// cannot find empty slot => return 0
	return 0;
}

/******************************************************************************/
/*!
	
*/
/******************************************************************************/
void gameObjInstDestroy(GameObjInst * pInst)
{
	// if instance is destroyed before, just return
	if (pInst->flag == 0)
		return;

	// zero out the flag
	pInst->flag = 0;
}