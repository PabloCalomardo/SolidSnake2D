#ifndef _INFERIOR_BAR_INCLUDE
#define _INFERIOR_BAR_INCLUDE

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <array>
#include <string>
#include <vector>

#include "objeto.h"
#include "ShaderProgram.h"
class Scene;

// Simple black & white pixel-art HUD rendered at the bottom of the screen.
// Shows: weapon status (SI/NO), player lives, and current map.
class InferiorBar {
public:
    InferiorBar();
    ~InferiorBar() = default;

    void init(Scene* scene, ShaderProgram& shaderProgram, const glm::ivec2& tileMapPos);
    void update(int deltaTime);
    void render();
    Texture spritesheet;
    Sprite* sprite;

	objeto* Caja;
	objeto* Vida1;
	objeto* Vida2;
	objeto* Vida3;
	objeto* Arma;

private:
    void ensureFont();
    void drawBarBackground();
    void drawFrame();
    void drawSeparator(int x);

    void drawText(int x, int y, const std::string& text, int pixelSize = 3, int spacing = 1);
    void drawChar(int x, int y, char c, int pixelSize, int spacing);
    void drawCharDigitAt(int x, int y, char digit);

    void drawFilledRect(int x, int y, int w, int h);
    void drawRect(int x, int y, int w, int h);

    // NES-like layout sections
    void renderLivesSection(int x, int y, int width, int lives);
    void renderWeaponSection(int x, int y, int width, bool hasWeapon, const std::vector<objeto*>& inventory, int selectedIndex);
    void renderMapSection(int x, int y, int width, int mapId);

private:
    Scene* scene = nullptr;
    bool fontInitialized = false;
    // 5x7 font: 7 rows, each row a 5-bit mask (LSB is rightmost bit 0..4)
    std::unordered_map<char, std::array<uint8_t, 7>> font;

    int barHeight = 80; // pixels

    // Shader used by Sprite rendering (set in init)
    ShaderProgram* shaderProg = nullptr;
};

#endif // _INFERIOR_BAR_INCLUDE
