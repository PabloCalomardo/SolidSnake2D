#include "InferiorBar.h"
#include "Scene.h"
#include "Player.h"
#include <algorithm>
#include <cctype>

#include <GL/glew.h>

#include "Game.h"

InferiorBar::InferiorBar() {}

void InferiorBar::init(Scene* sc) {
    scene = sc;
}

void InferiorBar::update(int /*deltaTime*/) {
    // Nothing dynamic to update yet; values are pulled at render time.
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

    glDisable(GL_TEXTURE_2D);

    // Background and frame
    drawBarBackground();
    drawFrame();

    // Gather data
    Player* player = scene->getPlayer();
    int lives = 0;
    bool arma = false;
    if (player) {
        lives = player->getLives();
        arma = player->hasWeapon();
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

    // Sections
    renderLivesSection(padding, innerY, sectionW - padding * 2, lives);
    renderWeaponSection(sectionW + padding, innerY, sectionW - padding * 2, arma);
    renderMapSection(sectionW * 2 + padding, innerY, sectionW - padding * 2, map);

    glEnable(GL_TEXTURE_2D);

    // Restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glUseProgram(prevProgram);
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

void InferiorBar::drawText(int x, int y, const std::string& text, int pixelSize, int spacing) {
    int cursorX = x;
    for (char c : text) {
        if (c == ' ') {
            cursorX += (5 * pixelSize) + spacing * 6; // extra space for blank
            continue;
        }
        drawChar(cursorX, y, char(std::toupper(static_cast<unsigned char>(c))), pixelSize, spacing);
        cursorX += (5 * pixelSize) + spacing * 6; // character advance
    }
}

void InferiorBar::drawChar(int x, int y, char c, int pixelSize, int spacing) {
    auto it = font.find(c);
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

void InferiorBar::renderLivesSection(int x, int y, int width, int lives) {
    // Title
    const int titleSize = 2;
    drawText(x, y, "LIFE", titleSize, 1);

    // Draw life bar as small white squares (max 3 as per Player logic)
    int maxLives = 3;
    int boxSize = 10;
    int spacing = 6;
    int startX = x;
    int startY = y + 22;

    glColor3f(1.f, 1.f, 1.f);
    for (int i = 0; i < maxLives; ++i) {
        int bx = startX + i * (boxSize + spacing);
        // empty box outline
        drawRect(bx, startY, boxSize, boxSize);
        // filled for current lives
        if (i < lives) {
            drawFilledRect(bx + 2, startY + 2, boxSize - 4, boxSize - 4);
        }
    }
}

void InferiorBar::renderWeaponSection(int x, int y, int width, bool hasWeapon) {
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

    // Label inside: NONE or GUN-like filled icon bar
    if (!hasWeapon) {
        drawText(boxX + 6, boxY + 8, "NONE", 2, 1);
    } else {
        // Simple stylized weapon icon as horizontal bars
        int ix = boxX + 6;
        int iy = boxY + 6;
        glColor3f(1.f, 1.f, 1.f);
        drawFilledRect(ix, iy, boxW - 12, 4);
        drawFilledRect(ix, iy + 8, boxW - 30, 4);
        drawFilledRect(ix, iy + 16, boxW - 20, 4);
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
