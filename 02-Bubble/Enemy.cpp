#include "Enemy.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <GL/glew.h>
#include <GL/gl.h>
#include "Enemy.h"
#include "Game.h"
#include <random>

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



void Enemy::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int TipusEnemic, Scene &sc, bool mh, bool q, bool mv, int OriginalScene)
{
	Escena_Original = OriginalScene;
	moviment_horitzontal = mh;
	quiet = q;
	moviment_vertical = mv;

    scene = &sc;
    posPlayer = glm::ivec2(0);
    hasEnemyDetected = false;
	EnemyType = TipusEnemic;
    if (EnemyType == 0) vida = 2;
    if (EnemyType == 3) vida = 20;
    else vida = 4;
	moviment_escorpi = 0;
	mort = false;
    if(EnemyType == 3) spritesheet.loadFromFile("images/Bosses.png", TEXTURE_PIXEL_FORMAT_RGBA);	//Enemies ES: 255x211
	else spritesheet.loadFromFile("images/Enemies.png", TEXTURE_PIXEL_FORMAT_RGBA);	//Enemies ES: 255x211
    spritesheet2.loadFromFile("images/objects.png", TEXTURE_PIXEL_FORMAT_RGBA);//EXCLAMACIÓ

    exclamacio = Sprite::createSprite(glm::ivec2(16 * 2, 16 * 2), glm::vec2((1/342.f) * 16, (1/204.f) * 16), &spritesheet2, &shaderProgram);
	exclamacio->setNumberAnimations(1);
    exclamacio->setAnimationSpeed(0, 1);
    exclamacio->addKeyframe(0, glm::vec2((121.f / 342.f) , (144.f / 204.f))); //DEFINITIU, 2 pixel a la dreta i 26 cap a 
    exclamacio->changeAnimation(0);

	if (EnemyType == 0)sprite = Sprite::createSprite(glm::ivec2(16 * 2, 16 * 2), glm::vec2(PIXEL_X * 16, PIXEL_Y * 16), &spritesheet, &shaderProgram);
    else if (EnemyType == 3)sprite = Sprite::createSprite(glm::ivec2(34 * 2, 42 * 2), glm::vec2((1/381.f) * 34, (1 / 154.f) * 42), &spritesheet, &shaderProgram);
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
		sprite->setAnimationSpeed(STAND_NORMAL, 4);
        sprite->addKeyframe(STAND_NORMAL, glm::vec2(PIXEL_X * (2), PIXEL_Y * 56)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix
		sprite->setAnimationSpeed(STAND_UP, 4);
		sprite->addKeyframe(STAND_UP, glm::vec2(PIXEL_X * (21), PIXEL_Y * 56)); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix
		sprite->setAnimationSpeed(STAND_LEFT, 4);	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
		sprite->addKeyframe(STAND_LEFT, glm::vec2(PIXEL_X * (2 ), PIXEL_Y * 56));
		sprite->setAnimationSpeed(STAND_RIGHT, 4);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(PIXEL_X * (21), PIXEL_Y * 56));
		//==============================
		//			MOVES
		//==============================
		sprite->setAnimationSpeed(MOVE_LEFT, 4); // Inicialment STAND LEFT, despres les animacions de caminar
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * 2, PIXEL_Y * 91)); //Stand left
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * (40), PIXEL_Y * 91));

		sprite->setAnimationSpeed(MOVE_RIGHT, 4);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * 21, PIXEL_Y * 91)); //Stand right
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2((PIXEL_X * 59), PIXEL_Y * 91));

		sprite->setAnimationSpeed(MOVE_UP, 4);
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * 21, PIXEL_Y * 56)); //Stand up
		sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * (59), PIXEL_Y * 56));

		sprite->setAnimationSpeed(MOVE_DOWN, 4);
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * 2, PIXEL_Y * 56)); //Stand normal
		sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (40), PIXEL_Y * 56));

	}
	else if (EnemyType == 2) {
        //==============================
        //			STANDS 
        //==============================
        sprite->setAnimationSpeed(STAND_NORMAL, 4);
        sprite->addKeyframe(STAND_NORMAL, glm::vec2(PIXEL_X * (2), PIXEL_Y * (56+ 86))); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix
        sprite->setAnimationSpeed(STAND_UP, 4);
        sprite->addKeyframe(STAND_UP, glm::vec2(PIXEL_X * (21), PIXEL_Y * (56+ 86))); //DEFINITIU, (1+18+2) pixel a la dreta i 25 cap a baix
        sprite->setAnimationSpeed(STAND_LEFT, 4);	//La segona fila esta a (25 pixels de marge + 35 pixels d'alçada d'imatge +1 de contorn) cap a baix
        sprite->addKeyframe(STAND_LEFT, glm::vec2(PIXEL_X * (2), PIXEL_Y * (56+ 86)));
        sprite->setAnimationSpeed(STAND_RIGHT, 4);	//La segona fila esta a (25 pixels de marge + 34 pixels d'alçada d'imatge+1de contorn) cap a baix i (1 + 18 +1 de contorn) pixels a la dreta
        sprite->addKeyframe(STAND_RIGHT, glm::vec2(PIXEL_X * (21), PIXEL_Y * (56+ 86)));
        //==============================
        //			MOVES
        //==============================
        sprite->setAnimationSpeed(MOVE_LEFT, 4); // Inicialment STAND LEFT, despres les animacions de caminar
        sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * 2, PIXEL_Y * (91 + 86))); //Stand left
        sprite->addKeyframe(MOVE_LEFT, glm::vec2(PIXEL_X * (40), PIXEL_Y * (91 + 86)));

        sprite->setAnimationSpeed(MOVE_RIGHT, 4);
        sprite->addKeyframe(MOVE_RIGHT, glm::vec2(PIXEL_X * 21, PIXEL_Y * (91 + 86))); //Stand right
        sprite->addKeyframe(MOVE_RIGHT, glm::vec2((PIXEL_X * 59), PIXEL_Y * (91 + 86)));

        sprite->setAnimationSpeed(MOVE_UP, 4);
        sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * 21, PIXEL_Y * (56+ 86))); //Stand up
        sprite->addKeyframe(MOVE_UP, glm::vec2(PIXEL_X * (59), PIXEL_Y * (56+ 86)));

        sprite->setAnimationSpeed(MOVE_DOWN, 4);
        sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * 2, PIXEL_Y * (56+ 86))); //Stand normal
        sprite->addKeyframe(MOVE_DOWN, glm::vec2(PIXEL_X * (40), PIXEL_Y * (56+ 86)));
	}
	else {//BOSS  34 amplada, 42 alçada
        sprite->setAnimationSpeed(STAND_NORMAL, 4);
        sprite->addKeyframe(STAND_NORMAL, glm::vec2((1 / 381.f) * (311), (1 / 154.f) * 9)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix
        sprite->addKeyframe(STAND_NORMAL, glm::vec2((1 / 381.f) * (311 + 35), (1 / 154.f) * 9)); //DEFINITIU, 2 pixel a la dreta i 26 cap a baix
    }

	if (EnemyType == 0)sprite->changeAnimation(MOVE_DOWN);
	else sprite->changeAnimation(STAND_NORMAL);

	tileMapDispl = tileMapPos;
	delta_ant = 0;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));

}

