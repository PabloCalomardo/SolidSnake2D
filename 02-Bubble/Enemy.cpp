#include "Enemy.h"

#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Enemy.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define PIXEL_X 1/255.0f
#define PIXEL_Y 1/211.0f
#define FALL_STEP 4


enum EnemyAnims
{ 
	STAND_NORMAL, STAND_UP, STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, SITTED
};

/* DADES:
* DOG COMENÇA AL PIXEL 2,9
* SOLDIER COMENÇA AL PIXEL 2,56
* SOLDIER2 COMENÇA AL PIXEL 2,142

*/



void Enemy::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int TipusEnemic)
{
	enemic_detectat = false;
	EnemyType = TipusEnemic;
	mort = false;
	spritesheet.loadFromFile("images/Enemies.png", TEXTURE_PIXEL_FORMAT_RGBA);	//Enemies ES: 255x211 
	sprite = Sprite::createSprite(glm::ivec2(16 * 2, 32 * 2), glm::vec2(PIXEL_X * 16, PIXEL_Y * 32), &spritesheet, &shaderProgram);

	switch (EnemyType) {
		case 0: sprite->setNumberAnimations(10);; break; //DOG
		default: sprite->setNumberAnimations(8); break; //SOLDIER
	}
	

	//==============================
	//			GOS
	//==============================
	if (EnemyType == 0)
	{

		sprite->setAnimationSpeed(SITTED, 1);
		sprite->addKeyframe(SITTED, glm::vec2(PIXEL_X * 2, PIXEL_Y * 10)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix
		sprite->addKeyframe(SITTED, glm::vec2(PIXEL_X * 21, PIXEL_Y * 10)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix


		//==============================
		//			STANDS 
		//==============================
		sprite->setAnimationSpeed(STAND_NORMAL, 8);
		sprite->addKeyframe(STAND_NORMAL, glm::vec2(PIXEL_X * (36 + 11), PIXEL_Y * 9)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix

		sprite->setAnimationSpeed(STAND_UP, 8);
		sprite->addKeyframe(STAND_UP, glm::vec2(PIXEL_X * 66, PIXEL_Y * 26)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix

		sprite->setAnimationSpeed(STAND_LEFT, 8);	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(STAND_LEFT, glm::vec2(PIXEL_X * 85, PIXEL_Y * 61));

		sprite->setAnimationSpeed(STAND_RIGHT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(PIXEL_X * 104, PIXEL_Y * 61));

		//==============================
		//			MOVES
		//==============================

		sprite->setAnimationSpeed(MOVE_LEFT, 8); // Inicialment STAND LEFT, despres les animacions de caminar
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 61)); //Stand left
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * (36 + 11), PIXEL_Y * 61));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * (36 + 11 + 38), PIXEL_Y * 61));

		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * 21, PIXEL_Y * 61)); //Stand right
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 1), PIXEL_Y * 61));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1), PIXEL_Y * 61));

		sprite->setAnimationSpeed(MOVE_UP, 8);
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * 21, PIXEL_Y * 26)); //Stand up
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * (3 + 54 + 8 + 1), PIXEL_Y * 26));
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1), PIXEL_Y * 26));

		sprite->setAnimationSpeed(MOVE_DOWN, 8);
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * 2, PIXEL_Y * 26)); //Stand normal
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (36 + 10 + 1), PIXEL_Y * 26));
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (36 + 10 + 38 + 1), PIXEL_Y * 26));
	}
	else if (EnemyType == 1) //SOLDIER
	{
		//==============================
		//			STANDS 
		//==============================
		sprite->setAnimationSpeed(STAND_NORMAL, 8);
		sprite->addKeyframe(STAND_NORMAL, glm::vec2(PIXEL_X * (2 + 136), PIXEL_Y * 26)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix
		sprite->setAnimationSpeed(STAND_UP, 8);
		sprite->addKeyframe(STAND_UP, glm::vec2(PIXEL_X * (21 + 136), PIXEL_Y * 26)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix
		sprite->setAnimationSpeed(STAND_LEFT, 8);	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(STAND_LEFT, glm::vec2(PIXEL_X * (2 + 136), PIXEL_Y * 61));
		sprite->setAnimationSpeed(STAND_RIGHT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(PIXEL_X * (21 + 136), PIXEL_Y * 61));
		//==============================
		//			MOVES
		//==============================
		sprite->setAnimationSpeed(MOVE_LEFT, 8); // Inicialment STAND LEFT, despres les animacions de caminar
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 61)); //Stand left
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * (36 + 11), PIXEL_Y * 61));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * (36 + 11 + 38), PIXEL_Y * 61));

		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * 21, PIXEL_Y * 61)); //Stand right
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 1), PIXEL_Y * 61));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1), PIXEL_Y * 61));

		sprite->setAnimationSpeed(MOVE_UP, 8);
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * 21, PIXEL_Y * 26)); //Stand up
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * (3 + 54 + 8 + 1), PIXEL_Y * 26));
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1), PIXEL_Y * 26));

		sprite->setAnimationSpeed(MOVE_DOWN, 8);
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * 2, PIXEL_Y * 26)); //Stand normal
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (36 + 10 + 1), PIXEL_Y * 26));
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (36 + 10 + 38 + 1), PIXEL_Y * 26));

	}
	else {
		//==============================
		//			STANDS 
		//==============================
		sprite->setAnimationSpeed(STAND_NORMAL, 8);
		sprite->addKeyframe(STAND_NORMAL, glm::vec2(PIXEL_X * (2 + 136), PIXEL_Y * 26)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix
		sprite->setAnimationSpeed(STAND_UP, 8);
		sprite->addKeyframe(STAND_UP, glm::vec2(PIXEL_X * (21 + 136), PIXEL_Y * 26)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix
		sprite->setAnimationSpeed(STAND_LEFT, 8);	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(STAND_LEFT, glm::vec2(PIXEL_X * (2 + 136), PIXEL_Y * 61));
		sprite->setAnimationSpeed(STAND_RIGHT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(PIXEL_X * (21 + 136), PIXEL_Y * 61));
		//==============================
		//			MOVES
		//==============================
		sprite->setAnimationSpeed(MOVE_LEFT, 8); // Inicialment STAND LEFT, despres les animacions de caminar
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 61)); //Stand left
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * (36 + 11), PIXEL_Y * 61));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * (36 + 11 + 38), PIXEL_Y * 61));

		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * 21, PIXEL_Y * 61)); //Stand right
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 1), PIXEL_Y * 61));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1), PIXEL_Y * 61));

		sprite->setAnimationSpeed(MOVE_UP, 8);
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * 21, PIXEL_Y * 26)); //Stand up
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * (3 + 54 + 8 + 1), PIXEL_Y * 26));
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1), PIXEL_Y * 26));

		sprite->setAnimationSpeed(MOVE_DOWN, 8);
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * 2, PIXEL_Y * 26)); //Stand normal
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (36 + 10 + 1), PIXEL_Y * 26));
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (36 + 10 + 38 + 1), PIXEL_Y * 26));
	}

	if(EnemyType == 0) sprite->changeAnimation(SITTED);
	else sprite->changeAnimation(STAND_NORMAL);

	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));

}

void Enemy::update(int deltaTime)
{
	sprite->update(deltaTime);

	if (EnemyType == 0) //GOS
	{
		if (enemic_detectat) {

		}
		else { //Tranquilitat
			if (sprite->animation() != SITTED) sprite->changeAnimation(SITTED);
		}
	}
	else { //SOLDIER
		if (enemic_detectat) {

		}
		else {

		}
	}
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

void Enemy::render()
{
	sprite->render();
}

void Enemy::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Enemy::setPosition(const glm::vec2& pos)
{
	posEnemy = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}




