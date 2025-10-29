#include <cmath>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"
#include "Enemy.h"



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
	MORT, MORT_FERIT, CAIXA
};

vector<vector<vector<PlayerAnims>>> Animacions = {
	{{ STAND_NORMAL, STAND_UP, STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, PUNCH_LEFT, PUNCH_RIGHT, PUNCH_UP, PUNCH_DOWN, MORT },
	{ ARMED_STAND_NORMAL, ARMED_STAND_UP, ARMED_STAND_LEFT, ARMED_STAND_RIGHT, ARMED_MOVE_LEFT, ARMED_MOVE_RIGHT, ARMED_MOVE_UP, ARMED_MOVE_DOWN}}, //SI PORTA ARMA EL PUNCH ES ESTATIC NORMAL
	{{ STAND_NORMAL_FERIT, STAND_UP_FERIT, STAND_LEFT_FERIT, STAND_RIGHT_FERIT, MOVE_LEFT_FERIT, MOVE_RIGHT_FERIT, MOVE_UP_FERIT, MOVE_DOWN_FERIT, PUNCH_LEFT_FERIT, PUNCH_RIGHT_FERIT, PUNCH_UP_FERIT, PUNCH_DOWN_FERIT, MORT_FERIT },
	{ ARMED_STAND_NORMAL_FERIT, ARMED_STAND_UP_FERIT, ARMED_STAND_LEFT_FERIT, ARMED_STAND_RIGHT_FERIT, ARMED_MOVE_LEFT_FERIT, ARMED_MOVE_RIGHT_FERIT, ARMED_MOVE_UP_FERIT, ARMED_MOVE_DOWN_FERIT}} //SI PORTA ARMA EL PUNCH ES ESTATIC NORMAL

};	


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene &sc, bool render)
{
	vida = 7;
	scene = &sc;
	porta_arma = false;
	ferit = false;
	mort = false;
	god = false;
	rend = render;
	lastButton = ' ';
	spritesheet.loadFromFile("images/Solid_snake.png", TEXTURE_PIXEL_FORMAT_RGBA); 	//SOLID SNAKE ES: 368x189 (1 pixel es 0.0027 en x i 0.0053 en y)
	sprite = Sprite::createSprite(glm::ivec2(16*2,32*2), glm::vec2(PIXEL_X *16, PIXEL_Y *32), &spritesheet, &shaderProgram);
	spriteCapsa = Sprite::createSprite(glm::ivec2(16 * 2, 24 * 2), glm::vec2(PIXEL_X * 16, PIXEL_Y * 24), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(43);

	sprite->setAnimationSpeed(CAIXA, 8);
	sprite->addKeyframe(CAIXA, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34 + 19 + 36), PIXEL_Y * 26)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix


	spriteCapsa->setNumberAnimations(1);
	spriteCapsa->setAnimationSpeed(0, 8);
	spriteCapsa->addKeyframe(0, glm::vec2(PIXEL_X * (3 + 54 + 8 + 19 + 19 + 1 + 136 + 34 + 19 + 36), PIXEL_Y * 26)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix
	spriteCapsa->changeAnimation(0);
		//==============================
		//			STANDS 
		//==============================
		sprite->setAnimationSpeed(STAND_NORMAL, 8);
		sprite->addKeyframe(STAND_NORMAL, glm::vec2(PIXEL_X*2, PIXEL_Y*26)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix

		sprite->setAnimationSpeed(STAND_UP, 8);
		sprite->addKeyframe(STAND_UP, glm::vec2(PIXEL_X *21, PIXEL_Y * 26)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix
		
		sprite->setAnimationSpeed(STAND_LEFT, 8); 	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(STAND_LEFT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 61));

		sprite->setAnimationSpeed(STAND_RIGHT, 8); 	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(PIXEL_X *21, PIXEL_Y * 61));

		//==============================
		//			MORT
		//==============================

		sprite->setAnimationSpeed(MORT, 8); 	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
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
		
		sprite->setAnimationSpeed(ARMED_STAND_LEFT, 8); 	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(ARMED_STAND_LEFT, glm::vec2(PIXEL_X * (2 + 136), PIXEL_Y * 61));

		sprite->setAnimationSpeed(ARMED_STAND_RIGHT, 8); 	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
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

		sprite->setAnimationSpeed(STAND_LEFT_FERIT, 8); 	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(STAND_LEFT_FERIT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 155));

		sprite->setAnimationSpeed(STAND_RIGHT_FERIT, 8); 	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(STAND_RIGHT_FERIT, glm::vec2(PIXEL_X * 21, PIXEL_Y * 155));

		//==============================
		//			MORT_ferit
		//==============================

		sprite->setAnimationSpeed(MORT_FERIT, 8); 	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
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

		sprite->setAnimationSpeed(ARMED_STAND_LEFT_FERIT, 8); 	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(ARMED_STAND_LEFT_FERIT, glm::vec2(PIXEL_X* (2 + 136), PIXEL_Y * 155));

		sprite->setAnimationSpeed(ARMED_STAND_RIGHT_FERIT, 8); 	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
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

	if(ferit) sprite->changeAnimation(Animacions[1][0][12]);
	else sprite->changeAnimation(Animacions[0][0][0]);

	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	spriteCapsa->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	if (scene->victory) {
		if (porta_arma) sprite->changeAnimation(Animacions[0][1][0]);
		else sprite->changeAnimation(Animacions[0][0][0]);
		return;
	}
	int a = porta_arma;
	if (vida <= 0) mort = true;
	else {
		// if (vida < 3) ferit = true;
		if (ferit_animacio % 10 == 9) { // està a 9 en comptes de a 0 perque aixi no entra de normal
			ferit = !ferit;
			int f2, f1;
			if (ferit) {
				f2 = 0;
				f1 = 1;
			}
			else {
				f2 = 1;
				f1 = 0;
			}
			for (int i = 0; i < Animacions[f2][a].size(); ++i) {
				if (sprite->animation() == Animacions[f2][a][i]) { // Ha crasheao aqui
					sprite->changeAnimation(Animacions[f1][a][i]);
					break;
				}
			}
		}

		if (ferit_animacio > 0) ferit_animacio -= 1;
		else ferit = false;

	}

	int f = ferit;
	sprite->update(deltaTime);
	spriteCapsa->update(deltaTime);
	shootTimer += deltaTime;
	pegaTimer += deltaTime;
	

    // handle item action cooldown (prevents multiple triggers on single keypress)
    if (itemActionCooldownMs > 0) itemActionCooldownMs -= deltaTime;

	if(mort) {
		if (sprite->animation() != Animacions[f][0][12])
			sprite->changeAnimation(Animacions[f][0][12]);
		else if (Game::instance().getKey(GLFW_KEY_ENTER)) {
			if (SoundManager::instance().isSoundPlaying("game_over")) {
				SoundManager::instance().stopSound("game_over");
			}
			SoundManager::instance().playSound("select");
			lastButton = ' ';
			vida = 7;
			mort = false;
			porta_arma = false;
			ha_disparat = false;
			hasKey = false;
			ferit = 0;
			scene->DeleteObjectsAndEnemies();
			scene->ChargeEnemiesAndObjects();
			inventari.clear();
			sprite->changeAnimation(Animacions[0][0][0]);
			scene->tp_to_map(1);
			scene->Death(1);
		}
		else if (Game::instance().getKey(GLFW_KEY_R)) {
			if (SoundManager::instance().isSoundPlaying("game_over")) {
				SoundManager::instance().stopSound("game_over");
			}
			SoundManager::instance().playSound("select");
			inventari.clear();
			scene->GoToMainMenu();
			lastButton = ' ';
			vida = 7;
			mort = false;
			porta_arma = false;
			ha_disparat = false;
			hasKey = false;
			ferit = 0;
			sprite->changeAnimation(Animacions[0][0][0]);
		}
		return;
	}
	else if (scene->CurrentMap == 0) {
		if (countSelect > 0) countSelect -= 1;
		if ((Game::instance().getKey(GLFW_KEY_UP) || Game::instance().getKey(GLFW_KEY_W)) && countSelect == 0) {
			if (scene->currentOption == 1) {
				scene->currentOption = 4;
			}
			else scene->currentOption -= 1;
			countSelect = 10;
			scene->hud->menuCount = 30;
			SoundManager::instance().playSound("option");
		}
		else if ((Game::instance().getKey(GLFW_KEY_DOWN) || Game::instance().getKey(GLFW_KEY_S)) && countSelect == 0) {
			if (scene->currentOption == 4) {
				scene->currentOption = 1;
			}
			else scene->currentOption += 1;
			scene->hud->menuCount = 30;
			countSelect = 10;
			SoundManager::instance().playSound("option");
		}
		if (Game::instance().getKey(GLFW_KEY_ENTER)) {
			lastButton = ' ';
			SoundManager::instance().playSound("select");
			if (scene->currentOption == 1) {
				scene->ChangeMap(0);
			}
			else if (scene->currentOption == 2) {
				scene->Instructions();
			}
			else if (scene->currentOption == 3) {
				scene->Credits();
			}
			else if (scene->currentOption == 4) {
				exit(0);
			}
		}
		return;
	}
	else if (scene->CurrentMap == -1 || scene->CurrentMap == -2) {
		if (Game::instance().getKey(GLFW_KEY_R)) {
			lastButton = ' ';
			scene->GoToMainMenu();
		}
		return;
	}
	else if(Game::instance().getKey(GLFW_KEY_LEFT) || Game::instance().getKey(GLFW_KEY_A))
	{
		lastButton = 'A';
		if(sprite->animation() != Animacions[f][a][4])
			sprite->changeAnimation(Animacions[f][a][4]);
		posPlayer.x -= 2;
		if (posPlayer.x <= 0) {
			scene->ChangeMap(1);
		}
		else if ((scene->CurrentMap == 6 || scene-> CurrentMap == 7) && posPlayer.x <= 30 && posPlayer.y >= 176 && posPlayer.y <= 224) {
			if (!SoundManager::instance().isSoundPlaying("door")) {
				SoundManager::instance().playSound("door");
			}
			scene->ChangeMap(1);
		}
		else if (scene->CurrentMap == 9 && posPlayer.x <= 30 && posPlayer.y >= 86 && posPlayer.y <= 132) {
			if (!SoundManager::instance().isSoundPlaying("door")) {
				SoundManager::instance().playSound("door");
			}
			scene->ChangeMap(1);
		}
		else if (map->collisionMoveLeft(posPlayer, glm::ivec2(32, 32), scene->CurrentMap))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(Animacions[f][a][2]);
		}
	}
	else if(Game::instance().getKey(GLFW_KEY_RIGHT) || Game::instance().getKey(GLFW_KEY_D))
	{
		lastButton = 'D';
		if(sprite->animation() != Animacions[f][a][5])
			sprite->changeAnimation(Animacions[f][a][5]);
		posPlayer.x += 2;
		if (posPlayer.x >= ((map->mapSize[0])*16)-32) {
			scene->ChangeMap(2);
		}
		else if ((scene->CurrentMap == 5 || scene->CurrentMap == 6) && posPlayer.x >= 578 && posPlayer.y >= 176 && posPlayer.y <= 224) {
			if (!SoundManager::instance().isSoundPlaying("door")) {
				SoundManager::instance().playSound("door");
			}
			scene->ChangeMap(2);
		}
		else if (scene->CurrentMap == 10 && posPlayer.x >= 578 && posPlayer.y >= 86 && posPlayer.y <= 132) {
			if (!SoundManager::instance().isSoundPlaying("door")) {
				SoundManager::instance().playSound("door");
			}
			scene->ChangeMap(2);
		}
		else if (map->collisionMoveRight(posPlayer, glm::ivec2(32, 32), scene->CurrentMap))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(Animacions[f][a][3]);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_UP) || Game::instance().getKey(GLFW_KEY_W))
	{
		lastButton = 'W';
		if (sprite->animation() != Animacions[f][a][6])
			sprite->changeAnimation(Animacions[f][a][6]);
		
		posPlayer.y -= 2;
		if (posPlayer.y <= 0) {
			scene->ChangeMap(3);
		}
		else if (scene->CurrentMap == 4 && posPlayer.y <= 32 && posPlayer.x >= 352 && posPlayer.x <= 384) {
			if (!SoundManager::instance().isSoundPlaying("door")) {
				SoundManager::instance().playSound("door");
			}
			scene->ChangeMap(3);
		}
		else if (scene->CurrentMap == 5 && posPlayer.y <= 32 && posPlayer.x >= 96 && posPlayer.x <= 128 && hasKey) {
			if (!SoundManager::instance().isSoundPlaying("doorSuccess")) {
				SoundManager::instance().playSound("doorSuccess");
			}
			scene->ChangeMap(3);
		}
		else if (scene->CurrentMap == 5 && posPlayer.y <= 32 && posPlayer.x >= 96 && posPlayer.x <= 128) {
			if (!SoundManager::instance().isSoundPlaying("doorError")) {
				SoundManager::instance().playSound("doorError");
			}
			posPlayer.y += 2;
			sprite->changeAnimation(Animacions[f][a][1]);
		}
		else if (map->collisionMoveUP(posPlayer, glm::ivec2(32, 32), scene->CurrentMap))
		{
			posPlayer.y += 2;
			sprite->changeAnimation(Animacions[f][a][1]);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_DOWN) || Game::instance().getKey(GLFW_KEY_S))
	{
		lastButton = 'S';
		if (sprite->animation() != Animacions[f][a][7])
			sprite->changeAnimation(Animacions[f][a][7]);

		posPlayer.y += 2;

		if (posPlayer.y >= ((map->mapSize[1]) * 16) - 64) {
			scene->ChangeMap(4);
		}
		else if (scene->CurrentMap == 5 && posPlayer.y >= 386 && posPlayer.x >= 352 && posPlayer.x <= 384) {
			if (!SoundManager::instance().isSoundPlaying("door")) {
				SoundManager::instance().playSound("door");
			}
			scene->ChangeMap(4);
		}
		else if (map->collisionMoveDown(posPlayer, glm::ivec2(32, 32),scene->CurrentMap))
		{
			posPlayer.y -= 2;
			sprite->changeAnimation(Animacions[f][a][0]);
		}
	}
	else if (!cajaActive && Game::instance().getKey(GLFW_KEY_Z) && !porta_arma && pegaTimer >= 50) 		// PUNCH NOMÉS SI NO PORTA ARMA
	{
		lastButton = 'Z';
		if(!HaPegat) handlePunchNoWeapon(f, a);
	}
	else if (lastButton != 'K' && Game::instance().getKey(GLFW_KEY_K)) {
		lastButton = 'K';
		scene->tp_to_map(5);
	}
	else if (lastButton != 'P' && Game::instance().getKey(GLFW_KEY_P)) {
		lastButton = 'P';
		scene->tp_to_map(1);
	}
	else if (lastButton != 'B' && Game::instance().getKey(GLFW_KEY_B)) {
		lastButton = 'B';
		scene->tp_to_map(11);
	}
	else if (lastButton != 'N' && Game::instance().getKey(GLFW_KEY_N)) {
		lastButton = 'N';
		if (scene->detectable)  scene->detectable = false;
		else scene->detectable = true;
	}
	else if (lastButton != 'G' && Game::instance().getKey(GLFW_KEY_G)) {
		lastButton = 'G';
		if (!god) {
			vida = 7;
			ferit = 0;
			god = true;
			if (ferit != f) {
				if (sprite->animation() == Animacions[f][a][0]) sprite->changeAnimation(Animacions[ferit][a][0]);
				else if (sprite->animation() == Animacions[f][a][7]) sprite->changeAnimation(Animacions[ferit][a][7]);
				else if (sprite->animation() == Animacions[f][a][1]) sprite->changeAnimation(Animacions[ferit][a][1]);
				else if (sprite->animation() == Animacions[f][a][6]) sprite->changeAnimation(Animacions[ferit][a][6]);
				else if (sprite->animation() == Animacions[f][a][2]) sprite->changeAnimation(Animacions[ferit][a][2]);
				else if (sprite->animation() == Animacions[f][a][4]) sprite->changeAnimation(Animacions[ferit][a][4]);
				else if (sprite->animation() == Animacions[f][a][3]) sprite->changeAnimation(Animacions[ferit][a][3]);
				else if (sprite->animation() == Animacions[f][a][5]) sprite->changeAnimation(Animacions[ferit][a][5]);
				f = ferit;
			}
		}
		else god = false;
	}
	else
	{
		if (sprite->animation() == Animacions[f][a][4])
			sprite->changeAnimation(Animacions[f][a][2]);
		else if (sprite->animation() == Animacions[f][a][5])
			sprite->changeAnimation(Animacions[f][a][3]);
		else if (sprite->animation() == Animacions[f][a][6])
			sprite->changeAnimation(Animacions[f][a][1]);
		else if (sprite->animation() == Animacions[f][a][7])
			sprite->changeAnimation(Animacions[f][a][0]);
		// Acabar Animacions de punch
		else if (!porta_arma && sprite->animation() == Animacions[f][a][11])
		{
			pegaTimer = 0;
			sprite->changeAnimation(Animacions[f][a][0]);
			HaPegat = false;
		}
		else if (!porta_arma && sprite->animation() == Animacions[f][a][10])
		{
			pegaTimer = 0;
			sprite->changeAnimation(Animacions[f][a][1]);
			HaPegat = false;
		}
		else if (!porta_arma && sprite->animation() == Animacions[f][a][8])
		{
			pegaTimer = 0;
			sprite->changeAnimation(Animacions[f][a][2]);
			HaPegat = false;
		}
		else if (!porta_arma && sprite->animation() == Animacions[f][a][9])
		{
			pegaTimer = 0;
			sprite->changeAnimation(Animacions[f][a][3]);
			HaPegat = false;
		}
	}
	
	if (Game::instance().getKey(GLFW_KEY_I)) {
		lastButton = 'I';
		GetAllObjects();
	}
	if (Game::instance().getKey(GLFW_KEY_H)) {
		lastButton = 'H';
		if (!SoundManager::instance().isSoundPlaying("heal")) {
			SoundManager::instance().playSound("heal");
		}
		vida = 7;
		ferit = 0;
		if (ferit != f) {
			if (sprite->animation() == Animacions[f][a][0]) sprite->changeAnimation(Animacions[ferit][a][0]);
			else if (sprite->animation() == Animacions[f][a][7]) sprite->changeAnimation(Animacions[ferit][a][7]);
			else if (sprite->animation() == Animacions[f][a][1]) sprite->changeAnimation(Animacions[ferit][a][1]);
			else if (sprite->animation() == Animacions[f][a][6]) sprite->changeAnimation(Animacions[ferit][a][6]);
			else if (sprite->animation() == Animacions[f][a][2]) sprite->changeAnimation(Animacions[ferit][a][2]);
			else if (sprite->animation() == Animacions[f][a][4]) sprite->changeAnimation(Animacions[ferit][a][4]);
			else if (sprite->animation() == Animacions[f][a][3]) sprite->changeAnimation(Animacions[ferit][a][3]);
			else if (sprite->animation() == Animacions[f][a][5]) sprite->changeAnimation(Animacions[ferit][a][5]);
			f = ferit;
		}
	}
    // Inventory cycling with C key
    if (Game::instance().getKey(GLFW_KEY_C) && itemActionCooldownMs <= 0 && !inventari.empty()) {
		lastButton = 'C';
        // if only one item, do nothing
        if (inventari.size() > 1) {
            int oldSelected = selectedItem;
            if (selectedItem < 0) selectedItem = 0;
            else selectedItem = (selectedItem + 1) % int(inventari.size());

            // switching item cancels caja active and makes player detectable again
            if (cajaActive) {
				if (!SoundManager::instance().isSoundPlaying("box")) {
					SoundManager::instance().playSound("box");
				}
                // deactivate box and set player to standing down immediately
                cajaActive = false;
                int fcur = ferit ? 1 : 0;
                int a2 = porta_arma ? 1 : 0;
                // Force stand-down animation (index 0) for current ferit/armed state
                sprite->changeAnimation(Animacions[fcur][a2][0]);
            }
            // If we switched away from a weapon, deactivate porta_arma
            else if (oldSelected >= 0 && oldSelected < int(inventari.size())) {
                 objeto* oldIt = inventari[oldSelected];
                 if (oldIt && oldIt->getSprite() && oldIt->getSprite()->animation() == 1) {
                    // old was weapon and now not selected -> turn off
                    // Also if newly selected is not weapon
                    bool newIsWeapon = false;
                    if (selectedItem >= 0 && selectedItem < int(inventari.size())) {
                        objeto* newIt = inventari[selectedItem];
                        if (newIt && newIt->getSprite() && newIt->getSprite()->animation() == 1) newIsWeapon = true;
                    }
					if (!newIsWeapon) {
						// disable armed state and update sprite animation mapping immediately
						porta_arma = false;
						int fcur = ferit ? 1 : 0;
						// Map any currently armed animation to its unarmed counterpart
						sprite->changeAnimation(Animacions[fcur][0][0]);
					}
                 }
             }
			SoundManager::instance().playSound("change");

            itemActionCooldownMs = 200; // 200ms debounce

            // We don't directly render here (Scene::render will draw the player this frame).
            // The sprite animation/state was already updated above (armed/unarmed mapping), so the next
            // Scene::render call will display the correct appearance immediately.
        }
    }

    // Activate selected item with X key (toggle behavior)
    if (Game::instance().getKey(GLFW_KEY_X) && itemActionCooldownMs <= 0 && selectedItem >= 0 && selectedItem < int(inventari.size())) {
		lastButton = 'X';
        objeto* it = inventari[selectedItem];
        if (it) {
            int typeAnim = 0;
            if (it->getSprite()) typeAnim = it->getSprite()->animation();
            // From objeto::init: animations: 0 Caja, 1 Arma, 2 Vida
            int fcur = ferit ? 1 : 0;
            if (typeAnim == 1) {
                // ARMA: toggle armed state
                if (porta_arma) {
					if (!SoundManager::instance().isSoundPlaying("unequip")) {
						SoundManager::instance().playSound("unequip");
					}
                    // if currently armed and selected item is a weapon, deselect -> deactivate
                    porta_arma = false;
                    // map armed animations to unarmed equivalents
                    for (int i = 0; i <= 11; ++i) {
                        if (sprite->animation() == Animacions[fcur][1][i]) {
                            sprite->changeAnimation(Animacions[fcur][0][i]);
                            break;
                        }
                    }
                } else {
					if (!SoundManager::instance().isSoundPlaying("arma")) {
						SoundManager::instance().playSound("arma");
					}
                    porta_arma = true;
                    // map unarmed animations to armed equivalents
                    for (int i = 0; i <= 11; ++i) {
                        if (sprite->animation() == Animacions[fcur][0][i]) {
                            sprite->changeAnimation(Animacions[fcur][1][i]);
                            break;
                        }
                    }
                }
            }
            else if (typeAnim == 0) {
                // CAIXA: toggle box state
                if (cajaActive) {
					if (!SoundManager::instance().isSoundPlaying("unequip")) {
						SoundManager::instance().playSound("unequip");
					}
                    // deactivate box
                    cajaActive = false;
                    // restore appropriate stand animation
                    int a2 = porta_arma ? 1 : 0;
                } else {
					if (!SoundManager::instance().isSoundPlaying("box")) {
						SoundManager::instance().playSound("box");
					}
                    // activate box
                    cajaActive = true;
                }
            }
            else if (typeAnim == 2) {
				if (!SoundManager::instance().isSoundPlaying("heal")) {
					SoundManager::instance().playSound("heal");
				}
                // VIDA: heal 1 and remove item from inventory (no toggle)
                vida = std::min(7, vida + 3);
                inventari.erase(inventari.begin() + selectedItem);
                if (inventari.empty()) selectedItem = -1;
                else selectedItem = selectedItem % int(inventari.size());
            }
        }
        itemActionCooldownMs = 200;
    }

    // Disparar amb arma fins i tot mentre es mou
    if (!cajaActive && Game::instance().getKey(GLFW_KEY_Z) && porta_arma) {
		lastButton = 'Z';
        tryShoot();
    }

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	spriteCapsa->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

    // Update projectiles after movement
    updateProjectiles(deltaTime);

    // Check pick up objects after movement
    checkObjectPickup();
}

