#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4


enum PlayerAnims
{
	STAND_NORMAL, STAND_UP, STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	bJumping = false;
	spritesheet.loadFromFile("images/Solid_snake.png", TEXTURE_PIXEL_FORMAT_RGBA);	//SOLID SNAKE ES: 368x189 (1 pixel es 0.0027 en x i 0.0053 en y)
	sprite = Sprite::createSprite(glm::ivec2(18,35), glm::vec2(0.048, 0.18), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(6);
	
		sprite->setAnimationSpeed(STAND_NORMAL, 8);
		sprite->addKeyframe(STAND_NORMAL, glm::vec2(0.0027f, 0.0053f*25)); //DEFINITIU, 1 pixel a la dreta i 25 cap a baix

		sprite->setAnimationSpeed(STAND_UP, 8);
		sprite->addKeyframe(STAND_UP, glm::vec2(0.0027f*20, 0.0053f * 25)); //DEFINITIU, (1+18+1) pixel a la dreta i 25 cap a baix
		
		sprite->setAnimationSpeed(STAND_LEFT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge) cap a baix
		sprite->addKeyframe(STAND_LEFT, glm::vec2(0.0027f, 0.0053f * 59));

		sprite->setAnimationSpeed(STAND_RIGHT, 8);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge) cap a baix i (1 + 18) pixels a la dreta
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.0027f*20, 0.0053f * 59));
		
		sprite->setAnimationSpeed(MOVE_LEFT, 8); // Inicialment STAND LEFT, despres les animacions de caminar
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.0027f, 0.0053f * 59)); //Stand normal
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.0027f* (36+9), 0.0053f * 59));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.0027f * (36+9+38), 0.0053f * 59));
		
		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.0027f * 20, 0.0053f * 59)); //Stand normal
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.0027f * (3 + 54 + 8), 0.0053f * 59));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.0027f * (3 + 54 + 8 + 19+19), 0.0053f * 59));
		
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	sprite->update(deltaTime);
	if(Game::instance().getKey(GLFW_KEY_LEFT))
	{
		if(sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posPlayer.x -= 2;
		if(map->collisionMoveLeft(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if(Game::instance().getKey(GLFW_KEY_RIGHT))
	{
		if(sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		posPlayer.x += 2;
		if(map->collisionMoveRight(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_UP))
	{
		//if (sprite->animation() != MOVE_RIGHT)
		//	sprite->changeAnimation(MOVE_RIGHT);
		
		posPlayer.y -= 2;
		
		if (map->collisionMoveUP(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.y += 2;
			//sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_DOWN))
	{
		//if (sprite->animation() != MOVE_RIGHT)
		//	sprite->changeAnimation(MOVE_RIGHT);

		posPlayer.y += 2;

		if (map->collisionMoveDown(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.y -= 2;
			//sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else
	{
		if(sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if(sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
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




