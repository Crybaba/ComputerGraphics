#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <iostream>

const float pi = 3.14159265358979323846;
float xAlpha = 20;
float zAlpha = 0;
POINTFLOAT pos = {0,0};

void MoveCamera(){
    if(GetKeyState(VK_UP) < 0) xAlpha = ++xAlpha > 180 ? 180 : xAlpha;
    if(GetKeyState(VK_DOWN) < 0) xAlpha = --xAlpha < 0 ? 0 : xAlpha;
    if(GetKeyState(VK_LEFT) < 0) zAlpha++;
    if(GetKeyState(VK_RIGHT) < 0) zAlpha--;

    float ugol = -zAlpha / 180 * pi;
    float speed = 0;

    if(GetKeyState('W') < 0){
            speed = 0.1;
    }

    if(GetKeyState('S') < 0){
            speed = -0.1;
    }

    if(GetKeyState('A') < 0){
        speed = -0.1;
        ugol += pi*0.5;
    }

    if(GetKeyState('D') < 0){
        speed = -0.1;
        ugol -= pi*0.5;
    }

    if(speed != 0){
        pos.x += sin(ugol) * speed;
        pos.y += cos(ugol) * speed;
    }

    glRotatef(-xAlpha, 1,0,0);
    glRotatef(-zAlpha, 0,0,1);
    glTranslatef(-pos.x, -pos.y, -3);
}
