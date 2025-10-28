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

#define PIXEL_X2 1/808.0f
#define PIXEL_Y2 1/546.0f

InferiorBar::InferiorBar() {}

enum PlayerAnims
{
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	one, two, three, four, five, six, seven, eight, nine, zero, 
    punto, question, exclamation, exclamation2, coma, apostrof, comillas, guion
};

void InferiorBar::init(Scene* sc, ShaderProgram& shaderProgram, const glm::ivec2& tileMapPos) {
    scene = sc;
    shaderProg = &shaderProgram;

    spritesheet.loadFromFile("images/lletres.png", TEXTURE_PIXEL_FORMAT_RGBA); 
    sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(PIXEL_X * 8, PIXEL_Y * 8), &spritesheet, shaderProg);
    sprite->setNumberAnimations(44);

    spritesheet2.loadFromFile("images/Pantallas_Inicio.png", TEXTURE_PIXEL_FORMAT_RGBA);
    spriteArma = Sprite::createSprite(glm::ivec2(24*2, 16*2), glm::vec2(PIXEL_X2 * 24, PIXEL_Y2 * 16), &spritesheet2, shaderProg);
    spriteArma->setNumberAnimations(1);
    spriteArma->setAnimationSpeed(0, 1);
    spriteArma->addKeyframe(0, glm::vec2(PIXEL_X2 * (2), PIXEL_Y2 * 269));
    spriteArma->changeAnimation(0);

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
        sprite->addKeyframe(i + 26, glm::vec2(PIXEL_X * (181 + (i * 11)), PIXEL_Y * (1 + j)));
    }
    j = 39;
    for (int i = 0; i < 8; ++i) {
        sprite->setAnimationSpeed(i + 36, 8);
        sprite->addKeyframe(i + 36, glm::vec2(PIXEL_X * (181 + (i * 11)), PIXEL_Y * (1 + j)));
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
    char uc = c;
    if (uc != '-' && uc != '\'') uc = char(std::toupper(static_cast<unsigned char>(c)));
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
    if ((uc == '-' || uc == '\'') && sprite && shaderProg) {
        int animId = 0;
        if(uc == '-') animId = guion;
		else animId = apostrof;
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

void InferiorBar::Menu() {
    drawBarBackground();
    const int titleSize = 4;
    if (menuCount == 0) {
        menuCount = 60;
        glColor3f(0.f, 0.f, 0.f);
        glBegin(GL_QUADS);
        glVertex2f(230.f, 390.f);
        glVertex2f(280.f, 390.f);
        glVertex2f(280.f, 550.f);
        glVertex2f(230.f, 550.f);
        glEnd();
    }
    drawText(280, 390, "START", titleSize, 1);
    drawText(280, 425, "INSTRUCCIONES", titleSize, 1);
    drawText(280, 460, "CREDITS", titleSize, 1);
    drawText(280, 495, "SALIR", titleSize, 1);
    if (menuCount < 30) {
        if (scene->currentOption == 1) spriteArma->setPosition(glm::vec2(250.f, 360.f));
        else if (scene->currentOption == 2) spriteArma->setPosition(glm::vec2(250.f, 395.f));
        else if (scene->currentOption == 3) spriteArma->setPosition(glm::vec2(250.f, 430.f));
        else spriteArma->setPosition(glm::vec2(250.f, 462.f));
        spriteArma->render();
    }
    menuCount -= 1;
}

void InferiorBar::Instructions() {
    drawBarBackground();
    const int titleSize = 2;
    const int yTop = 24;
    const int padding = 10;
    const int innerY = yTop + padding;
    const int innerH = padding * 2;
    drawText(210, 70, "INSTRUCCIONES", titleSize, 1);
    drawText(50, 86 + 36, "WASD O FLECHAS - MOVIMIENTO", 1, 1);
    drawText(50, 86 + 64, "C - CAMBIAR OBJETO.", 1, 1);
    drawText(50, 86 + 92, "X - USAR OBJETO.", 1, 1);
    drawText(50, 86 + 120, "Z - GOLPEAR O DISPARAR.", 1, 1);
    drawText(50, 86 + 148, "I - OBTENER TODOS LOS OBJETOS", 1, 1);
    drawText(50, 86 + 176, "H - CURARSE", 1, 1);
    drawText(50, 86 + 204, "P - TELETRANSPORTE A LA JUNGLA", 1, 1);
    drawText(50, 86 + 232, "K - TELETRANSPORTE AL EDIFICIO", 1, 1);
    drawText(50, 86 + 260, "K - TELETRANSPORTE AL JEFE FINAL", 1, 1);
    drawText(50, 86 + 288, "N - INDETECTABLE", 1, 1);
    drawText(50, 86 + 316, "G - MODO DIOS", 1, 1);
    drawText(50, 86 + 344, "R - VOLVER AL MENU", 1, 1);
    drawText(50, 86 + 372, "ENTER - SELECCIONAR", 1, 1);
    drawText(40, 550, "PULSA R PARA VOLVER AL MENU PRINCIPAL", 1, 1);
}

void InferiorBar::Credits() {

    creditsY -= 0.5;
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.f, 0.f, 0.f);
    glBegin(GL_QUADS);
    glVertex2f(32.f, float(SCREEN_HEIGHT - 24));
    glVertex2f(float(672), float(SCREEN_HEIGHT - 24));
    glVertex2f(float(672), float(16));
    glVertex2f(32.f, float(16));
    glEnd();
    drawBarBackground();
    if (creditsY >= 40 && creditsY <= (SCREEN_HEIGHT - 24)) drawText(210.f, creditsY, "PROGRAMADORES");
    if (creditsY >= -20 && creditsY <= (SCREEN_HEIGHT - 84)) drawText(65.f, creditsY + 60.f, "ENRIC SEGARRA");
    if (creditsY >= -20 && creditsY <= (SCREEN_HEIGHT - 84)) drawText(340.f, creditsY + 60.f, "PABLO CALOMARDO");
    if (creditsY >= -140 && creditsY <= (SCREEN_HEIGHT - 204)) drawText(100.f, creditsY + 180.f, "CREADOR Y DIRECTOR ORIGINAL");
    if (creditsY >= -200 && creditsY <= (SCREEN_HEIGHT - 264)) drawText(215.f, creditsY + 240.f, "HIDEO KOJIMA");
    if (creditsY >= -300 && creditsY <= (SCREEN_HEIGHT - 364)) drawText(160.f, creditsY + 340.f, "ARTISTAS ORIGINALES");
    if (creditsY >= -360 && creditsY <= (SCREEN_HEIGHT - 424)) drawText(65.f, creditsY + 400.f, "MASAMI TABATA");
    if (creditsY >= -360 && creditsY <= (SCREEN_HEIGHT - 424)) drawText(340.f, creditsY + 400.f, "AZUSA FUJIMOTO");
    if (creditsY >= -460 && creditsY <= (SCREEN_HEIGHT - 524)) drawText(125.f, creditsY + 500.f, "DESARROLLADORA ORIGINAL");
    if (creditsY >= -520 && creditsY <= (SCREEN_HEIGHT - 584)) drawText(260.f, creditsY + 560.f, "KONAMI");
    if (creditsY >= -640 && creditsY <= (SCREEN_HEIGHT - 704)) drawText(260.f, creditsY + 680.f, "MUSICA");
    if (creditsY >= -720 && creditsY <= (SCREEN_HEIGHT - 784)) drawText(90.f, creditsY + 760.f, "ENCOUNTER - METAL GEAR SOLID");
    if (creditsY >= -760 && creditsY <= (SCREEN_HEIGHT - 824)) drawText(150.f, creditsY + 800.f, "KCE SOUND TEAM JAPAN");
    if (creditsY >= -820 && creditsY <= (SCREEN_HEIGHT - 884)) drawText(20.f, creditsY + 860.f, "FATHER AND SON - METAL GEAR SOLID 2");
    if (creditsY >= -860 && creditsY <= (SCREEN_HEIGHT - 924)) drawText(185.f, creditsY + 900.f, "NORIHIKO HIBINO");
    if (creditsY >= -920 && creditsY <= (SCREEN_HEIGHT - 984)) drawText(65.f, creditsY + 960.f, "BATTLE IN THE JUNGLE - CAUTION");
    if (creditsY >= -950 && creditsY <= (SCREEN_HEIGHT - 1014)) drawText(170.f, creditsY + 990.f, "METAL GEAR SOLID 3");
    if (creditsY >= -990 && creditsY <= (SCREEN_HEIGHT - 1054)) drawText(185.f, creditsY + 1030.f, "NORIHIKO HIBINO");
    if (creditsY >= -1050 && creditsY <= (SCREEN_HEIGHT - 1114)) drawText(70.f, creditsY + 1090.f, "REX'S LAIR - METAL GEAR SOLID");
    if (creditsY >= -1090 && creditsY <= (SCREEN_HEIGHT - 1154)) drawText(150.f, creditsY + 1130.f, "KCE SOUND TEAM JAPAN");
    if (creditsY >= -1150 && creditsY <= (SCREEN_HEIGHT - 1214)) drawText(55.f, creditsY + 1190.f, "MAIN THEME - METAL GEAR SOLID 3");
    if (creditsY >= -1190 && creditsY <= (SCREEN_HEIGHT - 1254)) drawText(135.f, creditsY + 1230.f, "HARRY GREGSON-WILLIAMS");
    if (creditsY >= -1250 && creditsY <= (SCREEN_HEIGHT - 1314)) drawText(50.f, creditsY + 1290.f, "SNAKE EATER - METAL GEAR SOLID 3");
    if (creditsY >= -1290 && creditsY <= (SCREEN_HEIGHT - 1354)) drawText(60.f, creditsY + 1330.f, "CYNTHIA HARREL    RIKA MURANAKA");
    if (creditsY >= -1370 && creditsY <= (SCREEN_HEIGHT - 1434)) drawText(165.f, creditsY + 1410.f, "EFECTOS DE SONIDO");
    if (creditsY >= -1430 && creditsY <= (SCREEN_HEIGHT - 1494)) drawText(65.f, creditsY + 1470.f, "GAME OVER - METAL GEAR SOLID 2");
    if (creditsY >= -1470 && creditsY <= (SCREEN_HEIGHT - 1534)) drawText(120.f, creditsY + 1510.f, "ALERT - METAL GEAR SOLID");
    if (creditsY >= -1510 && creditsY <= (SCREEN_HEIGHT - 1574)) drawText(85.f, creditsY + 1550.f, "PICK UP - METAL GEAR SOLID 5");
    if (creditsY >= -1550 && creditsY <= (SCREEN_HEIGHT - 1614)) drawText(65.f, creditsY + 1590.f, "CHANGE ITEM - METAL GEAR SOLID");
    if (creditsY >= -1590 && creditsY <= (SCREEN_HEIGHT - 1654)) drawText(135.f, creditsY + 1630.f, "HEAL - METAL GEAR SOLID");
    if (creditsY >= -1630 && creditsY <= (SCREEN_HEIGHT - 1694)) drawText(40.f, creditsY + 1670.f, "EQUIP GUN - COUNTER STRIKE SOURCE");
    if (creditsY >= -1670 && creditsY <= (SCREEN_HEIGHT - 1734)) drawText(70.f, creditsY + 1710.f, "SHOOT - VKY SOUNDS YT CHANNEL");
    if (creditsY >= -1710 && creditsY <= (SCREEN_HEIGHT - 1774)) drawText(65.f, creditsY + 1750.f, "SELECT - GFX SOUNDS YT CHANNEL");
    if (creditsY >= -1750 && creditsY <= (SCREEN_HEIGHT - 1814)) drawText(70.f, creditsY + 1790.f, "PUNCH - GFX SOUNDS YT CHANNEL");
    if (creditsY >= -1790 && creditsY <= (SCREEN_HEIGHT - 1854)) drawText(85.f, creditsY + 1830.f, "BOX - ALL SOUNDS YT CHANNEL");
    if (creditsY >= -1830 && creditsY <= (SCREEN_HEIGHT - 1894)) drawText(15.f, creditsY + 1870.f, "UNEQUIP - GAMING SOUND FX YT CHANNEL");
    if (creditsY >= -1870 && creditsY <= (SCREEN_HEIGHT - 1934)) drawText(50.f, creditsY + 1910.f, "CHANGE OPTION - KINGDOM HEARTS 2");
    if (creditsY >= -1910 && creditsY <= (SCREEN_HEIGHT - 1974)) drawText(70.f, creditsY + 1950.f, "ENEMY DEATH - FINAL FANTASY VII");
    if (creditsY >= -1840 && creditsY <= (SCREEN_HEIGHT - 2164)) drawText(165.f, creditsY + 2140.f, "GRACIAS POR JUGAR");
    else if (creditsY <= (SCREEN_HEIGHT - 2164)) drawText(165.f, SCREEN_HEIGHT / 2, "GRACIAS POR JUGAR");
    if (creditsY <= -2040) drawText(140.f, 530.f, "PULSA R PARA VOLVER AL MENU");
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

    if (mort) {
        renderDeathMessage();
        return;
    }
    else if (menu) {
        Menu();
        return;
	}
    else if (instructions) {
        Instructions();
		return;
    }
    else if (credits) {
        Credits();
        return;
    }

    drawBarBackground();
    if (scene->CurrentMap == 0) return;
    drawFrame();

    // Gather data
    Player* player = scene->getPlayer();
    int lives = 0;
    bool arma = false;
	bool god = false;
    bool detectable = true;
    std::vector<objeto*> inv;
    int sel = -1;
    if (player) {
        lives = player->getLives();
        god = player->god;
        detectable = scene->detectable;
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
    renderLivesSection(padding, innerY, sectionW - padding * 2, lives, god, detectable);
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

void InferiorBar::renderDeathMessage() {
    drawBarBackground();
    drawFrame();
    const int titleSize = 2;
    const int yTop = SCREEN_HEIGHT - barHeight;
    const int padding = 10;
    const int innerY = yTop + padding;
    const int innerH = barHeight - padding * 2;
    drawText(innerH, innerY, "GAME OVER", titleSize, 1);
    drawText(innerH + 32, innerY + 24, "PULSA ENTER PARA REINICIAR", 1, 1);
    drawText(innerH + 32, innerY + 48, "PULSA R PARA VOLVER AL MENU", 1, 1);
}

void InferiorBar::renderLivesSection(int x, int y, int width, int lives, bool g, bool det) {
    // Title
    const int titleSize = 2;
    drawText(x, y, "LIFE", titleSize, 1);

    // Clamp lives 0..3
    int displayLives = std::max(0, std::min(7, lives));

    // Compute center position inside section to draw the digit sprite
    int startY = y ; // a bit below the title
    int cx = x + width / 2; // center x relative to section
    int charW = 16; // sprite width used in drawText
    int drawX = cx - charW / 2;
    // Draw the number as a sprite digit
    drawChar(drawX, startY, char('0' + displayLives), 3, 1);

    if (g) {
		drawText(x + 86, y + 48, "GOD", 2, 1);
    }
    if (!det) {
        drawText(x + 156, y + 48, "INV", 2, 1);
    }
}

void InferiorBar::renderWeaponSection(int x, int y, int width, bool hasWeapon, const std::vector<objeto*>& inventory, int selectedIndex) {
    // Title
    const int titleSize = 2;
    drawText(x, y, "OBJECTS", titleSize, 1);

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

    objeto* slotObj = nullptr;
    /*for (int i = 0; i < totalSlots + 1; ++i) {
        int sx = startX + (i) * (slotW + spacing);
        int sy = startY;
        // draw slot outline
        glColor3f(1.f, 1.f, 1.f);
        drawRect(sx, sy, slotW, slotH);
    }*/
    for (int i = 0; i < totalSlots + 1; ++i) {

        int sx = startX + (i) * (slotW + spacing);
        int sy = startY;

        switch (i) {
            case 0: slotObj = Arma; break;
            case 1: slotObj = Caja; break;
            case 2: slotObj = Vida1; break;
            case 3: slotObj = Vida2; break;
            case 4: slotObj = Vida3; break;
        }
        if (slotObj == nullptr) continue;

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
                if (slotObj == Caja) drawX = (centerX + (slotW + spacing)) - (slotW * scale) / 2.0f;
                else drawX = (centerX + (slotW + spacing)) - 1 - (slotW * scale) / 2.0f;
                float drawY = centerY - (slotH * scale) / 2.0f;
                itemSpr->renderAtScaled(glm::vec2(drawX, drawY), scale);
            } else {
                // weapon: draw normally centered
                float drawX = (centerX + (slotW + spacing)) + 1 - slotW / 2.0f + 2.0f;
                float drawY = centerY + 5 - slotH / 2.0f + 2.0f;
                itemSpr->renderAt(glm::vec2(drawX, drawY));
            }

            // restore fixed-function
            glUseProgram(0);
        }

        // highlight selected
        if (slotObj && selectedObj && slotObj == selectedObj) {
            glColor3f(1.f, 1.f, 1.f);
            drawRect((sx + (slotW + spacing)) - 3, sy - 3, slotW + 6, slotH + 6);
        }
        slotObj = nullptr;
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
