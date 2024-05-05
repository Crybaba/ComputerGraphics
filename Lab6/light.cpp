#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <iostream>



void Init_Light()
{
     GLfloat light_position[] = { 0, 0, 10, 1}; // позиция источника (x, y, z)
     GLfloat light_spot_direction[] = { 0, 0, -1.0f }; // направление прожектора (x, y, z)
     GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // параметры
     GLfloat light_diffuse[] = { 3.0f, 3.0f, 1.0f, 1.0f }; // параметры
     GLfloat light_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // параметры

     glEnable(GL_LIGHTING); // включить освещение
     glShadeModel(GL_SMOOTH);

     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
     glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 70.0f); // угол среза прожектора
     glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot_direction);
     glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 20.0f); // экспонента затухания
     glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
     glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
     glEnable(GL_LIGHT0); // включить источник света GL_LIGHT0
}

void Init_Material()
{
    glEnable(GL_COLOR_MATERIAL); //разрешения использования//материала
    glShadeModel(GL_SMOOTH);
    GLfloat material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat material_diffuse[] = { 2.0f, 1.0f, 2.0f, 2.0f };
    GLfloat material_specular[] = { 1.0f, 2.0f, 1.0f, 2};
    GLfloat material_shininess[] = { 50.0f }; //блеск материала
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
}

void on_light(){
    Init_Light();
    Init_Material();
}

