#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Scene.h"
#include <vector>

class Scene;


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.

class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene &sc);
	void update(int deltaTime);
	void render();
	
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);

	glm::ivec2 tileMapDispl, posPlayer;
	Sprite* getSprite() { return sprite; }
	void baixavida();
    // Optional: set bullet speed in pixels per second
    void setBulletSpeedPerSecond(float pxPerSec) { bulletSpeedPxPerMs = pxPerSec / 1000.0f; }
	
private:
    struct Bullet {
        glm::vec2 pos;      // top-left in pixels
        glm::ivec2 dir;     // unit dir (1,0),(0,1),(-1,0),(0,-1)
        float speed;        // pixels per millisecond
        bool active;
    };

    void updateProjectiles(int deltaTime);
    void renderProjectiles();
    void tryShoot();
    glm::ivec2 facingDirFromAnim(int anim) const;

	int vida;
	bool porta_arma;
	bool ferit;
	bool mort;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	Scene *scene;

    // Player projectile state
    std::vector<Bullet> bullets;
    int shootCooldownMs = 400;
    int shootTimer = 0;
    float bulletSpeedPxPerMs = 0.5f; // 500 px/s

};


#endif // _PLAYER_INCLUDE


