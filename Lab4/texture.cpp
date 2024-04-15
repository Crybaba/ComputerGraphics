#include <gl/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <malloc.h>
#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master_lib/stb_image.h"

using namespace std;

int spriteLine = 0;
unsigned short frame = 0;

void nextLine(int k) {
    spriteLine = (spriteLine+3+k)%3;
}

void GlSettingsOn(GLuint texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.7);
    glBindTexture(GL_TEXTURE_2D, texture);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void GlSettingsOff() {
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_ALPHA_TEST);
    glBindTexture(GL_TEXTURE_2D, 0);
}


GLuint LoadTexture(const char *filename)
{
    int width, height, cnt;
    unsigned char *image = stbi_load(filename, &width, &height, &cnt, 0);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
    return textureID;
}

void Load_Texture( char *filename, GLuint *textureID, int swarp, int twarp, int filter) {
    int twidth, thight, tcnt;
    unsigned char *data=stbi_load(filename,&twidth,&thight,&tcnt,0);

    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,swarp);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,twarp);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, twidth, thight, 0,
                 tcnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void Show_Animation(GLuint texture) {

    int frameW = 110;
    int frameH = 80;

    static float svertixLine[] = {
        10, HEIGHT/2-50,
        10, HEIGHT/2-50+frameH,
        WIDTH-25, HEIGHT/2-50+frameH,
        WIDTH-25, HEIGHT/2-50};

    static float TexCordLine[] = {0,1, 0,0, 1,0, 1,1 };

    static float svertixFrame[] = {
        (WIDTH-frameW)/2.,HEIGHT/2+frameH,
        (WIDTH-frameW)/2.,HEIGHT/2+2*frameH,
        (WIDTH+frameW)/2.,HEIGHT/2+2*frameH,
        (WIDTH+frameW)/2.,HEIGHT/2+frameH};

    static float TexCordFrame[] = {0,1, 0,0, 1,0, 1,1 };

    TexCordLine[1] = TexCordLine[7] = TexCordFrame[1] = TexCordFrame[7] = (spriteLine+1)/3.;
    TexCordLine[3] = TexCordLine[5] = TexCordFrame[3] = TexCordFrame[5] = spriteLine/3.;

    TexCordFrame[0] = TexCordFrame[2];
    TexCordFrame[4] = TexCordFrame[6];

    GlSettingsOn(texture);

    glVertexPointer(2,GL_FLOAT,0,svertixLine);
    glTexCoordPointer(2,GL_FLOAT,0,TexCordLine);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glVertexPointer(2,GL_FLOAT,0,svertixFrame);
    glTexCoordPointer(2,GL_FLOAT,0,TexCordFrame);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    GlSettingsOff();

    ++frame;
}

void Show_Background(GLuint texture)
{
    static float svertix[]= {0,0, 0,HEIGHT, WIDTH,HEIGHT, WIDTH,0,};
    static float TexCord[]= {0,1-0, 0,1-1, 1,1-1, 1,1-0};
    glClearColor(0,0,0,0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2,GL_FLOAT,0,svertix);
    glTexCoordPointer(2,GL_FLOAT,0,TexCord);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
