#include "InferiorBar.h"
#include "Scene.h"
#include "Player.h"
#include <algorithm>
#include <cctype>

#include <GL/glew.h>

#include "Game.h"

#define PIXEL_X 1/301.0f
#define PIXEL_Y 1/127.0f

InferiorBar::InferiorBar() {}

enum PlayerAnims
{
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	one, two, three, four, five, six, seven, eight, nine, zero
};

void InferiorBar::init(Scene* sc, ShaderProgram& shaderProgram, const glm::ivec2& tileMapPos) {
    scene = sc;
    shaderProg = &shaderProgram;

    spritesheet.loadFromFile("images/lletres.png", TEXTURE_PIXEL_FORMAT_RGBA); 
    sprite = Sprite::createSprite(glm::ivec2(8 * 2, 8 * 2), glm::vec2(PIXEL_X * 8, PIXEL_Y * 8), &spritesheet, shaderProg);
    sprite->setNumberAnimations(36);

    int j = 9;
    for (int i = 0; i < 13; ++i) {
        sprite->setAnimationSpeed(i, 8);
        sprite->addKeyframe(i, glm::vec2(PIXEL_X * (2 + (i * 11)), PIXEL_Y * (1+j)));
    }
    j = 20;
    for (int i = 0; i < 13; ++i) {
        sprite->setAnimationSpeed(i + 13, 8);
        sprite->addKeyframe(i+13, glm::vec2(PIXEL_X * (2 + (i * 11)), PIXEL_Y * (1 + j)));
    }
    j = 9;
    for (int i = 0; i < 10; ++i) {
        sprite->setAnimationSpeed(i + 26, 8);
        sprite->addKeyframe(i + 26, glm::vec2(PIXEL_X * (181 + (i * 11)), PIXEL_Y * (1 + j + 11)));
    }
}

void InferiorBar::update(int /*deltaTime*/) {
    // Nothing dynamic to update yet; values are pulled at render time.
}

// Helper: draw a single character using sprite font if available; otherwise fallback to bitmap font
void InferiorBar::drawChar(int x, int y, char c, int pixelSize, int spacing) {
    char uc = char(std::toupper(static_cast<unsigned char>(c)));
    // Digits: use sprite animations starting at index 26 but map 0->35
    if (uc >= '0' && uc <= '9' && sprite && shaderProg) {
        int animId;
        if (uc == '0') animId = 35;
        else animId = 25 + (uc - '0'); // '1'->26 .. '9'->34

        GLint prevProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
        shaderProg->use();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Render sprite digit
        sprite->changeAnimation(animId);
        sprite->setPosition(glm::vec2(float(x), float(y)));
        sprite->render();

        // Restore
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        if (prevProgram) glUseProgram(prevProgram);
        return;
    }

    // Use sprite for A-Z
    if (uc >= 'A' && uc <= 'Z' && sprite && shaderProg) {
        int animId = (uc - 'A');
        // Setup ortho and modelview for drawing sprite at screen coords
        GLint prevProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
        shaderProg->use();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_TEXTURE_2D);
        // Use sprite's texture rendering (Sprite::render will enable texture as needed)
        sprite->changeAnimation(animId);
        sprite->setPosition(glm::vec2(float(x), float(y)));
        sprite->render();

        // Restore matrices and program
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        if (prevProgram) glUseProgram(prevProgram);
        return;
    }

    // Fallback to bitmap font for digits and others (if not handled above)
    auto it = font.find(uc);
    if (it == font.end()) return;

    const auto& rows = it->second;
    // White pixels on black background
    glColor3f(1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
    for (int row = 0; row < 7; ++row) {
        uint8_t mask = rows[row];
        for (int col = 0; col < 5; ++col) {
            bool on = (mask >> (4 - col)) & 1; // bit 4 is leftmost
            if (!on) continue;
            int px = x + col * (pixelSize + spacing);
            int py = y + row * (pixelSize + spacing);
            glVertex2f(float(px), float(py));
            glVertex2f(float(px + pixelSize), float(py));
            glVertex2f(float(px + pixelSize), float(py + pixelSize));
            glVertex2f(float(px), float(py + pixelSize));
        }
    }
    glEnd();
}

void InferiorBar::drawText(int x, int y, const std::string& text, int pixelSize, int spacing) {
    int cursorX = x;
    // For sprite-based letters we choose an advance consistent with sprite size (16 px)
    int spriteAdvance = 16;
    for (char c : text) {
        if (c == ' ') {
            cursorX += (5 * pixelSize) + spacing * 6; // keep previous behaviour for spaces
            continue;
        }
        // If char is letter, use sprite with fixed advance; otherwise use bitmap advance
        if (std::isalpha(static_cast<unsigned char>(c))) {
            drawChar(cursorX, y, c, pixelSize, spacing);
            cursorX += spriteAdvance;
        } else {
            drawChar(cursorX, y, c, pixelSize, spacing);
            cursorX += (5 * pixelSize) + spacing * 6;
        }
    }
}

