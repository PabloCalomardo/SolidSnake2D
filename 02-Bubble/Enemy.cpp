#include "Enemy.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <GL/glew.h>
#include "Enemy.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define PIXEL_X 1/255.0f
#define PIXEL_Y 1/211.0f
#define FALL_STEP 4


enum EnemyAnims
{ 
	STAND_NORMAL, STAND_UP, STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, MOVE_DOWN2, MOVE_UP2
};

/* DADES:
* Scorpion COMENÇA AL PIXEL 93,102
* SOLDIER COMENÇA AL PIXEL 2,56
* SOLDIER2 COMENÇA AL PIXEL 2,142

*/



void Enemy::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int TipusEnemic)
{
    vida = 2;
    hasEnemyDetected = false;
	EnemyType = TipusEnemic;
	moviment_escorpi = 0;
	mort = false;
	spritesheet.loadFromFile("images/Enemies.png", TEXTURE_PIXEL_FORMAT_RGBA);	//Enemies ES: 255x211

	if (EnemyType == 0)sprite = Sprite::createSprite(glm::ivec2(16 * 2, 16 * 2), glm::vec2(PIXEL_X * 16, PIXEL_Y * 16), &spritesheet, &shaderProgram);
	else sprite = Sprite::createSprite(glm::ivec2(16 * 2, 32 * 2), glm::vec2(PIXEL_X * 16, PIXEL_Y * 32), &spritesheet, &shaderProgram);

	
	sprite->setNumberAnimations(10); //all
	

	//==============================
	//			escorpi
	//==============================
		//==============================
		//			STANDS 
		//==============================
	if (EnemyType == 0)
	{

		sprite->setAnimationSpeed(MOVE_DOWN, 1);
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (93), PIXEL_Y * 102)); //DEFINITIU, 2 pixel a la dreta i 26 cap a 
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (93), PIXEL_Y * 102)); //DEFINITIU, 2 pixel a la dreta i 26 cap a 

		sprite->setAnimationSpeed(MOVE_DOWN2, 1);
		sprite->addKeyframe(MOVE_DOWN2, glm::vec2(PIXEL_X * 66, PIXEL_Y * 26)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix

		sprite->setAnimationSpeed(MOVE_UP, 1);	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * 131, PIXEL_Y * 102));
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * 131, PIXEL_Y * 102));

		sprite->setAnimationSpeed(MOVE_UP2, 1);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(MOVE_UP2, glm::vec2(PIXEL_X * 104, PIXEL_Y * 61));
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

	if (EnemyType == 0)sprite->changeAnimation(MOVE_DOWN);
	else sprite->changeAnimation(STAND_NORMAL);

	tileMapDispl = tileMapPos;
	delta_ant = 0;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));

}

