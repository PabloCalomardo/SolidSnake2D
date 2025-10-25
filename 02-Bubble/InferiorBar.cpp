#include "InferiorBar.h"
#include "Scene.h"
#include "Player.h"
#include <algorithm>
#include <cctype>

#include <GL/glew.h>

#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>

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
    // Sync HUD items from player inventory each frame
    if (!scene) return;
    Player* player = scene->getPlayer();
    // Reset HUD object pointers
    Caja = nullptr;
    Arma = nullptr;
    Vida1 = Vida2 = Vida3 = nullptr;
    if (!player) return;

    for (objeto* o : player->inventari) {
        if (!o) continue;
        Sprite* s = o->getSprite();
        if (!s) continue;
        int t = s->animation();
        if (t == 0) {
            if (!Caja) Caja = o;
        } else if (t == 1) {
            if (!Arma) Arma = o;
        } else if (t == 2) {
            if (!Vida1) Vida1 = o;
            else if (!Vida2) Vida2 = o;
            else if (!Vida3) Vida3 = o;
        }
    }
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
            drawChar(cursorX+32, y-24, c, pixelSize, spacing);
            cursorX += spriteAdvance;
        } else {
            drawChar(cursorX+32, y-24, c, pixelSize, spacing);
            cursorX += (5 * pixelSize) + spacing * 6;
        }
    }
}

void InferiorBar::render() {
    if (!scene) return;

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
    renderWeaponSection(sectionW-24 + padding, innerY, sectionW - padding * 2, arma, inv, sel);
    renderMapSection((sectionW-16) * 2 + padding, innerY, sectionW - padding * 2, map);

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
    glVertex2f(32.f, float(SCREEN_HEIGHT - barHeight - 24));
    glVertex2f(float(672), float(SCREEN_HEIGHT - barHeight - 24));
    glVertex2f(float(672), float(SCREEN_HEIGHT - 24));
    glVertex2f(32.f, float(SCREEN_HEIGHT - 24));
    glEnd();
}

void InferiorBar::drawFrame() {
    // White 1px rectangle border around the HUD bar
    glColor3f(1.f, 1.f, 1.f);
    const int x = 32;
    const int y = SCREEN_HEIGHT - barHeight - 24;
    const int w = 640;
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
    const int y = SCREEN_HEIGHT - barHeight + 2 - 24;
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
    int startY = y ; // a bit below the title
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

    // Use the provided section width for the item box so slots can expand
    int boxW = width;
    int boxH = 44; // make box taller to fit larger icons
    int boxX = x;
    int boxY = y - 2;
    glColor3f(1.f, 1.f, 1.f);
    drawRect(boxX, boxY, boxW, boxH);

    // We'll reserve up to 5 slots: Caja, Arma, Vida1, Vida2, Vida3
    const int totalSlots = 5;
    // make slots reasonably large so caja/vida (larger sprites) fit comfortably
    int slotW = 28; // slot square size
    int slotH = 28;
    int spacing = 10; // more separation to avoid overlap
    int totalW = totalSlots * slotW + (totalSlots - 1) * spacing;
    int startX = boxX + std::max(0, (boxW - totalW) / 2);
    int startY = boxY + (boxH - slotH) / 2;

    // Determine which HUD pointer corresponds to player's selected item (to highlight)
    Player* player = scene ? scene->getPlayer() : nullptr;
    objeto* selectedObj = nullptr;
    if (player && player->getSelectedItemIndex() >= 0 && player->getSelectedItemIndex() < (int)player->inventari.size())
        selectedObj = player->inventari[player->getSelectedItemIndex()];

    // Prepare shader projection for sprite rendering
    glm::mat4 proj = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
    glm::mat4 modelview = glm::mat4(1.0f);

    for (int i = 0; i < totalSlots; ++i) {
        int sx = startX + i * (slotW + spacing);
        int sy = startY;
        // draw slot outline
        drawRect(sx, sy, slotW, slotH);

        objeto* slotObj = nullptr;
        switch (i) {
            case 0: slotObj = Caja; break;
            case 1: slotObj = Arma; break;
            case 2: slotObj = Vida1; break;
            case 3: slotObj = Vida2; break;
            case 4: slotObj = Vida3; break;
        }

        if (slotObj && slotObj->getSprite()) {
            Sprite* itemSpr = slotObj->getSprite();
            // set shader projection and modelview for HUD rendering
            if (shaderProg) {
                shaderProg->use();
                shaderProg->setUniformMatrix4f("projection", proj);
                shaderProg->setUniformMatrix4f("modelview", modelview);
                shaderProg->setUniform2f("texCoordDispl", 0.f, 0.f);
            }

            // Compute center within slot and render sprite there
            float centerX = float(sx + slotW / 2);
            float centerY = float(sy + slotH / 2);

            // If Caja or Vida, render at half size (scale 0.5) and center
            bool isLarge = (slotObj == Caja) || (slotObj == Vida1) || (slotObj == Vida2) || (slotObj == Vida3);
            if (isLarge) {
                float scale = 0.5f; // half size
                // sprite quad original size is 16x16 or 24x16 (we offset to center)
                float drawX;
                if (slotObj == Caja) drawX = centerX - (slotW * scale) / 2.0f;
                else drawX = centerX - 1 - (slotW * scale) / 2.0f;
                float drawY = centerY - (slotH * scale) / 2.0f;
                itemSpr->renderAtScaled(glm::vec2(drawX, drawY), scale);
            } else {
                // weapon: draw normally centered
                float drawX = centerX + 1 - slotW / 2.0f + 2.0f;
                float drawY = centerY + 5 - slotH / 2.0f + 2.0f;
                itemSpr->renderAt(glm::vec2(drawX, drawY));
            }

            // restore fixed-function
            glUseProgram(0);
        }

        // highlight selected
        if (slotObj && selectedObj && slotObj == selectedObj) {
            glColor3f(1.f, 1.f, 1.f);
            drawRect(sx - 3, sy - 3, slotW + 6, slotH + 6);
        }
    }
}

void InferiorBar::renderMapSection(int x, int y, int width, int mapId) {
    // Title
    const int titleSize = 2;
    drawText(x, y, "KEY", titleSize, 1);
    Player* player = scene ? scene->getPlayer() : nullptr;
    if (player != nullptr) {
        if (player->hasKey) drawText(x + 32, y + 28, "YES", titleSize, 1);
        else drawText(x + 40, y + 28, "NO", titleSize, 1);
    }
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
