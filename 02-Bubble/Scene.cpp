#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Enemy.h"
#include "InferiorBar.h"
#include "objeto.h"
//#include "Sprite.h"

#define SCREEN_X 32
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 12
#define INIT_PLAYER_Y_TILES 4

#define PIXEL_X 1/808.0f
#define PIXEL_Y 1/546.0f


Scene::Scene()
{
	map = NULL;
	player = NULL;
    enemies.clear();
    // HUD
    hud = nullptr;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
    for (Enemy* e : enemies) delete e;
    enemies.clear();
    if (hud) { delete hud; hud = nullptr; }
}


void Scene::init()
{
	initShaders();
	CurrentMap = 0;
	player = new Player();
	SoundManager::instance().setMusicVolume(60.f);
	SoundManager::instance().playMusic("audio/Main_Theme.ogg", true);
	SoundManager::instance().loadSound("alert", "audio/Alert_Sound.ogg");
	SoundManager::instance().setSoundVolume("alert", 75.f);
	SoundManager::instance().loadSound("shoot", "audio/Shoot.ogg");
	SoundManager::instance().loadSound("game_over", "audio/Game_Over.ogg");
	SoundManager::instance().setSoundVolume("game_over", 50.f);
	SoundManager::instance().loadSound("select", "audio/Select.ogg");
	SoundManager::instance().setSoundVolume("select", 70.f);
	SoundManager::instance().loadSound("item", "audio/Item.ogg");
	SoundManager::instance().setSoundVolume("item", 65.f);
	SoundManager::instance().loadSound("change", "audio/ChangeItem.ogg");
	SoundManager::instance().loadSound("punch", "audio/Punch.ogg");
	SoundManager::instance().setSoundVolume("punch", 55.f);
	SoundManager::instance().loadSound("box", "audio/Box.ogg");
	SoundManager::instance().setSoundVolume("box", 65.f);
	SoundManager::instance().loadSound("heal", "audio/Heal.ogg");
	SoundManager::instance().loadSound("arma", "audio/Select_Gun.ogg");
	SoundManager::instance().loadSound("unequip", "audio/Unequip.ogg");
	SoundManager::instance().setSoundVolume("unequip", 30.f);
	//SoundManager::instance().music.setVolume(40.f);
	map = TileMap::createTileMap("levels/level00.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, *this, false);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);

	ChargeEnemiesAndObjects();
	projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);

	// HUD
	hud = new InferiorBar();
	hud->init(this, texProgram, glm::ivec2(SCREEN_X, SCREEN_Y));

	spritesheet.loadFromFile("images/Pantallas_Inicio.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(PIXEL_X*8, PIXEL_Y*8), glm::vec2(PIXEL_X * 256, PIXEL_Y * 240), &spritesheet, &texProgram);
	sprite->setNumberAnimations(1);

	sprite->setAnimationSpeed(0, 8);
	sprite->addKeyframe(0, glm::vec2(PIXEL_X * (550), PIXEL_Y * (18)));
	sprite->changeAnimation(0);
	sprite->setPosition(glm::ivec2(SCREEN_X, SCREEN_Y));

	sprite->render();

}

void Scene::GoToMainMenu() {
	if (CurrentMap == -1) {
		glClear(GL_COLOR_BUFFER_BIT);
		hud->instructions = false;
	}
	else {
		SoundManager::instance().setMusicVolume(60.f);
		SoundManager::instance().playMusic("audio/Main_Theme.ogg", true);
		map = TileMap::createTileMap("levels/level00.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		Death(1);
		DeleteObjectsAndEnemies();
		ChargeEnemiesAndObjects();
	}
	CurrentMap = 0;
	player->rend = false;

	spritesheet.loadFromFile("images/Pantallas_Inicio.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(PIXEL_X * 8, PIXEL_Y * 8), glm::vec2(PIXEL_X * 256, PIXEL_Y * 240), &spritesheet, &texProgram);
	sprite->setNumberAnimations(1);

	sprite->setAnimationSpeed(0, 8);
	sprite->addKeyframe(0, glm::vec2(PIXEL_X * (550), PIXEL_Y * (18)));
	sprite->changeAnimation(0);
	sprite->setPosition(glm::ivec2(SCREEN_X, SCREEN_Y));

	sprite->render();
}

void Scene::Instructions() {
	CurrentMap = -1;
	hud->instructions = true;
}

void Scene::ChargeEnemiesAndObjects() {
	{
		//==============================
		//			OBJECTES
		//==============================
		// Caixa mapa 1
		objeto* o = new objeto();
		o->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0, *this, 1, glm::ivec2(15 * map->getTileSize(), 15 * map->getTileSize()));
		o->setTileMap(map);
		objetos.push_back(o);
	}
	{
		// Arma mapa 1
		objeto* o = new objeto();
		o->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, 1, glm::ivec2(15 * map->getTileSize(), 20 * map->getTileSize()));
		o->setTileMap(map);
		objetos.push_back(o);
	}
	{
		// Vida mapa 1
		objeto* o = new objeto();
		o->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 2, *this, 1, glm::ivec2(15 * map->getTileSize(), 25 * map->getTileSize()));
		o->setTileMap(map);
		objetos.push_back(o);
	}
	{
		// Vida mapa 1
		objeto* o = new objeto();
		o->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 2, *this, 1, glm::ivec2(15 * map->getTileSize(), 10 * map->getTileSize()));
		o->setTileMap(map);
		objetos.push_back(o);
	}
	{
		// CLAU mapa 1
		objeto* o = new objeto();
		o->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 3, *this, 1, glm::ivec2(10 * map->getTileSize(), 10 * map->getTileSize()));
		o->setTileMap(map);
		objetos.push_back(o);
	}

	//==============================
	//			ENEMICS
	//==============================
	{
		// Soldat mapa 1
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, false, false, true, 1);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(30 * map->getTileSize(), 20 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Escorpí mapa 2
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0, *this, false, false, true, 2);
		e->posEnemy = glm::ivec2(8 * map->getTileSize(), 5 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Soldat mapa 2
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, false, false, true, 2);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(25 * map->getTileSize(), 20 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Soldat2 mapa 2
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, true, false, false, 2);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(22 * map->getTileSize(), 5 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Escorpí1 mapa 3
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0, *this, true, false, false, 3);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(7 * map->getTileSize(), 2 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Escorpí2 mapa 3
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0, *this, true, false, false, 3);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(12 * map->getTileSize(), 8 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Escorpí3 mapa 3
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0, *this, true, false, false, 3);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(20 * map->getTileSize(), 8 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Escorpí4 mapa 3
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0, *this, true, false, false, 3);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(30 * map->getTileSize(), 4 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Escorpí5 mapa 3
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0, *this, true, false, false, 3);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(15 * map->getTileSize(), 2 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Escorpí6 mapa 3
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0, *this, true, false, false, 3);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(25 * map->getTileSize(), 6 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Soldat1 mapa 4
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, true, false, false, 4);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(30 * map->getTileSize(), 17 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Soldat2 mapa 4
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, false, false, true, 4);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(22 * map->getTileSize(), 6 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Soldat1 mapa 5
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, false, true, false, 5);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(7 * map->getTileSize(), 6 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Soldat2 mapa 5
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, true, false, false, 5);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(34 * map->getTileSize(), 13 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Soldat2 mapa 5
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, true, false, false, 6);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(8 * map->getTileSize(), 4 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// Soldat2 mapa 5
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1, *this, true, false, false, 6);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(34 * map->getTileSize(), 13 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// BOSS1 mapa 11
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 3, *this, true, false, false, 11);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(10 * map->getTileSize(), 5 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
	{
		// BOSS2 mapa 11
		Enemy* e = new Enemy();
		e->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 3, *this, true, false, false, 11);
		// Set initial position directly to avoid setPosition guard against non-current maps
		e->posEnemy = glm::ivec2(30 * map->getTileSize(), 5 * map->getTileSize());
		e->setTileMap(map);
		enemies.push_back(e);
	}
}

