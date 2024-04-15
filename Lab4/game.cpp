#include <gl/gl.h>
#include <math.h>
#include <string>
#include "game.hpp"
#include "texture.hpp"
#include "stb-master_lib/stb_image.h"

Player player;


void Game(GLuint texture) {
    static float svertixFrame[] = { 0,0, 0,0, 0,0, 0,0 };
    static float TexCordFrame[] = { 0,0, 0,0, 0,0, 0,0 };
    static float tileCords[] = { 0,0, 0,0, 0,0, 0,0 };

    int up = GetKeyState(VK_UP);
    int left = GetKeyState(VK_LEFT);
    int right = GetKeyState(VK_RIGHT);


    if (up < 0) {
        player.jump();
    }
    if (left < 0 || right < 0) {
        player.vx = 4;
        player.animation = 0;

        if( left < 0 ) player.direction = -1;
        else player.direction = 1;

    } else if (player.onGround) {
        player.vx = 0;
        player.animation = 1;
    }

    player.iter();

    svertixFrame[0] = svertixFrame[2] = player.x;
    svertixFrame[1] = svertixFrame[7] = player.y;
    svertixFrame[3] = svertixFrame[5] = player.y+player.h*player.scale;
    svertixFrame[4] = svertixFrame[6] = player.x+player.w*player.scale;

    TexCordFrame[3] = TexCordFrame[5] = player.animation/3.;
    TexCordFrame[1] = TexCordFrame[7] = (player.animation+1)/3.;
    if (player.direction == 1) {
        TexCordFrame[0] = TexCordFrame[2] = player.frame/7.;
        TexCordFrame[4] = TexCordFrame[6] = (player.frame+1)/7.;
    } else {
        TexCordFrame[4] = TexCordFrame[6] = player.frame/7.;
        TexCordFrame[0] = TexCordFrame[2] = (player.frame+1)/7.;
    }

    GlSettingsOn(texture);

    glVertexPointer(2,GL_FLOAT,0,svertixFrame);
    glTexCoordPointer(2,GL_FLOAT,0,TexCordFrame);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    GLuint block = LoadTexture("block.png");
    GLuint textureID = 0;

    glBindTexture(GL_TEXTURE_2D, 0);
    int rows = 22, cols = 40;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            textureID = 0;
            if (TileMap[rows-i-1][j] == ' ') continue;
            if (TileMap[rows-i-1][j] == 'R') {
                textureID = block;
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, textureID);
                glBegin(GL_QUADS);
                glColor3f(1,1,1);
                glTexCoord2f(0.0f, 0.0f); glVertex2f(j * player.blockSize, i * player.blockSize);
                glTexCoord2f(1.0f, 0.0f); glVertex2f(j * player.blockSize + player.blockSize, i * player.blockSize);
                glTexCoord2f(1.0f, 1.0f); glVertex2f(j * player.blockSize + player.blockSize, i * player.blockSize + player.blockSize);
                glTexCoord2f(0.0f, 1.0f); glVertex2f(j * player.blockSize, i * player.blockSize + player.blockSize);
                glEnd();
                glDisable(GL_TEXTURE_2D);
            }
        }
    }

    GlSettingsOff();
}
