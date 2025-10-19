#pragma once
#include "Sprite.h"
#include "TileMap.h"
#include "Scene.h"

// Enemy is basically a Sprite that represents an enemy. As such it has
// all properties it needs to track its movement, and collisions.

class Scene;

class Enemy
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int TipusEnemic, Scene &sc);
	void update(int deltaTime, glm::ivec2 posp);
	void render();

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);

    // Devuelve true si este enemigo ve al objetivo desde su FOV (semicírculo) y sin obstáculos.
    // targetPos: posición en píxeles del objetivo (p.ej. `Player::posPlayer`).
    // radius_detection: radio de detección en tiles (por ejemplo 4).
    bool enemic_detectat(const glm::ivec2& targetPos, int radius_detection) const;

    // Mou l'enemic cap a una posició objectiu sense trepitjar tiles no permesos (una passa per crida).
    // Només avança per tiles que estan dins del set Col (walkables) del TileMap (via funcions de col·lisió).
    // targetPos: posició en píxels dins del mateix espai que `posEnemy`.
    // speed: multiplica la longitud de la passa (base 3 píxels) -> pas = 3 * speed.
    void goToPosition(int deltaTime, const glm::ivec2& targetPos, int speed = 1);
	glm::ivec2 posEnemy;
	void baixavida();

private:
	int EnemyType; // 0: DOG, 1: SOLDIER, 2: SOLDIER2
	int vida;
	int moviment_escorpi;
    bool hasEnemyDetected;
	bool mort;
	glm::ivec2 tileMapDispl;
	int jumpAngle, startY;
	int delta_ant;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;
	Scene* scene;
};