void Scene::DeleteObjectsAndEnemies() {
	for (objeto* o : objetos) {
		delete o;
	}
	objetos.clear();
	for (Enemy* e : enemies) {
		delete e;
	}
	enemies.clear();
}

void Scene::Death(int op) {
	if (op == 0) hud->mort = true;
	else hud->mort = false;
}

void Scene::tp_to_map(int m)
{
	string aux;
	bool d = false;
	for (Enemy* e : enemies) {
		if (e->Escena_Original == m && e->hasEnemyDetected) d = true;
	}
	if (m < 10) aux = "levels/level0" + to_string(m) + ".txt";
	else aux = "levels/level" + to_string(m) + ".txt";
	map = TileMap::createTileMap(aux, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	if (m == 1) {
		if (!d && CurrentMap < 1 || CurrentMap > 4) {
			SoundManager::instance().setMusicVolume(60.f);
			SoundManager::instance().playMusic("audio/Jungle.ogg", true);
		}
		player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	}
	else if (m == 5) {
		if (!d && CurrentMap < 5 || CurrentMap > 10) {
			SoundManager::instance().setMusicVolume(25.f);
			SoundManager::instance().playMusic("audio/Interior.ogg", true);
		}
		glm::vec2 posaux(23, 24);
		player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1] * map->getTileSize()));
	}
	else if (m == 11) {
		if (!d && CurrentMap != 11) {
			SoundManager::instance().setMusicVolume(25.f);
			SoundManager::instance().playMusic("audio/Final_Boss.ogg", true);
		}
		glm::vec2 posaux(19, 24);
		player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1] * map->getTileSize()));
	}
	CurrentMap = m;
	player->setTileMap(map);
	for (Enemy* e : enemies) {
		e->setTileMap(map);
	}
}


