#pragma once
#include "Sprite.h"
#include "TileMap.h"


// Enemy is basically a Sprite that represents an enemy. As such it has
// all properties it needs to track its movement, and collisions.


class Enemy
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int TipusEnemic);
	void update(int deltaTime);
	void render();

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);

private:
	int EnemyType; // 0: DOG, 1: SOLDIER, 2: SOLDIER2
	int vida;
	bool enemic_detectat;
	bool mort;
	glm::ivec2 tileMapDispl, posEnemy;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;

};