void Player::render() {
	if (rend == false) return;
	if (cajaActive) {
		spriteCapsa->render();
	}
	else sprite->render();
    renderProjectiles();
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

void Player::baixavida(int dg)
{
	ferit_animacio = 60; // dura 60 frames l'animacio de ferit
	if (!god) {
		vida = vida - dg;
		if (vida < 3) {
			ferit = 1;
			int f = 0;
			int a = porta_arma;
			if (sprite->animation() == Animacions[f][a][0]) sprite->changeAnimation(Animacions[ferit][a][0]);
			else if (sprite->animation() == Animacions[f][a][7]) sprite->changeAnimation(Animacions[ferit][a][7]);
			else if (sprite->animation() == Animacions[f][a][1]) sprite->changeAnimation(Animacions[ferit][a][1]);
			else if (sprite->animation() == Animacions[f][a][6]) sprite->changeAnimation(Animacions[ferit][a][6]);
			else if (sprite->animation() == Animacions[f][a][2]) sprite->changeAnimation(Animacions[ferit][a][2]);
			else if (sprite->animation() == Animacions[f][a][4]) sprite->changeAnimation(Animacions[ferit][a][4]);
			else if (sprite->animation() == Animacions[f][a][3]) sprite->changeAnimation(Animacions[ferit][a][3]);
			else if (sprite->animation() == Animacions[f][a][5]) sprite->changeAnimation(Animacions[ferit][a][5]);
		}
		if (vida <= 0) {
			SoundManager::instance().playSound("game_over");
			SoundManager::instance().stopMusic();
			mort = true;
			for each (auto enemy in scene->getEnemies())
			{
				if (enemy->Escena_Original == scene->CurrentMap) enemy->noUpdate = true;
			}
			scene->Death(0);
		}

		// If player was in box state, cancel it and become detectable again
		if (cajaActive) {
			cajaActive = false;
		}
	}
}

glm::ivec2 Player::facingDirFromAnim(int anim) const
{
    // Mirror EnemyFacingFromAnim mapping for player animations
    switch (anim) {
    // Unarmed
    case STAND_LEFT:
    case MOVE_LEFT:
    case STAND_LEFT_FERIT:
    case MOVE_LEFT_FERIT:
        return glm::ivec2(-1, 0);
    case STAND_RIGHT:
    case MOVE_RIGHT:
    case STAND_RIGHT_FERIT:
    case MOVE_RIGHT_FERIT:
        return glm::ivec2(1, 0);
    case STAND_UP:
    case MOVE_UP:
    case STAND_UP_FERIT:
    case MOVE_UP_FERIT:
        return glm::ivec2(0, -1);
    case STAND_NORMAL:
    case MOVE_DOWN:
    case STAND_NORMAL_FERIT:
    case MOVE_DOWN_FERIT:
        return glm::ivec2(0, 1);
    // Armed
    case ARMED_STAND_LEFT:
    case ARMED_MOVE_LEFT:
    case ARMED_STAND_LEFT_FERIT:
    case ARMED_MOVE_LEFT_FERIT:
        return glm::ivec2(-1, 0);
    case ARMED_STAND_RIGHT:
    case ARMED_MOVE_RIGHT:
    case ARMED_STAND_RIGHT_FERIT:
    case ARMED_MOVE_RIGHT_FERIT:
        return glm::ivec2(1, 0);
    case ARMED_STAND_UP:
    case ARMED_MOVE_UP:
    case ARMED_STAND_UP_FERIT:
    case ARMED_MOVE_UP_FERIT:
        return glm::ivec2(0, -1);
    case ARMED_STAND_NORMAL:
    case ARMED_MOVE_DOWN:
    case ARMED_STAND_NORMAL_FERIT:
    case ARMED_MOVE_DOWN_FERIT:
        return glm::ivec2(0, 1);
    default:
        return glm::ivec2(0, 1);
    }
}

void Player::tryShoot()
{
    if (!porta_arma) return;
    if (shootTimer < shootCooldownMs) return;

	SoundManager::instance().playSound("shoot");

    Bullet b;
    // Decideix direcció primer
    b.dir = facingDirFromAnim(sprite->animation());
	ha_disparat = true;
    // Punt d'origen segons direcció: si apuntem amunt, surt del cap; en cas contrari, lleugerament per sobre del centre
    const float muzzleYOffset = -6.0f; // elevar una mica quan no és cap amunt
    float spawnX = posPlayer.x + 16.0f;
    float spawnY;
    if (b.dir.y < 0) {
        // cap amunt -> antiga posició del cap
        spawnY = posPlayer.y + 16.0f;
    } else {
        // resta direccions -> centre elevat
        spawnY = posPlayer.y + 32.0f + muzzleYOffset;
    }
    b.pos = glm::vec2(spawnX, spawnY);

    b.speed = bulletSpeedPxPerMs;
    b.active = true;
    bullets.push_back(b);
    shootTimer = 0;
}

void Player::updateProjectiles(int deltaTime)
{
    if (bullets.empty()) return;

    const int ts = map->getTileSize();

    auto hitEnemy = [&](const glm::vec2& p, Enemy* e) {
        if (!e) return false;
        if (e->Escena_Original != scene->CurrentMap) return false;
        glm::ivec2 ep = e->posEnemy;
        // Enemy size: scorpion 32x32, soldiers 32x64
		glm::ivec2 enemySize = (e->EnemyType == 0) ? glm::ivec2(32, 32) : (e->EnemyType == 3) ? glm::ivec2(68, 84) : glm::ivec2(32, 64);
        return p.x >= ep.x && p.x <= ep.x + enemySize.x &&
               p.y >= ep.y && p.y <= ep.y + enemySize.y;
    };

    for (auto& b : bullets) {
        if (!b.active) continue;
        b.pos.x += b.dir.x * b.speed * deltaTime;
        b.pos.y += b.dir.y * b.speed * deltaTime;

        // si choca con un tile sòlid se desactiva
        int tx = int(b.pos.x) / ts;
        int ty = int(b.pos.y) / ts;
		if (!map->isTransparentAtTile(tx, ty, scene->CurrentMap)) {
            b.active = false;
            continue;
        }

        // revisar colisión con enemies del mapa
        auto& enemies = scene->getEnemies();
        for (auto* e : enemies) {
            if (!e->mort && hitEnemy(b.pos, e)) {
				e->baixavida(3, false); //El false és perque no és un punyetazo
                b.active = false;
                break;
            }
        }
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());
}

void Player::renderProjectiles()
{
    // Igual que en Enemy: dibujar en espacio de pantalla
    GLint prevProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);

    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_TEXTURE_2D);
    glColor3f(0.9f, 0.9f, 0.9f); // bala gris
    glBegin(GL_QUADS);
    for (const auto& b : bullets) {
        if (!b.active) continue;
        float x = tileMapDispl.x + b.pos.x;
        float y = tileMapDispl.y + b.pos.y;
        float s = 5.0f;
        glVertex2f(x,     y);
        glVertex2f(x + s, y);
        glVertex2f(x + s, y + s);
        glVertex2f(x,     y + s);
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glUseProgram(prevProgram);
}