void Enemy::update(int deltaTime, glm::ivec2 posp, bool player_ha_disparat, bool caixa)
{
    antic_hasEnemyDetected = hasEnemyDetected;
	if (scene->CurrentMap != Escena_Original) return;
    if (noUpdate) return;
	if (!mort && player_ha_disparat) hasEnemyDetected = true;
    if (mort) {
        sprite->setPosition(glm::vec2(float(-100), float(-100)));
        return;
    }
    sprite->update(deltaTime);
    shootTimer += deltaTime;
    // Recalcular detecció cada frame per iniciar la persecució quan entri al FOV
    if (mort) {
        posEnemy.x = 0;
        posEnemy.y = 0;
    }
    else {
        if (!scene->detectable) hasEnemyDetected = false;
		else if (!hasEnemyDetected && posPlayer == posp && caixa) hasEnemyDetected = false;
        else if (!hasEnemyDetected){
            hasEnemyDetected = enemic_detectat(posp, 20);
        }
        posPlayer = posp;
        //contador per al moviment en cercle de l'escorpi
        delta_ant += deltaTime;

        if (EnemyType == 0) //Escorpí
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
        else if (EnemyType == 3) //BOSS
        {
            hasEnemyDetected = false;
            //DIsparem aleatoriament a posp (al voltant) i ens movem una mica també aleatoriament
            if(shootTimer >= 500)
            {
                int aleatorietat = rand() % 200 + 1; // valor entre 0 i 5
				aleatorietat = 100 - aleatorietat; // valor entre -100 i 100
                glm::ivec2 pos_to_shoot = glm::ivec2(posp.x + aleatorietat, posp.y);
                BOSStryShootAt(pos_to_shoot);
                shootTimer = 0;
                if (aleatorietat < -25 && !(map->collisionMoveLeft(posEnemy, glm::ivec2(64, 84), scene->CurrentMap))) posEnemy.x -= 10;
                else if(aleatorietat > 25 && !(map->collisionMoveRight(posEnemy, glm::ivec2(64, 84), scene->CurrentMap))) posEnemy.x += 10;
            }
        }
        else if (EnemyType == 2) { //JetpackSoldier
            if (hasEnemyDetected) {
                if (delta_ant > 45) {
                    goToPosition(deltaTime, posp, 2);
                    delta_ant = 0;
                }
            }
            else { // Tranquilitat, patrullem (pot ser en vertical o horitzontal segons el moviment que vulguem)
                if (moviment_horitzontal) {
                    if ((sprite->animation() == STAND_NORMAL) || (sprite->animation() == MOVE_RIGHT) && delta_ant > 1000) {
                        posEnemy.x -= 10;
                        delta_ant = 0;
                        sprite->changeAnimation(MOVE_LEFT);
                    }
                    else if (sprite->animation() == MOVE_LEFT && delta_ant > 1000) {
                        posEnemy.x += 10;
                        delta_ant = 0;
                        sprite->changeAnimation(MOVE_RIGHT);
                    }
                }
                else if (moviment_vertical) {
                    if ((sprite->animation() == STAND_NORMAL) || (sprite->animation() == MOVE_DOWN) && delta_ant > 1000) {
                        posEnemy.y -= 10;
                        delta_ant = 0;
                        sprite->changeAnimation(MOVE_UP);
                    }
                    else if (sprite->animation() == MOVE_UP && delta_ant > 1000) {
                        posEnemy.y += 10;
                        delta_ant = 0;
                        sprite->changeAnimation(MOVE_DOWN);
                    }
                }
            }

        }
        else { //SOLDIER
            if (hasEnemyDetected) {
                // 1) Perseguir fins tenir-lo a tiro
                glm::ivec2 shootDir(0, 0);
                bool inLine = hasClearAxisShot(posp, shootDir);
                if (!inLine) {
                    // Perseguir cap al player (moviment cardinal)
                    if (delta_ant > 200) {
                        goToPosition(deltaTime, posp, 2);
                        delta_ant = 0;
                    }
                } else {
                    // Orientar animació segons direcció
                    if (shootDir.x < 0) {
                        if (sprite->animation() != MOVE_LEFT) sprite->changeAnimation(MOVE_LEFT);
                    } else if (shootDir.x > 0) {
                        if (sprite->animation() != MOVE_RIGHT) sprite->changeAnimation(MOVE_RIGHT);
                    } else if (shootDir.y < 0) {
                        if (sprite->animation() != MOVE_UP) sprite->changeAnimation(MOVE_UP);
                    } else if (shootDir.y > 0) {
                        if (sprite->animation() != MOVE_DOWN) sprite->changeAnimation(MOVE_DOWN);
                    }

                    // 2) Disparar si cooldown permès
                    if (shootTimer >= shootCooldownMs) {
                        tryShootAt(posp);
                        shootTimer = 0;
                    }
                }
            } else { // Tranquilitat, patrullem (pot ser en vertical o horitzontal segons el moviment que vulguem)
                if (moviment_horitzontal) {
                    if ((sprite->animation() == STAND_NORMAL) || (sprite->animation() == MOVE_RIGHT) && delta_ant > 1000) {
                        posEnemy.x -= 10;
                        delta_ant = 0;
                        sprite->changeAnimation(MOVE_LEFT);
                    }
                    else if (sprite->animation() == MOVE_LEFT && delta_ant > 1000) {
                        posEnemy.x += 10;
                        delta_ant = 0;
                        sprite->changeAnimation(MOVE_RIGHT);
                    }
                }
                else if (moviment_vertical) {
                    if ((sprite->animation() == STAND_NORMAL) || (sprite->animation() == MOVE_DOWN) && delta_ant > 1000) {
                        posEnemy.y -= 10;
                        delta_ant = 0;
                        sprite->changeAnimation(MOVE_UP);
                    }
                    else if (sprite->animation() == MOVE_UP && delta_ant > 1000) {
                        posEnemy.y += 10;
                        delta_ant = 0;
                        sprite->changeAnimation(MOVE_DOWN);
                    }

                }
            }
        }
        sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
    }

    // Update projectiles irrespective of detection
    updateProjectiles(deltaTime);
}

