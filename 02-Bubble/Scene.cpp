#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Enemy.h"


#define SCREEN_X 32
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 12
#define INIT_PLAYER_Y_TILES 4


Scene::Scene()
{
	map = NULL;
	player = NULL;
    enemies.clear();
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
    for (Enemy* e : enemies) delete e;
    enemies.clear();
}


void Scene::init()
{
	initShaders();
	CurrentMap = 1;
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();

    // Create enemies and push them into the vector
    // Example enemies across maps:
    {
        // Soldier in map 1
        Enemy* e = new Enemy();
        e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, false, false, true, 1);
        // Set initial position directly to avoid setPosition guard against non-current maps
        e->posEnemy = glm::ivec2(25 * map->getTileSize(), 10 * map->getTileSize());
        e->setTileMap(map);
        enemies.push_back(e);
    }
    // You can add more enemies here for other maps if desired, e.g.:
    {
        Enemy* e = new Enemy();
        e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0, *this, false, false, true, 2);
        e->posEnemy = glm::ivec2(15 * map->getTileSize(), 15 * map->getTileSize());
		e->setTileMap(map);
        enemies.push_back(e);
    }

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
		if (dir == 3) {
			CurrentMap = 5;
			map = TileMap::createTileMap("levels/level05.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 24);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
	}
	else if (CurrentMap == 5) {
		if (dir == 4) {
			CurrentMap = 4;
			map = TileMap::createTileMap("levels/level04.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 4);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
	}
	player->setTileMap(map);
    // Update map on all enemies
    for (Enemy* e : enemies) {
        e->setTileMap(map);
    }
	projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;

	glm::ivec2 posp = player->posPlayer;
    // Update all enemies
    for (Enemy* e : enemies) {
        e->update(deltaTime, posp);
    }
	player->update(deltaTime);
    comprovar_vides(deltaTime);
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
    // Render all enemies
    for (Enemy* e : enemies) {
        e->render();
    }
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


bool animation_not_in(int animation) {
	switch (animation) {
	case 8: case 9: case 10: case 11:
	case 28: case 29: case 30: case 31:
		return false; // está en la lista -> NO permitido
	default:
		return true;  // no está -> permitido
	}
}

void Scene::comprovar_vides(int deltaTime)
{
	//Si l'enemic toca al jugador, aquest perd una vida
	glm::ivec2 posp = player->posPlayer;
    Sprite* spr_player = player->getSprite();

    // Comprovar per cada enemic actiu (CurrentMap == e->Escena_Original)
    for (Enemy* e : enemies) {
        if (!e) continue;
		if (e->mort) continue;
		if (e->Escena_Original != CurrentMap && (e->EnemyType != 0)) continue;
        glm::ivec2 pose = e->posEnemy;
        if ((abs(posp.x - pose.x) < 20 && abs(posp.y - pose.y) < 20)) {
            if (animation_not_in(spr_player->animation())) { //Treiem una vida al jugador
                player->baixavida();
            }
            else { //Treiem una vida a l'enemic
                e->baixavida();
            }
        }
    }
}

