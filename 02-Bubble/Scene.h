#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "objeto.h"
#include "Sprite.h"
#include "SoundManager.h"
#include "InferiorBar.h"

// Forward declare to avoid heavy includes here
class Enemy;
class InferiorBar;
//class SoundManager;


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
	void ChargeEnemiesAndObjects();
	void DeleteObjectsAndEnemies();
	void update(int deltaTime);
	void render();
	void ChangeMap(int dir);
	void tp_to_map(int m);
	void Death(int op);
    void comprovar_vides(int deltaTime);
	void GoToMainMenu();
	void Instructions();
	void Credits();
	int CurrentMap;
	bool detectable;
	bool detectat;
	int currentOption = 1; // 1 = Start, 2 = Instructions, 3 = Credits, 4 = Exit
	Texture spritesheet;
    // Accessor for enemies to interact with the player (e.g., projectile hits)
    Player* getPlayer() const { return player; }
    // Access enemies for collision checks (player bullets)
    std::vector<Enemy*>& getEnemies() { return enemies; }
	std::vector<objeto*>& getObjetos() { return objetos; }
	Sprite* sprite;
	InferiorBar* hud;
	bool revived = false;
	bool victory = false;

private:
	void initShaders();

private:
	int damageCD;
	TileMap *map;
	Player *player;
	SoundManager *sm;
    std::vector<Enemy*> enemies;
	std::vector<objeto*> objetos;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	

};


#endif // _SCENE_INCLUDE

