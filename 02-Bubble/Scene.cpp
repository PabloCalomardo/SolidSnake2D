#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 32
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 12
#define INIT_PLAYER_Y_TILES 4


Scene::Scene()
{
	map = NULL;
	player = NULL;
	enemy = NULL;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
}


void Scene::init()
{
	initShaders();
	CurrentMap = 1;
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	enemy = new Enemy();
	enemy->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0);
	enemy->setPosition(glm::vec2(27 * map->getTileSize(), 22 * map->getTileSize()));
	enemy->setTileMap(map);

	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram,*this);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
	currentTime = 0.0f;
}

void Scene::ChangeMap(int dir) 
{
	glm::vec2 posaux(0, player->posPlayer[1]);
	if (CurrentMap == 1) {
		CurrentMap = 2;
		map = TileMap::createTileMap("levels/level02.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		glm::vec2 posaux(0, player->posPlayer[1]);
		player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1]));
	}
	else if (CurrentMap == 2) {
		if (dir == 1) {
			CurrentMap = 1;
			map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(38, player->posPlayer[1]);
			player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1]));
		}
		else if (dir == 3) {
			CurrentMap = 3;
			map = TileMap::createTileMap("levels/level03.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 26);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
	}
	else if (CurrentMap == 3) {
		if (dir == 4) {
			CurrentMap = 2;
			map = TileMap::createTileMap("levels/level02.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 0);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
		else if (dir == 3) {
			CurrentMap = 4;
			map = TileMap::createTileMap("levels/level04.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 26);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
	}
	else if (CurrentMap == 4) {
		if (dir == 4) {
			CurrentMap = 3;
			map = TileMap::createTileMap("levels/level03.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 0);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
	}
	player->setTileMap(map);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;

	glm::ivec2 posp = player->posPlayer;

	enemy->update(deltaTime, posp);
	player->update(deltaTime);
}

void Scene::render()
{
	glm::mat4 modelview;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	map->render();

	enemy->render();
	player->render();
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}