void InferiorBar::render() {
    if (!scene) return;

    ensureFont();

    // Save previous state, switch to fixed-function immediate mode for simple 2D drawing
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

    // Draw background and frame with textures disabled
    glDisable(GL_TEXTURE_2D);

    drawBarBackground();
    drawFrame();

    // Gather data
    Player* player = scene->getPlayer();
    int lives = 0;
    bool arma = false;
    std::vector<objeto*> inv;
    int sel = -1;
    if (player) {
        lives = player->getLives();
        arma = player->hasWeapon();
        inv = player->inventari;
        sel = player->getSelectedItemIndex();
    }

    int map = scene->CurrentMap;

    // NES-like layout: three sections separated by thin vertical lines
    const int yTop = SCREEN_HEIGHT - barHeight;
    const int padding = 10;
    const int innerY = yTop + padding;
    const int innerH = barHeight - padding * 2;

    const int sectionCount = 3;
    const int sectionW = SCREEN_WIDTH / sectionCount;

    // Separators
    for (int i = 1; i < sectionCount; ++i) {
        drawSeparator(i * sectionW);
    }

    // Sections (these will call drawText which may enable textures briefly for sprites)
    renderLivesSection(padding, innerY, sectionW - padding * 2, lives);
    renderWeaponSection(sectionW + padding, innerY, sectionW - padding * 2, arma, inv, sel);
    renderMapSection(sectionW * 2 + padding, innerY, sectionW - padding * 2, map);

    // Restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    if (prevProgram) glUseProgram(prevProgram);
}

void InferiorBar::drawBarBackground() {
    // Solid black rectangle at the bottom
    glColor3f(0.f, 0.f, 0.f);
    glBegin(GL_QUADS);
    glVertex2f(0.f, float(SCREEN_HEIGHT - barHeight));
    glVertex2f(float(SCREEN_WIDTH), float(SCREEN_HEIGHT - barHeight));
    glVertex2f(float(SCREEN_WIDTH), float(SCREEN_HEIGHT));
    glVertex2f(0.f, float(SCREEN_HEIGHT));
    glEnd();
}

void InferiorBar::drawFrame() {
    // White 1px rectangle border around the HUD bar
    glColor3f(1.f, 1.f, 1.f);
    const int x = 0;
    const int y = SCREEN_HEIGHT - barHeight;
    const int w = SCREEN_WIDTH;
    const int h = barHeight;

    glBegin(GL_LINE_LOOP);
    glVertex2f(float(x), float(y));
    glVertex2f(float(x + w), float(y));
    glVertex2f(float(x + w), float(y + h));
    glVertex2f(float(x), float(y + h));
    glEnd();
}

void InferiorBar::drawSeparator(int x) {
    // Thin vertical line separator
    glColor3f(1.f, 1.f, 1.f);
    const int y = SCREEN_HEIGHT - barHeight + 2;
    const int h = barHeight - 4;
    glBegin(GL_LINES);
    glVertex2f(float(x), float(y));
    glVertex2f(float(x), float(y + h));
    glEnd();
}

void InferiorBar::renderLivesSection(int x, int y, int width, int lives) {
    // Title
    const int titleSize = 2;
    drawText(x, y, "LIFE", titleSize, 1);

    // Clamp lives 0..3
    int displayLives = std::max(0, std::min(3, lives));

    // Compute center position inside section to draw the digit sprite
    int startY = y + 24; // a bit below the title
    int cx = x + width / 2; // center x relative to section
    int charW = 16; // sprite width used in drawText
    int drawX = cx - charW / 2;
    // Draw the number as a sprite digit
    drawChar(drawX, startY, char('0' + displayLives), 3, 1);
}