void Scene::ChangeMap(int dir) 
{
	for (Enemy* e : enemies) {
		if (e->Escena_Original == CurrentMap && e->hasEnemyDetected) e->hasEnemyDetected = false;
	}
	glm::vec2 posaux(0, player->posPlayer[1]);
	player->ha_disparat = false;
	if (CurrentMap == 0) {
		SoundManager::instance().setMusicVolume(60.f);
		SoundManager::instance().playMusic("audio/Jungle.ogg", true);
		detectable = true;
		sprite->free();
		CurrentMap = 1;
		map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
		player->rend = true;
		currentTime = 0.0f;
	}
	else if (CurrentMap == 1) {
		printf("entra1\n");
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
		else if (dir == 3) {
			CurrentMap = 5;
			SoundManager::instance().setMusicVolume(25.f);
			SoundManager::instance().playMusic("audio/Interior.ogg", true);
			map = TileMap::createTileMap("levels/level05.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 24);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
	}
	else if (CurrentMap == 5) {
		if (dir == 4) {
			CurrentMap = 4;
			SoundManager::instance().setMusicVolume(60.f);
			SoundManager::instance().playMusic("audio/Jungle.ogg", true);
			map = TileMap::createTileMap("levels/level04.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 4);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
		else if (dir == 3) {
			CurrentMap = 11;
			SoundManager::instance().setMusicVolume(25.f);
			SoundManager::instance().playMusic("audio/Final_Boss.ogg", true);
			map = TileMap::createTileMap("levels/level11.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(19, 24);
			player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1] * map->getTileSize()));
		}
		else if (dir == 2) {
			CurrentMap = 6;
			map = TileMap::createTileMap("levels/level06.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(2, player->posPlayer[1]);
			player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1]));
		}
	}
	else if (CurrentMap == 6) {
		if (dir == 1) {
			CurrentMap = 5;
			map = TileMap::createTileMap("levels/level05.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(36, player->posPlayer[1]);
			player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1]));
		}
		else if (dir == 2) {
			CurrentMap = 7;
			map = TileMap::createTileMap("levels/level07.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(2, player->posPlayer[1]);
			player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1]));
		}
	}
	else if (CurrentMap == 7) {
		if (dir == 1) {
			CurrentMap = 6;
			map = TileMap::createTileMap("levels/level06.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(36, player->posPlayer[1]);
			player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1]));
		}
		else if (dir == 3) {
			CurrentMap = 8;
			map = TileMap::createTileMap("levels/level08.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 26);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
	}
	else if (CurrentMap == 8) {
		if (dir == 4) {
			CurrentMap = 7;
			map = TileMap::createTileMap("levels/level07.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 0);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
		else if (dir == 3) {
			CurrentMap = 9;
			map = TileMap::createTileMap("levels/level09.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 26);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
	}
	else if (CurrentMap == 9) {
		if (dir == 4) {
			CurrentMap = 8;
			map = TileMap::createTileMap("levels/level08.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(player->posPlayer[0], 0);
			player->setPosition(glm::vec2(posaux[0], posaux[1] * map->getTileSize()));
		}
		else if (dir == 1) {
			CurrentMap = 10;
			map = TileMap::createTileMap("levels/level10.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(36, player->posPlayer[1]);
			player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1]));
		}
	}
	else if (CurrentMap == 10) {
		if (dir == 2) {
			CurrentMap = 9;
			map = TileMap::createTileMap("levels/level09.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
			glm::vec2 posaux(2, player->posPlayer[1]);
			player->setPosition(glm::vec2(posaux[0] * map->getTileSize(), posaux[1]));
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
	bool d = false;
	glm::ivec2 posp = player->posPlayer;
    // Update all enemies
	bool player_ha_disparat = player->ha_disparat;
    for (Enemy* e : enemies) {
		e->update(deltaTime, posp, player_ha_disparat,player->cajaActive);
		if (!detectat && e->Escena_Original == CurrentMap && e->hasEnemyDetected) {
			detectat = true;
			d = true;
			SoundManager::instance().playSound("alert");
			SoundManager::instance().setMusicVolume(25.f);
			SoundManager::instance().playMusic("audio/Encounter.ogg", true);
		}
		else if (!d && e->Escena_Original == CurrentMap && e->hasEnemyDetected) {
			d = true;
		}
    }
	if (!d && detectat) {
		detectat = false;
		if (CurrentMap >= 1 && CurrentMap <=4) {
			SoundManager::instance().setMusicVolume(60.f);
			SoundManager::instance().playMusic("audio/Jungle.ogg", true);
		}
		else if (CurrentMap >=5 && CurrentMap <=10) {
			SoundManager::instance().setMusicVolume(25.f);
			SoundManager::instance().playMusic("audio/Interior.ogg", true);
		}
	}
	for (objeto* o : objetos) {
		o->update(deltaTime);
	}
	player->update(deltaTime);
    comprovar_vides(deltaTime);
    if (hud) hud->update(deltaTime);
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
	for (objeto* o : objetos) {
		o->render();
	}
	player->render();

    // Render HUD last so it overlays
    if (hud) hud->render();
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

	if (damageCD > 0) {
		damageCD -= deltaTime;
		return;
	}

	glm::ivec2 posp = player->posPlayer;
    Sprite* spr_player = player->getSprite();

    // Comprovar per cada enemic actiu (CurrentMap == e->Escena_Original)
    for (Enemy* e : enemies) {
        if (!e) continue;
		if (e->mort || e->noUpdate) continue;
		if (e->Escena_Original != CurrentMap || (e->EnemyType != 0)) continue;
        glm::ivec2 pose = e->posEnemy;
        if ((abs(posp.x - pose.x) < 20 && abs(posp.y - pose.y) < 20)) {
            if (animation_not_in(spr_player->animation())) { //Treiem una vida al jugador
                player->baixavida(2);
				damageCD = 500;
            }
        }
    }
}