void Enemy::update(int deltaTime, glm::ivec2 posp)
{
	if (vida == 0) mort = true;
	sprite->update(deltaTime);
    // Recalcular detecció cada frame per iniciar la persecució quan entri al FOV
    if (mort) {
        posEnemy.x = 0;
		posEnemy.y = 0;
    }
    else {
        if (!hasEnemyDetected) {
            hasEnemyDetected = enemic_detectat(posp, 8);
        }
        //contador per al moviment en cercle de l'escorpi
        delta_ant += deltaTime;

        if (EnemyType == 0) //GOS
        {
            if (hasEnemyDetected) {
                cout << "Enemy has detected the player!" << endl;
                if ((EnemyType == 0 || EnemyType == 1) && delta_ant > 500) { //MELEE (ens apropem i l'ataquem)
                    goToPosition(deltaTime, posp, 6);
                    delta_ant = 0;
                }
                else { //RANGED (ens mantenim a distancia i l'ataquem)

                }
            }
            else { //Tranquilitat (moviment en cercle)
                if (sprite->animation() == MOVE_DOWN && moviment_escorpi == 0 && delta_ant > 500) {
                    posEnemy.y += 15;
                    posEnemy.x -= 15;
                    moviment_escorpi++;
                    delta_ant = 0;
                    sprite->changeAnimation(MOVE_DOWN);
                }
                else if (sprite->animation() == MOVE_DOWN && moviment_escorpi == 1 && delta_ant > 500) {
                    posEnemy.y += 15;
                    posEnemy.x += 15;
                    moviment_escorpi++;
                    delta_ant = 0;
                    sprite->changeAnimation(MOVE_DOWN);
                }
                else if (sprite->animation() == MOVE_DOWN && moviment_escorpi == 2 && delta_ant > 500) {
                    delta_ant = 0;
                    sprite->changeAnimation(MOVE_UP);
                }
                else if (sprite->animation() == MOVE_UP && moviment_escorpi == 2 && delta_ant > 500) {
                    posEnemy.y -= 15;
                    posEnemy.x += 15;
                    moviment_escorpi++;
                    delta_ant = 0;
                    sprite->changeAnimation(MOVE_UP);
                }
                else if (sprite->animation() == MOVE_UP && moviment_escorpi == 3 && delta_ant > 500) {
                    posEnemy.y -= 15;
                    posEnemy.x -= 15;
                    moviment_escorpi++;
                    delta_ant = 0;
                    sprite->changeAnimation(MOVE_UP);
                }
                else if (sprite->animation() == MOVE_UP && moviment_escorpi == 0 && delta_ant > 500) {
                    delta_ant = 0;
                    delta_ant = 0;
                    sprite->changeAnimation(MOVE_DOWN);
                }

                if (moviment_escorpi == 4) moviment_escorpi = 0;
            }
        }
        else { //SOLDIER
            if (hasEnemyDetected) {
            }
            else {
                // de moment, cap patró especial
            }
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

// Helper to get facing direction unit vector from current animation
static glm::ivec2 EnemyFacingFromAnim(int anim)
{
    switch (anim) {
    case STAND_LEFT:
    case MOVE_LEFT: return glm::ivec2(-1, 0);
    case STAND_RIGHT:
    case MOVE_RIGHT: return glm::ivec2(1, 0);
    case STAND_UP:
    case MOVE_UP:
    case MOVE_UP2: return glm::ivec2(0, -1);
    case STAND_NORMAL:
    case MOVE_DOWN:
    case MOVE_DOWN2:
    default: return glm::ivec2(0, 1);
    }
}

bool Enemy::enemic_detectat(const glm::ivec2& targetPos, int radius_detection) const
{
    if (!map) return false;

    // Tile size in pixels
    const int ts = map->getTileSize();

    // Enemy center position in pixels
    glm::ivec2 enemyPosPx = posEnemy;
    glm::ivec2 enemyCenter = enemyPosPx + glm::ivec2(ts / 2, ts / 2);
    glm::ivec2 targetCenter = targetPos + glm::ivec2(ts / 2, ts / 2);

    // Distance check: radius 4 tiles
    int dx = targetCenter.x - enemyCenter.x;
    int dy = targetCenter.y - enemyCenter.y;
    int rPx = std::max(0, radius_detection) * ts;
    if (dx * dx + dy * dy > rPx * rPx)
        return false;

    // Facing semicircle check using dot product with facing vector
    glm::ivec2 f = EnemyFacingFromAnim(sprite->animation());
    glm::vec2 fv(f.x, f.y);
    glm::vec2 toTarget(dx, dy);
    float len = glm::length(toTarget);
    if (len == 0.f) {
        return true; // same position
    }
    glm::vec2 dir = toTarget / len;
    float facingDot = fv.x * dir.x + fv.y * dir.y; // cos(theta)
    if (facingDot < 0.0f) // more than 90 degrees away => behind
        return false;

    // Line-of-sight ray cast through tiles. Only tiles in Col (non-collidable) are transparent.
    // Cast from enemyCenter to targetCenter sampling along the line.
    int steps = std::max(std::abs(dx), std::abs(dy));
    steps = std::max(1, steps / (ts / 2)); // sample roughly every half tile
    for (int i = 1; i <= steps; ++i) {
        float t = float(i) / float(steps);
        int sx = int(std::round(enemyCenter.x + t * dx));
        int sy = int(std::round(enemyCenter.y + t * dy));

        int tx = sx / ts;
        int ty = sy / ts;
        if (!map->isTransparentAtTile(tx, ty)) {
            // If this opaque tile is the one containing the target center, allow detection
            int targetTx = targetCenter.x / ts;
            int targetTy = targetCenter.y / ts;
            if (tx == targetTx && ty == targetTy)
                break;
            return false;
        }
    }
    return true;
}

// Mou una sola passa cap a targetPos. Respecta col·lisions i anima segons tipus.
void Enemy::goToPosition(int /*deltaTime*/, const glm::ivec2& targetPos, int speed)
{
    if (!map) return;

    // mida hitbox coherent amb Player
    const glm::ivec2 bbox(32, 32);
    const int s = std::max(1, speed);
    const int step = 3 * s;

    auto sgn = [](int v) -> int { return (v > 0) - (v < 0); };

    int dx = targetPos.x - posEnemy.x;
    int dy = targetPos.y - posEnemy.y;

    if (EnemyType == 0) {
        // Moviment diagonal (+/- step en X i Y)
        int dirX = sgn(dx);
        int dirY = sgn(dy);

        bool movedX = false, movedY = false;

        if (dirX != 0 && dirY != 0) {
            // Intenta diagonal com a dues passes atòmiques X+Y
            // Primer X
            posEnemy.x += dirX * step;
            if ((dirX < 0 && map->collisionMoveLeft(posEnemy, bbox)) ||
                (dirX > 0 && map->collisionMoveRight(posEnemy, bbox)))
            {
                posEnemy.x -= dirX * step; // revertir
            } else {
                movedX = true;
            }

            // Després Y
            posEnemy.y += dirY * step;
            if ((dirY < 0 && map->collisionMoveUP(posEnemy, bbox)) ||
                (dirY > 0 && map->collisionMoveDown(posEnemy, bbox)))
            {
                posEnemy.y -= dirY * step; // revertir
                // si X havia mogut però Y no, segueix vàlid (no diagonal estricta però avança)
            } else {
                movedY = true;
            }

            // Animació segons diagonal prevista
            if (dirX > 0 && dirY < 0) {
                // dalt-dreta -> MOVE_DOWN
                if (sprite->animation() != MOVE_DOWN) sprite->changeAnimation(MOVE_DOWN);
            } else if (dirX < 0 && dirY > 0) {
                // baix-esquerra -> MOVE_DOWN
                if (sprite->animation() != MOVE_DOWN) sprite->changeAnimation(MOVE_DOWN);
            } else {
                // dalt-esquerra o baix-dreta -> MOVE_UP
                if (sprite->animation() != MOVE_UP) sprite->changeAnimation(MOVE_UP);
            }

            // Si cap component ha mogut (x ni y), intenta un pla B: primer l'eix més llunyà
            if (!movedX && !movedY) {
                if (std::abs(dx) >= std::abs(dy)) {
                    // prova X
                    posEnemy.x += dirX * step;
                    if ((dirX < 0 && map->collisionMoveLeft(posEnemy, bbox)) ||
                        (dirX > 0 && map->collisionMoveRight(posEnemy, bbox)))
                    {
                        posEnemy.x -= dirX * step;
                        // prova Y
                        posEnemy.y += dirY * step;
                        if ((dirY < 0 && map->collisionMoveUP(posEnemy, bbox)) ||
                            (dirY > 0 && map->collisionMoveDown(posEnemy, bbox)))
                        {
                            posEnemy.y -= dirY * step;
                        }
                    }
                } else {
                    // prova Y
                    posEnemy.y += dirY * step;
                    if ((dirY < 0 && map->collisionMoveUP(posEnemy, bbox)) ||
                        (dirY > 0 && map->collisionMoveDown(posEnemy, bbox)))
                    {
                        posEnemy.y -= dirY * step;
                        // prova X
                        posEnemy.x += dirX * step;
                        if ((dirX < 0 && map->collisionMoveLeft(posEnemy, bbox)) ||
                            (dirX > 0 && map->collisionMoveRight(posEnemy, bbox)))
                        {
                            posEnemy.x -= dirX * step;
                        }
                    }
                }
            }
        } else {
            // Si ja estem alineats amb alguna coordenada, mou només per l'altra
            if (dirX != 0) {
                posEnemy.x += dirX * step;
                if ((dirX < 0 && map->collisionMoveLeft(posEnemy, bbox)) ||
                    (dirX > 0 && map->collisionMoveRight(posEnemy, bbox)))
                {
                    posEnemy.x -= dirX * step;
                }
            }
            if (dirY != 0) {
                posEnemy.y += dirY * step;
                if ((dirY < 0 && map->collisionMoveUP(posEnemy, bbox)) ||
                    (dirY > 0 && map->collisionMoveDown(posEnemy, bbox)))
                {
                    posEnemy.y -= dirY * step;
                }
            }

            // Escull animació segons la intenció vertical/horizontal
            if (dirX > 0 && dirY <= 0) {
                if (sprite->animation() != MOVE_DOWN) sprite->changeAnimation(MOVE_DOWN);
            } else if (dirX < 0 && dirY >= 0) {
                if (sprite->animation() != MOVE_DOWN) sprite->changeAnimation(MOVE_DOWN);
            } else {
                if (sprite->animation() != MOVE_UP) sprite->changeAnimation(MOVE_UP);
            }
        }
    }
    else {
        // Enemics no 0 es mouen en cardinal amb 3 unitats
        if (std::abs(dx) >= std::abs(dy)) {
            int dirX = sgn(dx);
            if (dirX != 0) {
                posEnemy.x += dirX * step;
                bool collide = (dirX < 0 && map->collisionMoveLeft(posEnemy, bbox)) ||
                               (dirX > 0 && map->collisionMoveRight(posEnemy, bbox));
                if (collide) {
                    posEnemy.x -= dirX * step;
                    int dirY = sgn(dy);
                    if (dirY != 0) {
                        posEnemy.y += dirY * step;
                        bool colY = (dirY < 0 && map->collisionMoveUP(posEnemy, bbox)) ||
                                    (dirY > 0 && map->collisionMoveDown(posEnemy, bbox));
                        if (colY) posEnemy.y -= dirY * step;
                        if (dirY < 0) {
                            if (sprite->animation() != MOVE_UP) sprite->changeAnimation(MOVE_UP);
                        } else if (dirY > 0) {
                            if (sprite->animation() != MOVE_DOWN) sprite->changeAnimation(MOVE_DOWN);
                        }
                    }
                } else {
                    if (dirX < 0) {
                        if (sprite->animation() != MOVE_LEFT) sprite->changeAnimation(MOVE_LEFT);
                    } else if (dirX > 0) {
                        if (sprite->animation() != MOVE_RIGHT) sprite->changeAnimation(MOVE_RIGHT);
                    }
                }
            }
        } else {
            int dirY = sgn(dy);
            if (dirY != 0) {
                posEnemy.y += dirY * step;
                bool collide = (dirY < 0 && map->collisionMoveUP(posEnemy, bbox)) ||
                               (dirY > 0 && map->collisionMoveDown(posEnemy, bbox));
                if (collide) {
                    posEnemy.y -= dirY * step;
                    int dirX = sgn(dx);
                    if (dirX != 0) {
                        posEnemy.x += dirX * step;
                        bool colX = (dirX < 0 && map->collisionMoveLeft(posEnemy, bbox)) ||
                                    (dirX > 0 && map->collisionMoveRight(posEnemy, bbox));
                        if (colX) posEnemy.x -= dirX * step;
                        if (dirX < 0) {
                            if (sprite->animation() != MOVE_LEFT) sprite->changeAnimation(MOVE_LEFT);
                        } else if (dirX > 0) {
                            if (sprite->animation() != MOVE_RIGHT) sprite->changeAnimation(MOVE_RIGHT);
                        }
                    }
                } else {
                    if (dirY < 0) {
                        if (sprite->animation() != MOVE_UP) sprite->changeAnimation(MOVE_UP);
                    } else if (dirY > 0) {
                        if (sprite->animation() != MOVE_DOWN) sprite->changeAnimation(MOVE_DOWN);
                    }
                }
            }
        }
    }
}

void Enemy::baixavida() {
	vida = vida - 1;
}