#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define PIXEL_X 1/368.0f
#define PIXEL_Y 1/189.0f
#define FALL_STEP 4


enum PlayerAnims
{
	STAND_NORMAL, STAND_UP, STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, PUNCH_LEFT, PUNCH_RIGHT, PUNCH_UP, PUNCH_DOWN,
	ARMED_STAND_NORMAL, ARMED_STAND_UP, ARMED_STAND_LEFT, ARMED_STAND_RIGHT, ARMED_MOVE_LEFT, ARMED_MOVE_RIGHT, ARMED_MOVE_UP, ARMED_MOVE_DOWN,
	STAND_NORMAL_FERIT, STAND_UP_FERIT, STAND_LEFT_FERIT, STAND_RIGHT_FERIT, MOVE_LEFT_FERIT, MOVE_RIGHT_FERIT, MOVE_UP_FERIT, MOVE_DOWN_FERIT, PUNCH_LEFT_FERIT, PUNCH_RIGHT_FERIT, PUNCH_UP_FERIT, PUNCH_DOWN_FERIT,
	ARMED_STAND_NORMAL_FERIT, ARMED_STAND_UP_FERIT, ARMED_STAND_LEFT_FERIT, ARMED_STAND_RIGHT_FERIT, ARMED_MOVE_LEFT_FERIT, ARMED_MOVE_RIGHT_FERIT, ARMED_MOVE_UP_FERIT, ARMED_MOVE_DOWN_FERIT,
	MORT, MORT_FERIT
};