void Player::GetAllObjects() {
	auto& objs = scene->getObjetos();
	for (auto* o : objs) {
		if (!o) continue;
		if (o->recollit) continue;
		if (o->tipus == 3)hasKey = true;
		else {
			inventari.push_back(o);
		}
		o->recollit = true;
		if (selectedItem < 0) selectedItem = int(inventari.size()) - 1;
	}
	
}

void Player::checkObjectPickup()
{
    if (!scene) return;
    // AABB del jugador: 32x64. Volem tot el cos, del cap als peus
    glm::ivec2 pMin = posPlayer;                 // top-left del cos
    glm::ivec2 pMax = posPlayer + glm::ivec2(32, 64); // bottom-right del cos

    auto& objs = scene->getObjetos();
    for (auto* o : objs) {
        if (!o) continue;
        if (o->recollit) continue;
        if (o->Escena_Original != scene->CurrentMap) continue; // només a la seva escena

        // Usem la posició pública "position" que s'emplena a init()
        glm::ivec2 oMin = o->position;
        glm::ivec2 oMax = o->position + o->getSize();

        bool overlap = !(pMax.x <= oMin.x || pMin.x >= oMax.x || pMax.y <= oMin.y || pMin.y >= oMax.y);
        if (overlap) {
			if (o->tipus == 3) hasKey = true;
			else {
				inventari.push_back(o);
			}
            // afegir a inventari i marcar recollit
			SoundManager::instance().playSound("item");
            o->recollit = true;
            if (selectedItem < 0) selectedItem = int(inventari.size()) - 1;
        }
    }
}

