#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "objeto.h"

// Forward declare to avoid heavy includes here
class Enemy;
class InferiorBar;


// Scene contains all the entities of our game.
// It is responsible for updating and render them.
class Enemy;
class Player;

class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();
	void ChangeMap(int dir);
	void tp_to_init(int m);
    void comprovar_vides(int deltaTime);
	int CurrentMap;
    // Accessor for enemies to interact with the player (e.g., projectile hits)
    Player* getPlayer() const { return player; }
    // Access enemies for collision checks (player bullets)
    std::vector<Enemy*>& getEnemies() { return enemies; }
	std::vector<objeto*>& getObjetos() { return objetos; }

private:
	void initShaders();

private:
	TileMap *map;
	Player *player;
    std::vector<Enemy*> enemies;
	std::vector<objeto*> objetos;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

    InferiorBar* hud = nullptr;
};


#endif // _SCENE_INCLUDE