void Enemy::render()
{
    if (antic_hasEnemyDetected != hasEnemyDetected && hasEnemyDetected) {
		frames_render = 75; // 2 segons i mitg a 30 fps
        cout << "render exclamacio" << endl;
    }
    if (frames_render > 0) {
        exclamacio->setPosition(posEnemy + glm::ivec2(50, -20));
        frames_render -= 1;
        if(scene->CurrentMap == Escena_Original) exclamacio->render();
    }

    

    if (scene->CurrentMap != Escena_Original) return;
    if (mort) return;
	sprite->render();
    renderProjectiles();
}

void Enemy::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Enemy::setPosition(const glm::vec2& pos)
{
    if (scene->CurrentMap != Escena_Original) return;
    if (mort) return;
	posEnemy = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

bool Enemy::hasClearAxisShot(const glm::ivec2& targetPos, glm::ivec2& outDir) const
{
    if (!map) return false;
    const int ts = map->getTileSize();
    // Same row
    if (targetPos.y / ts == posEnemy.y / ts) {
        int dir = (targetPos.x > posEnemy.x) ? 1 : -1;
        // Check tiles between enemy and player on same row
        int y = posEnemy.y / ts;
        int x0 = posEnemy.x / ts;
        int x1 = targetPos.x / ts;
        if (x0 > x1) std::swap(x0, x1);
        for (int x = x0; x <= x1; ++x) {
            if (!map->isTransparentAtTile(x, y, scene->CurrentMap)) {
                // Allow if final tile is target tile
                if (x == targetPos.x / ts) break;
                return false;
            }
        }
        outDir = glm::ivec2(dir, 0);
        return true;
    }
    // Same column
    if (targetPos.x / ts == posEnemy.x / ts) {
        int dir = (targetPos.y > posEnemy.y) ? 1 : -1;
        int x = posEnemy.x / ts;
        int y0 = posEnemy.y / ts;
        int y1 = targetPos.y / ts;
        if (y0 > y1) std::swap(y0, y1);
        for (int y = y0; y <= y1; ++y) {
            if (!map->isTransparentAtTile(x, y, scene->CurrentMap)) {
                if (y == targetPos.y / ts) break;
                return false;
            }
        }
        outDir = glm::ivec2(0, dir);
        return true;
    }
    return false;
}

void Enemy::tryShootAt(const glm::ivec2& targetPos)
{
    if (EnemyType == 0) return; // only armed enemies
    glm::ivec2 dir;
    if (!hasClearAxisShot(targetPos, dir)) return;
    SoundManager::instance().playSound("shoot");

    Bullet b;
    // Spawn segons direcció: amunt surt del cap, altrament del centre lleugerament amunt
    const float muzzleYOffset = -10.0f; // per la resta direccions
    float spawnX = posEnemy.x + 16.0f;
    float spawnY;
    if (dir.y < 0) {
        // cap amunt -> cap de sprite (soldat 32x64 => cap ~ y+16)
        spawnY = (EnemyType == 0) ? (posEnemy.y + 16.0f) : (posEnemy.y + 16.0f);
    } else {
        // resta direccions -> centre
        spawnY = (EnemyType == 0) ? (posEnemy.y + 16.0f + muzzleYOffset) : (posEnemy.y + 32.0f + muzzleYOffset);
    }

    b.pos = glm::vec2(spawnX, spawnY);
    b.dir = dir;
    b.speed = 0.2f; // pixels per ms
    b.active = true;
    bullets.push_back(b);
}

void Enemy::BOSStryShootAt(const glm::ivec2& targetPos)
{
    SoundManager::instance().playSound("shoot");

	float targetX = static_cast<float>(targetPos.x);
	float targetY = static_cast<float>(targetPos.y);
    glm::vec2 playerCenter = glm::vec2(targetX, targetY) + glm::vec2(+20, +20);
    glm::vec2 enemyCenter = glm::vec2 (posEnemy.x + 28.f, posEnemy.y + 52.f);


    std::cout << "Player center: (" << playerCenter.x << ", " << playerCenter.y << ")\n";
    std::cout << "Enemy center: (" << enemyCenter.x << ", " << enemyCenter.y << ")\n";

    // vector del enemic al player
    glm::vec2 dir = glm::vec2(playerCenter - enemyCenter);

    // si el vector és nul (player just a sobre)
    if (glm::length(dir) < 0.01f)
        return;

    std::cout << "Bullet dir1: (" << dir.x << ", " << dir.y << ")\n";

    dir = glm::normalize(dir);

    std::cout << "Bullet dir: (" << dir.x << ", " << dir.y << ")\n";
    Bullet b;
    b.pos = glm::vec2(enemyCenter.x, enemyCenter.y); // centrat
    b.dir = dir;
    b.speed = 0.25f;
    b.active = true;

    

    bullets.push_back(b);
}

void Enemy::updateProjectiles(int deltaTime)
{
    if (bullets.empty()) return;
    Player* pl = scene->getPlayer();
    if (!pl) return;

    // Player full body 32x64
    glm::ivec2 playerPos = pl->posPlayer;
    glm::ivec2 playerSize(32, 64);

    auto intersects = [&](const glm::vec2& p) {
        return p.x >= playerPos.x && p.x <= playerPos.x + playerSize.x &&
               p.y >= playerPos.y && p.y <= playerPos.y + playerSize.y;
    };

    const int ts = map->getTileSize();
    for (auto& b : bullets) {
        if (!b.active) continue;
        b.pos.x += b.dir.x * b.speed * deltaTime;
        b.pos.y += b.dir.y * b.speed * deltaTime;

        int tx = int(b.pos.x) / ts;
        int ty = int(b.pos.y) / ts;
        if (!map->isTransparentAtTile(tx, ty, scene->CurrentMap)) {

            b.active = false;
            continue;
        }

        if (intersects(b.pos)) {
            pl->baixavida(3);
            b.active = false;
        }
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());
}

void Enemy::renderProjectiles()
{
    // Render bullets in screen space using fixed-function pipeline and restore shader after
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

    // Restore matrices
    glPopMatrix(); // MODELVIEW
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Restore previous shader program
    glUseProgram(prevProgram);
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

bool Enemy::enemic_detectat(const glm::ivec2& targetPos, int /*radius_detection*/) const
{
    if (!map) return false;

    const int ts = map->getTileSize();
    const int tolerance = ts; // tolerància d'1 tile

    // Posicions centrades en píxels
    glm::ivec2 enemyCenter = posEnemy + glm::ivec2(ts / 2, ts / 2);
    glm::ivec2 targetCenter = targetPos + glm::ivec2(ts / 2, ts / 2);

    // Vector direcció de l'enemic (facing)
    glm::ivec2 f = EnemyFacingFromAnim(sprite->animation());
    glm::ivec2 delta = targetCenter - enemyCenter;

    if (delta.x == 0 && delta.y == 0)
        return false;

    bool alineat = false;

    if (f.x == 0 && f.y != 0) {
        // Mira amunt o avall → mateixa X (amb tolerància)
        alineat = (std::abs(delta.x) <= tolerance &&
            ((f.y > 0 && delta.y > 0) || (f.y < 0 && delta.y < 0)));
    }
    else if (f.y == 0 && f.x != 0) {
        // Mira dreta o esquerra → mateixa Y (amb tolerància)
        alineat = (std::abs(delta.y) <= tolerance &&
            ((f.x > 0 && delta.x > 0) || (f.x < 0 && delta.x < 0)));
    }
    else if (f.x != 0 && f.y != 0) {
        // Mira en diagonal → permet error d'un tile a la línia ideal
        // La línia diagonal ideal té pendent ±1, així que comparem |dx| i |dy|
        alineat = (std::abs(std::abs(delta.x) - std::abs(delta.y)) <= tolerance &&
            ((f.x > 0 && delta.x > 0) || (f.x < 0 && delta.x < 0)) &&
            ((f.y > 0 && delta.y > 0) || (f.y < 0 && delta.y < 0)));
    }

    if (!alineat)
        return false;

    // Raycast per comprovar que no hi ha parets pel mig
    int dx = delta.x;
    int dy = delta.y;
    int steps = std::max(std::abs(dx), std::abs(dy));
    steps = std::max(1, steps / (ts / 2)); // mostreig cada mig tile

    for (int i = 1; i <= steps; ++i) {
        float t = float(i) / float(steps);
        int sx = int(std::round(enemyCenter.x + t * dx));
        int sy = int(std::round(enemyCenter.y + t * dy));

        int tx = sx / ts;
        int ty = sy / ts;

        if (!map->isTransparentAtTile(tx, ty, scene->CurrentMap)) {
            // Si el tile opac és el del jugador, OK; sinó, bloquejat
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
            if ((dirX < 0 && map->collisionMoveLeft(posEnemy, bbox, scene->CurrentMap)) ||
                (dirX > 0 && map->collisionMoveRight(posEnemy, bbox, scene->CurrentMap)))
            {
                posEnemy.x -= dirX * step; // revertir
            } else {
                movedX = true;
            }

            // Després Y
            posEnemy.y += dirY * step;
            if ((dirY < 0 && map->collisionMoveUP(posEnemy, bbox, scene->CurrentMap)) ||
                (dirY > 0 && map->collisionMoveDown(posEnemy, bbox, scene->CurrentMap)))
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
                    if ((dirX < 0 && map->collisionMoveLeft(posEnemy, bbox, scene->CurrentMap)) ||
                        (dirX > 0 && map->collisionMoveRight(posEnemy, bbox, scene->CurrentMap)))
                    {
                        posEnemy.x -= dirX * step;
                        // prova Y
                        posEnemy.y += dirY * step;
                        if ((dirY < 0 && map->collisionMoveUP(posEnemy, bbox, scene->CurrentMap)) ||
                            (dirY > 0 && map->collisionMoveDown(posEnemy, bbox, scene->CurrentMap)))
                        {
                            posEnemy.y -= dirY * step;
                        }
                    }
                } else {
                    // prova Y
                    posEnemy.y += dirY * step;
                    if ((dirY < 0 && map->collisionMoveUP(posEnemy, bbox, scene->CurrentMap)) ||
                        (dirY > 0 && map->collisionMoveDown(posEnemy, bbox, scene->CurrentMap)))
                    {
                        posEnemy.y -= dirY * step;
                        // prova X
                        posEnemy.x += dirX * step;
                        if ((dirX < 0 && map->collisionMoveLeft(posEnemy, bbox, scene->CurrentMap)) ||
                            (dirX > 0 && map->collisionMoveRight(posEnemy, bbox, scene->CurrentMap)))
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
                if ((dirX < 0 && map->collisionMoveLeft(posEnemy, bbox, scene->CurrentMap)) ||
                    (dirX > 0 && map->collisionMoveRight(posEnemy, bbox, scene->CurrentMap)))
                {
                    posEnemy.x -= dirX * step;
                }
            }
            if (dirY != 0) {
                posEnemy.y += dirY * step;
                if ((dirY < 0 && map->collisionMoveUP(posEnemy, bbox, scene->CurrentMap)) ||
                    (dirY > 0 && map->collisionMoveDown(posEnemy, bbox, scene->CurrentMap)))
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
                bool collide = (dirX < 0 && map->collisionMoveLeft(posEnemy, bbox, scene->CurrentMap)) ||
                               (dirX > 0 && map->collisionMoveRight(posEnemy, bbox, scene->CurrentMap));
                if (collide) {
                    posEnemy.x -= dirX * step;
                    int dirY = sgn(dy);
                    if (dirY != 0) {
                        posEnemy.y += dirY * step;
                        bool colY = (dirY < 0 && map->collisionMoveUP(posEnemy, bbox, scene->CurrentMap)) ||
                                    (dirY > 0 && map->collisionMoveDown(posEnemy, bbox, scene->CurrentMap));
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
                bool collide = (dirY < 0 && map->collisionMoveUP(posEnemy, bbox, scene->CurrentMap)) ||
                               (dirY > 0 && map->collisionMoveDown(posEnemy, bbox, scene->CurrentMap));
                if (collide) {
                    posEnemy.y -= dirY * step;
                    int dirX = sgn(dx);
                    if (dirX != 0) {
                        posEnemy.x += dirX * step;
                        bool colX = (dirX < 0 && map->collisionMoveLeft(posEnemy, bbox, scene->CurrentMap)) ||
                                    (dirX > 0 && map->collisionMoveRight(posEnemy, bbox, scene->CurrentMap));
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

void Enemy::baixavida(int dg) {
    
	vida = vida - dg;
    if (vida <= 0) {
        mort = true;
        hasEnemyDetected = false;
    }
}