void Player::handlePunchNoWeapon(int feritIdx, int armaIdx)
{
	const int sAnim = sprite->animation();
	if (!(sAnim == Animacions[feritIdx][armaIdx][8] || sAnim == Animacions[feritIdx][armaIdx][9] || sAnim == Animacions[feritIdx][armaIdx][11] || sAnim == Animacions[feritIdx][armaIdx][10])) {
		if (!SoundManager::instance().isSoundPlaying("punch")) {
			SoundManager::instance().playSound("punch");
		}
	}
	glm::ivec2 posPuny = posPlayer;
    if (sAnim == Animacions[feritIdx][armaIdx][0] || sAnim == Animacions[feritIdx][armaIdx][7]) {
        sprite->changeAnimation(Animacions[feritIdx][armaIdx][11]);
		posPuny.y += 48;
    }
    else if (sAnim == Animacions[feritIdx][armaIdx][1] || sAnim == Animacions[feritIdx][armaIdx][6]) {
        sprite->changeAnimation(Animacions[feritIdx][armaIdx][10]);
		posPuny.y += 24;
    }
    else if (sAnim == Animacions[feritIdx][armaIdx][2] || sAnim == Animacions[feritIdx][armaIdx][4]) {
        sprite->changeAnimation(Animacions[feritIdx][armaIdx][8]);
		posPuny.x -= 4;
    }
    else if (sAnim == Animacions[feritIdx][armaIdx][3] || sAnim == Animacions[feritIdx][armaIdx][5]) {
        sprite->changeAnimation(Animacions[feritIdx][armaIdx][9]);
		posPuny.x += 36;
    }
	auto hitEnemy = [&](const glm::vec2& p, Enemy* e) {
		if (!e) return false;
		if (e->Escena_Original != scene->CurrentMap) return false;
		glm::ivec2 ep = e->posEnemy;
		// Enemy size: scorpion 32x32, soldiers 32x64
		glm::ivec2 enemySize = (e->EnemyType == 0) ? glm::ivec2(32, 32) : glm::ivec2(32, 64);
		return p.x >= ep.x && p.x <= ep.x + enemySize.x &&
			p.y >= ep.y && p.y <= ep.y + enemySize.y;
	};
	auto& enemies = scene->getEnemies();
	for (auto* e : enemies) {
		if (!e->mort && hitEnemy(posPuny, e)) {
			e->baixavida(1, true);
			HaPegat = true;
		}
	}
}