vector<vector<vector<PlayerAnims>>> Animacions = {
	{{ STAND_NORMAL, STAND_UP, STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, PUNCH_LEFT, PUNCH_RIGHT, PUNCH_UP, PUNCH_DOWN, MORT },
	{ ARMED_STAND_NORMAL, ARMED_STAND_UP, ARMED_STAND_LEFT, ARMED_STAND_RIGHT, ARMED_MOVE_LEFT, ARMED_MOVE_RIGHT, ARMED_MOVE_UP, ARMED_MOVE_DOWN}}, //SI PORTA ARMA EL PUNCH ES ESTATIC NORMAL
	{{ STAND_NORMAL_FERIT, STAND_UP_FERIT, STAND_LEFT_FERIT, STAND_RIGHT_FERIT, MOVE_LEFT_FERIT, MOVE_RIGHT_FERIT, MOVE_UP_FERIT, MOVE_DOWN_FERIT, PUNCH_LEFT_FERIT, PUNCH_RIGHT_FERIT, PUNCH_UP_FERIT, PUNCH_DOWN_FERIT, MORT_FERIT },
	{ ARMED_STAND_NORMAL_FERIT, ARMED_STAND_UP_FERIT, ARMED_STAND_LEFT_FERIT, ARMED_STAND_RIGHT_FERIT, ARMED_MOVE_LEFT_FERIT, ARMED_MOVE_RIGHT_FERIT, ARMED_MOVE_UP_FERIT, ARMED_MOVE_DOWN_FERIT}} //SI PORTA ARMA EL PUNCH ES ESTATIC NORMAL

};	


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	porta_arma = false;
	ferit = false;
	mort = false;
	spritesheet.loadFromFile("images/Solid_snake.png", TEXTURE_PIXEL_FORMAT_RGBA, ferit);	//SOLID SNAKE ES: 368x189 (1 pixel es 0.0027 en x i 0.0053 en y)
	sprite = Sprite::createSprite(glm::ivec2(16*2,32*2), glm::vec2(PIXEL_X *16, PIXEL_Y *32), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(42);
	

		//==============================
		//			STANDS 
		//==============================
		sprite->setAnimationSpeed(STAND_NORMAL, 8);
		sprite->addKeyframe(STAND_NORMAL, glm::vec2(PIXEL_X*2, PIXEL_Y*26)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix

		sprite->setAnimationSpeed(STAND_UP, 8);
		sprite->addKeyframe(STAND_UP, glm::vec2(PIXEL_X *21, PIXEL_Y * 26)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix
		
		sprite->setAnimationSpeed(STAND_LEFT, 8);	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(STAND_LEFT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 61));

		sprite->setAnimationSpeed(STAND_RIGHT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(PIXEL_X *21, PIXEL_Y * 61));

		//==============================
		//			MORT
		//==============================

		sprite->setAnimationSpeed(MORT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(MORT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34 + 19 + 36), PIXEL_Y * 61));

		//==============================
		//			MOVES
		//==============================
		
		sprite->setAnimationSpeed(MOVE_LEFT, 8); // Inicialment STAND LEFT, despres les animacions de caminar
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X*2, PIXEL_Y * 61)); //Stand left
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * (36+11), PIXEL_Y * 61));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * (36+11+38), PIXEL_Y * 61));
		
		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * 21, PIXEL_Y * 61)); //Stand right
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8+1), PIXEL_Y * 61));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19+19+1), PIXEL_Y * 61));

		sprite->setAnimationSpeed(MOVE_UP, 8);
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * 21, PIXEL_Y * 26)); //Stand up
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * (3 + 54 + 8+1), PIXEL_Y * 26));
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19+1), PIXEL_Y * 26));

		sprite->setAnimationSpeed(MOVE_DOWN, 8);
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X*2, PIXEL_Y * 26)); //Stand normal
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (36 + 10+1), PIXEL_Y * 26));
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (36 + 10 + 38+1), PIXEL_Y * 26));


		//==============================
		//			PUNCH (UNARMED)
		//==============================

		sprite->setAnimationSpeed(PUNCH_LEFT, 8);
		sprite->addKeyframe(PUNCH_LEFT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34), PIXEL_Y * 61)); //PUNCH DOWN

		sprite->setAnimationSpeed(PUNCH_RIGHT, 8);
		sprite->addKeyframe(PUNCH_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34 + 19), PIXEL_Y * 61)); //Stand normal

		sprite->setAnimationSpeed(PUNCH_UP, 8);
		sprite->addKeyframe(PUNCH_UP, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34+19), PIXEL_Y * 26)); //Stand normal

		sprite->setAnimationSpeed(PUNCH_DOWN, 8);
		sprite->addKeyframe(PUNCH_DOWN, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1 + 136+34), PIXEL_Y * 26)); //Stand normal


		//==============================
		//			ARMED STANDS	(+136 PIXELS A LA DRETA)
		//==============================

		sprite->setAnimationSpeed(ARMED_STAND_NORMAL, 8);
		sprite->addKeyframe(ARMED_STAND_NORMAL, glm::vec2(PIXEL_X * (2+136), PIXEL_Y * 26)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix

		sprite->setAnimationSpeed(ARMED_STAND_UP, 8);
		sprite->addKeyframe(ARMED_STAND_UP, glm::vec2(PIXEL_X * (21+136), PIXEL_Y * 26)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix

		sprite->setAnimationSpeed(ARMED_STAND_LEFT, 8);	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(ARMED_STAND_LEFT, glm::vec2(PIXEL_X * (2 + 136), PIXEL_Y * 61));

		sprite->setAnimationSpeed(ARMED_STAND_RIGHT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(ARMED_STAND_RIGHT, glm::vec2(PIXEL_X * (21 + 136), PIXEL_Y * 61));

		//==============================
		//			ARMED MOVES		(+136 PIXELS A LA DRETA)
		//==============================

		sprite->setAnimationSpeed(ARMED_MOVE_LEFT, 8); // Inicialment STAND LEFT, despres les animacions de caminar
		sprite->addKeyframe(ARMED_MOVE_LEFT, glm::vec2(PIXEL_X * (2+136), PIXEL_Y * 61)); //Stand left
		sprite->addKeyframe(ARMED_MOVE_LEFT, glm::vec2(PIXEL_X * (36 + 11 + 136), PIXEL_Y * 61));
		sprite->addKeyframe(ARMED_MOVE_LEFT, glm::vec2(PIXEL_X * (36 + 11 + 38 + 136), PIXEL_Y * 61));

		sprite->setAnimationSpeed(ARMED_MOVE_RIGHT, 8);
		sprite->addKeyframe(ARMED_MOVE_RIGHT, glm::vec2(PIXEL_X * (21 + 136), PIXEL_Y * 61)); //Stand right
		sprite->addKeyframe(ARMED_MOVE_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 1 + 136), PIXEL_Y * 61));
		sprite->addKeyframe(ARMED_MOVE_RIGHT, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1 + 136), PIXEL_Y * 61));

		sprite->setAnimationSpeed(ARMED_MOVE_UP, 8);
		sprite->addKeyframe(ARMED_MOVE_UP, glm::vec2(PIXEL_X * (21 + 136), PIXEL_Y * 26)); //Stand up
		sprite->addKeyframe(ARMED_MOVE_UP, glm::vec2(PIXEL_X * (3 + 54 + 8 + 1 + 136), PIXEL_Y * 26));
		sprite->addKeyframe(ARMED_MOVE_UP, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1 + 136), PIXEL_Y * 26));

		sprite->setAnimationSpeed(ARMED_MOVE_DOWN, 8);
		sprite->addKeyframe(ARMED_MOVE_DOWN, glm::vec2(PIXEL_X * (2 + 136), PIXEL_Y * 26)); //Stand normal
		sprite->addKeyframe(ARMED_MOVE_DOWN, glm::vec2(PIXEL_X * (36 + 10 + 1 + 136), PIXEL_Y * 26));
		sprite->addKeyframe(ARMED_MOVE_DOWN, glm::vec2(PIXEL_X * (36 + 10 + 38 + 1 + 136), PIXEL_Y * 26));
		

		//==============================
		//			FERIT		(26 es 120 i 61 es 155)
		//==============================

		//==============================
		//			STANDS	
		//==============================
		sprite->setAnimationSpeed(STAND_NORMAL_FERIT, 8);
		sprite->addKeyframe(STAND_NORMAL_FERIT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 120)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix

		sprite->setAnimationSpeed(STAND_UP_FERIT, 8);
		sprite->addKeyframe(STAND_UP_FERIT, glm::vec2(PIXEL_X * 21, PIXEL_Y * 120)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix

		sprite->setAnimationSpeed(STAND_LEFT_FERIT, 8);	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(STAND_LEFT_FERIT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 155));

		sprite->setAnimationSpeed(STAND_RIGHT_FERIT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(STAND_RIGHT_FERIT, glm::vec2(PIXEL_X * 21, PIXEL_Y * 155));

		//==============================
		//			MORT_ferit
		//==============================

		sprite->setAnimationSpeed(MORT_FERIT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(MORT_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34 + 19 + 36), PIXEL_Y * 155));

		//==============================
		//			MOVES
		//==============================

		sprite->setAnimationSpeed(MOVE_LEFT_FERIT, 8); // Inicialment STAND LEFT, despres les animacions de caminar
		sprite->addKeyframe(MOVE_LEFT_FERIT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 155)); //Stand left
		sprite->addKeyframe(MOVE_LEFT_FERIT, glm::vec2(PIXEL_X* (36 + 11), PIXEL_Y * 155));
		sprite->addKeyframe(MOVE_LEFT_FERIT, glm::vec2(PIXEL_X* (36 + 11 + 38), PIXEL_Y * 155));

		sprite->setAnimationSpeed(MOVE_RIGHT_FERIT, 8);
		sprite->addKeyframe(MOVE_RIGHT_FERIT, glm::vec2(PIXEL_X * 21, PIXEL_Y * 155)); //Stand right
		sprite->addKeyframe(MOVE_RIGHT_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 1), PIXEL_Y * 155));
		sprite->addKeyframe(MOVE_RIGHT_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 19 + 19 + 1), PIXEL_Y * 155));

		sprite->setAnimationSpeed(MOVE_UP_FERIT, 8);
		sprite->addKeyframe(MOVE_UP_FERIT, glm::vec2(PIXEL_X * 21, PIXEL_Y * 120)); //Stand up
		sprite->addKeyframe(MOVE_UP_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 1), PIXEL_Y * 120));
		sprite->addKeyframe(MOVE_UP_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 19 + 19 + 1), PIXEL_Y * 120));

		sprite->setAnimationSpeed(MOVE_DOWN_FERIT, 8);
		sprite->addKeyframe(MOVE_DOWN_FERIT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 120)); //Stand normal
		sprite->addKeyframe(MOVE_DOWN_FERIT, glm::vec2(PIXEL_X* (36 + 10 + 1), PIXEL_Y * 120));
		sprite->addKeyframe(MOVE_DOWN_FERIT, glm::vec2(PIXEL_X* (36 + 10 + 38 + 1), PIXEL_Y * 120));


		//==============================
		//			PUNCH (UNARMED)
		//==============================

		sprite->setAnimationSpeed(PUNCH_LEFT_FERIT, 8);
		sprite->addKeyframe(PUNCH_LEFT_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34), PIXEL_Y * 155)); //PUNCH DOWN

		sprite->setAnimationSpeed(PUNCH_RIGHT_FERIT, 8);
		sprite->addKeyframe(PUNCH_RIGHT_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34 + 19), PIXEL_Y * 155)); //Stand normal

		sprite->setAnimationSpeed(PUNCH_UP_FERIT, 8);
		sprite->addKeyframe(PUNCH_UP_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34 + 19), PIXEL_Y * 120)); //Stand normal

		sprite->setAnimationSpeed(PUNCH_DOWN_FERIT, 8);
		sprite->addKeyframe(PUNCH_DOWN_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34), PIXEL_Y * 120)); //Stand normal


		//==============================
		//			ARMED STANDS	(+136 PIXELS A LA DRETA)
		//==============================

		sprite->setAnimationSpeed(ARMED_STAND_NORMAL_FERIT, 8);
		sprite->addKeyframe(ARMED_STAND_NORMAL_FERIT, glm::vec2(PIXEL_X* (2 + 136), PIXEL_Y * 120)); //DEFINITIU, 2 pixel a la dreta i 120 cap a baix

		sprite->setAnimationSpeed(ARMED_STAND_UP_FERIT, 8);
		sprite->addKeyframe(ARMED_STAND_UP_FERIT, glm::vec2(PIXEL_X* (21 + 136), PIXEL_Y * 120)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix

		sprite->setAnimationSpeed(ARMED_STAND_LEFT_FERIT, 8);	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(ARMED_STAND_LEFT_FERIT, glm::vec2(PIXEL_X* (2 + 136), PIXEL_Y * 155));

		sprite->setAnimationSpeed(ARMED_STAND_RIGHT_FERIT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(ARMED_STAND_RIGHT_FERIT, glm::vec2(PIXEL_X* (21 + 136), PIXEL_Y * 155));

		//==============================
		//			ARMED MOVES		(+136 PIXELS A LA DRETA)
		//==============================

		sprite->setAnimationSpeed(ARMED_MOVE_LEFT_FERIT, 8); // Inicialment STAND LEFT, despres les animacions de caminar
		sprite->addKeyframe(ARMED_MOVE_LEFT_FERIT, glm::vec2(PIXEL_X* (2 + 136), PIXEL_Y * 155)); //Stand left
		sprite->addKeyframe(ARMED_MOVE_LEFT_FERIT, glm::vec2(PIXEL_X* (36 + 11 + 136), PIXEL_Y * 155));
		sprite->addKeyframe(ARMED_MOVE_LEFT_FERIT, glm::vec2(PIXEL_X* (36 + 11 + 38 + 136), PIXEL_Y * 155));

		sprite->setAnimationSpeed(ARMED_MOVE_RIGHT_FERIT, 8);
		sprite->addKeyframe(ARMED_MOVE_RIGHT_FERIT, glm::vec2(PIXEL_X* (21 + 136), PIXEL_Y * 155)); //Stand right
		sprite->addKeyframe(ARMED_MOVE_RIGHT_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 1 + 136), PIXEL_Y * 155));
		sprite->addKeyframe(ARMED_MOVE_RIGHT_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 19 + 19 + 1 + 136), PIXEL_Y * 155));

		sprite->setAnimationSpeed(ARMED_MOVE_UP_FERIT, 8);
		sprite->addKeyframe(ARMED_MOVE_UP_FERIT, glm::vec2(PIXEL_X* (21 + 136), PIXEL_Y * 120)); //Stand up
		sprite->addKeyframe(ARMED_MOVE_UP_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 1 + 136), PIXEL_Y * 120));
		sprite->addKeyframe(ARMED_MOVE_UP_FERIT, glm::vec2(PIXEL_X* (3 + 54 + 8 + 19 + 19 + 1 + 136), PIXEL_Y * 120));

		sprite->setAnimationSpeed(ARMED_MOVE_DOWN_FERIT, 8);
		sprite->addKeyframe(ARMED_MOVE_DOWN_FERIT, glm::vec2(PIXEL_X* (2 + 136), PIXEL_Y * 120)); //Stand normal
		sprite->addKeyframe(ARMED_MOVE_DOWN_FERIT, glm::vec2(PIXEL_X* (36 + 10 + 1 + 136), PIXEL_Y * 120));
		sprite->addKeyframe(ARMED_MOVE_DOWN_FERIT, glm::vec2(PIXEL_X* (36 + 10 + 38 + 1 + 136), PIXEL_Y * 120));

	if(ferit) sprite->changeAnimation(Animacions[1][0][0]);
	else sprite->changeAnimation(Animacions[0][0][0]);

	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	int a = porta_arma;
	int f = ferit;
	sprite->update(deltaTime);

	if(mort) {
		if (sprite->animation() != Animacions[f][0][12])
			sprite->changeAnimation(Animacions[f][0][12]);
	}
	else if(Game::instance().getKey(GLFW_KEY_LEFT))
	{
		if(sprite->animation() != Animacions[f][a][4])
			sprite->changeAnimation(Animacions[f][a][4]);
		posPlayer.x -= 2;
		if(map->collisionMoveLeft(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(Animacions[f][a][2]);
		}
	}
	else if(Game::instance().getKey(GLFW_KEY_RIGHT))
	{
		if(sprite->animation() != Animacions[f][a][5])
			sprite->changeAnimation(Animacions[f][a][5]);
		posPlayer.x += 2;
		if(map->collisionMoveRight(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(Animacions[f][a][3]);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_UP))
	{
		if (sprite->animation() != Animacions[f][a][6])
			sprite->changeAnimation(Animacions[f][a][6]);
		
		posPlayer.y -= 2;
		
		if (map->collisionMoveUP(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.y += 2;
			sprite->changeAnimation(Animacions[f][a][1]);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_DOWN))
	{
		if (sprite->animation() != Animacions[f][a][7])
			sprite->changeAnimation(Animacions[f][a][7]);

		posPlayer.y += 2;

		if (map->collisionMoveDown(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.y -= 2;
			sprite->changeAnimation(Animacions[f][a][0]);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_X) && !porta_arma)		// PUNCH NOMÉS SI NO PORTA ARMA
	{
		if (sprite->animation() == Animacions[f][a][0] || sprite->animation() == Animacions[f][a][7]){
			sprite->changeAnimation(Animacions[f][a][11]);
		}
		else if (sprite->animation() == Animacions[f][a][1] || sprite->animation() == Animacions[f][a][6]) {
			sprite->changeAnimation(Animacions[f][a][10]);
		}
		else if (sprite->animation() == Animacions[f][a][2] || sprite->animation() == Animacions[f][a][4]){
			sprite->changeAnimation(Animacions[f][a][8]);
		}
		else if (sprite->animation() == Animacions[f][a][3] || sprite->animation() == Animacions[f][a][5]) {
			sprite->changeAnimation(Animacions[f][a][9]);
		}
			
	}
	else
	{
		if(sprite->animation() == Animacions[f][a][4])
			sprite->changeAnimation(Animacions[f][a][2]);
		else if(sprite->animation() == Animacions[f][a][5])
			sprite->changeAnimation(Animacions[f][a][3]);
		else if (sprite->animation() == Animacions[f][a][6])
			sprite->changeAnimation(Animacions[f][a][1]);
		else if (sprite->animation() == Animacions[f][a][7])
			sprite->changeAnimation(Animacions[f][a][0]);
		// Acabar Animacions de punch
		else if (!porta_arma && sprite->animation() == Animacions[f][a][11])
			sprite->changeAnimation(Animacions[f][a][0]);
		else if (!porta_arma && sprite->animation() == Animacions[f][a][10])
			sprite->changeAnimation(Animacions[f][a][1]);
		else if (!porta_arma && sprite->animation() == Animacions[f][a][8])
			sprite->changeAnimation(Animacions[f][a][2]);
		else if (!porta_arma && sprite->animation() == Animacions[f][a][9])
			sprite->changeAnimation(Animacions[f][a][3]);
	}
	
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite->render();
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}




