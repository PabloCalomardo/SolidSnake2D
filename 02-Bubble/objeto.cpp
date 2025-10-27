#include "objeto.h"
#include "Scene.h"
#include "Game.h"
#include <GL/glew.h>

// Full d'sprites d'objectes. Espera 3 icones en fila horitzontal de 32x32 cadascuna.
// Ex.: images/objects.png (no inclòs). Ajusta la ruta i les coordenades si calen.

//MIDA D'IMATGE: 342x204
#define PIXEL_X 1/342.0f
#define PIXEL_Y 1/204.0f


void objeto::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int type, Scene& sc, int originalScene, glm::ivec2 posicio )
{
    scene = &sc;
    Escena_Original = originalScene;
	position = posicio;
    tipus = type;
    // Carrega full d'objectes
    spritesheet.loadFromFile("images/objects.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Cada icona és 16x16, atlas 3x1 (ajusta si cal)
    const int iconW = 16;
    const int iconH = 16;

    
    sprite = Sprite::createSprite(glm::ivec2(16 * 2, 16 * 2), glm::vec2(PIXEL_X * 16, PIXEL_Y * 16), &spritesheet, &shaderProgram);
    
    if (type == 1){ //Arma
		sprite = Sprite::createSprite(glm::ivec2(24 , 16 ), glm::vec2(PIXEL_X * 24, PIXEL_Y * 16), &spritesheet, &shaderProgram);
    }


    sprite->setNumberAnimations(4);

    // Animacions fixades a un sol frame cadascuna (tipus)
    // La UV x es desplaça: 0 -> Caja, 1 -> Arma, 2 -> Vida
    // Assumim textura de mida (iconW*3, iconH)

    sprite->setAnimationSpeed(0, 1);
    sprite->addKeyframe(0, glm::vec2(PIXEL_X * (116), PIXEL_Y * 40)); // Caja 114, 40

    sprite->setAnimationSpeed(1, 1);
    sprite->addKeyframe(1, glm::vec2(PIXEL_X * (2), PIXEL_Y * 86)); // Arma 2,85

    sprite->setAnimationSpeed(2, 1);
    sprite->addKeyframe(2, glm::vec2(PIXEL_X * (2), PIXEL_Y * 40)); // Vida 2, 40

    sprite->setAnimationSpeed(3, 1);
    sprite->addKeyframe(3, glm::vec2(PIXEL_X * (59), PIXEL_Y * 40)); // CLAU 59, 40

    sprite->changeAnimation(type);

    tileMapDispl = tileMapPos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posicio.x), float(tileMapDispl.y + posicio.y)));
}

void objeto::setTileMap(TileMap* tileMap)
{
    map = tileMap;
}

void objeto::setPosition(const glm::vec2& pos)
{
    posObjeto = glm::ivec2(int(pos.x), int(pos.y));
    if (sprite)
        sprite->setPosition(glm::vec2(float(tileMapDispl.x + posObjeto.x), float(tileMapDispl.y + posObjeto.y)));
}

void objeto::update(int /*deltaTime*/)
{
    if (scene->CurrentMap != Escena_Original) return;
    if (recollit) return;

    if (!sprite) return;
    sprite->update(0);
    // Aquí es podria afegir lògica de recollida (collision amb player)
}

void objeto::render()
{
    if (scene->CurrentMap != Escena_Original) return;
    if (recollit) return;

    if (!sprite) return;
    sprite->render();
}
