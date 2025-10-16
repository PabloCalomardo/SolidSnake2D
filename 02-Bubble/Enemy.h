#pragma once
#include "Sprite.h"
#include "TileMap.h"

// Enemy is basically a Sprite that represents an enemy. As such it has
// all properties it needs to track its movement, and collisions.


class Enemy
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int TipusEnemic);
	void update(int deltaTime, glm::ivec2 posp);
	void render();

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);

    // Devuelve true si este enemigo ve al objetivo desde su FOV (semicírculo) y sin obstáculos.
    // targetPos: posición en píxeles del objetivo (p.ej. `Player::posPlayer`).
    // radius_detection: radio de detección en tiles (por ejemplo 4).
    bool enemic_detectat(const glm::ivec2& targetPos, int radius_detection) const;

private:
	int EnemyType; // 0: DOG, 1: SOLDIER, 2: SOLDIER2
	int vida;
	int moviment_escorpi;
    bool hasEnemyDetected;
	bool mort;
	glm::ivec2 tileMapDispl, posEnemy;
	int jumpAngle, startY;
	int delta_ant;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;

};