void InferiorBar::renderWeaponSection(int x, int y, int width, bool hasWeapon, const std::vector<objeto*>& inventory, int selectedIndex) {
    // Title
    const int titleSize = 2;
    drawText(x, y, "ARMS", titleSize, 1);

    // Item box (NES-like slot)
    int boxW = 60;
    int boxH = 28;
    int boxX = x;
    int boxY = y + 22;
    glColor3f(1.f, 1.f, 1.f);
    drawRect(boxX, boxY, boxW, boxH);

    // If no inventory, show NONE or existing weapon status
    if (inventory.empty()) {
        if (!hasWeapon) drawText(boxX + 6, boxY + 8, "NONE", 2, 1);
        else drawText(boxX + 6, boxY + 8, "GUN", 2, 1);
        return;
    }

    // Draw up to 3 inventory slots horizontally inside the box
    int maxSlots = std::min(3, int(inventory.size()));
    int slotW = 16;
    int slotH = 16;
    int totalW = maxSlots * slotW + (maxSlots - 1) * 4;
    int startX = boxX + (boxW - totalW) / 2;
    int startY = boxY + (boxH - slotH) / 2;

    for (int i = 0; i < maxSlots; ++i) {
        int sx = startX + i * (slotW + 4);
        int sy = startY;
        // outline box
        drawRect(sx, sy, slotW, slotH);

        // draw simple HUD icon for item type
        objeto* it = inventory[i];
        if (it) {
            int typeAnim = -1;
            if (it->getSprite()) typeAnim = it->getSprite()->animation();
            if (typeAnim == 1) {
                // arma -> horizontal bars
                drawFilledRect(sx + 2, sy + 6, slotW - 4, 3);
            } else if (typeAnim == 0) {
                // caixa -> square center
                drawFilledRect(sx + 4, sy + 4, slotW - 8, slotH - 8);
            } else if (typeAnim == 2) {
                // vida -> small heart-like (two pixels)
                drawFilledRect(sx + 6, sy + 6, 4, 4);
            }
        }

        // highlight selected
        if (i == selectedIndex) {
            // draw a white border thicker
            glColor3f(1.f, 1.f, 1.f);
            drawRect(sx - 2, sy - 2, slotW + 4, slotH + 4);
        }
    }
}

void InferiorBar::renderMapSection(int x, int y, int width, int mapId) {
    // Title
    const int titleSize = 2;
    drawText(x, y, "MAP", titleSize, 1);

    // Big number centered in the section
    std::string num = std::to_string(mapId);

    // Compute rough centered x based on char advance used in drawText
    int pixelSize = 3;
    int charAdvance = (5 * pixelSize) + 6; // spacing *6 like drawText
    int totalW = int(num.size()) * charAdvance;

    int cx = x + std::max(0, (width - totalW) / 2);
    int cy = y + 20;

    drawText(cx, cy, num, pixelSize, 1);
}

void InferiorBar::ensureFont() {
    if (fontInitialized) return;
    fontInitialized = true;

    // Digits 0-9
    font['0'] = { 0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110 };
    font['1'] = { 0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 };
    font['2'] = { 0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111 };
    font['3'] = { 0b11110, 0b00001, 0b00001, 0b01110, 0b00001, 0b00001, 0b11110 };
    font['4'] = { 0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010 };
    font['5'] = { 0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110 };
    font['6'] = { 0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110 };
    font['7'] = { 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000 };
    font['8'] = { 0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110 };
    font['9'] = { 0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100 };

    // Letters (subset): A-Z
    font['A'] = { 0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 };
    font['B'] = { 0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110 };
    font['C'] = { 0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110 };
    font['D'] = { 0b11100, 0b10010, 0b10001, 0b10001, 0b10001, 0b10010, 0b11100 };
    font['E'] = { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111 };
    font['F'] = { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000 };
    font['G'] = { 0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01110 };
    font['H'] = { 0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 };
    font['I'] = { 0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 };
    font['J'] = { 0b00001, 0b00001, 0b00001, 0b00001, 0b10001, 0b10001, 0b01110 };
    font['K'] = { 0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001 };
    font['L'] = { 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111 };
    font['M'] = { 0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001 };
    font['N'] = { 0b10001, 0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001 };
    font['O'] = { 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 };
    font['P'] = { 0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000 };
    font['Q'] = { 0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101 };
    font['R'] = { 0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001 };
    font['S'] = { 0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110 };
    font['T'] = { 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100 };
    font['U'] = { 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 };
    font['V'] = { 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b01010, 0b00100 };
    font['W'] = { 0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b11011, 0b10001 };
    font['X'] = { 0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001 };
    font['Y'] = { 0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100 };
    font['Z'] = { 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111 };

    // Colon and dash
    font[':'] = { 0b00000, 0b00100, 0b00000, 0b00000, 0b00100, 0b00000, 0b00000 };
}

void InferiorBar::drawFilledRect(int x, int y, int w, int h) {
    glBegin(GL_QUADS);
    glVertex2f(float(x), float(y));
    glVertex2f(float(x + w), float(y));
    glVertex2f(float(x + w), float(y + h));
    glVertex2f(float(x), float(y + h));
    glEnd();
}

void InferiorBar::drawRect(int x, int y, int w, int h) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(float(x), float(y));
    glVertex2f(float(x + w), float(y));
    glVertex2f(float(x + w), float(y + h));
    glVertex2f(float(x), float(y + h));
    glEnd();
}
