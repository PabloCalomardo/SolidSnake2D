#pragma once
#include "Sprite.h"
#include "TileMap.h"
#include "ShaderProgram.h"
#include <glm/glm.hpp>

class Scene;



// `objeto` és un sprite estàtic d'un full PNG amb 3 icones.
// Es comporta com un enemic pel que fa a render/update bàsic, però sense IA ni moviment.
class objeto {
public:
    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int type, Scene& sc, int originalScene, glm::ivec2 posicio);
    void update(int deltaTime);
    void render();

    void setTileMap(TileMap* tileMap);
    void setPosition(const glm::vec2& pos);

    // Estat públic mínim per integració ràpida
    glm::ivec2 posObjeto{0, 0};
    int Escena_Original = 1;
    bool recollit = false; // marcat si el jugador el recull

    glm::ivec2 getSize() const { return glm::ivec2(32, 32); }
    Sprite* getSprite() { return sprite; }
    glm::ivec2 position;
  
private:
    Texture spritesheet;
    Sprite* sprite = nullptr;
    TileMap* map = nullptr;
    Scene* scene = nullptr;
    glm::ivec2 tileMapDispl{0, 0};
};

