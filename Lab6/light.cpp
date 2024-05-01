#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <iostream>

GLfloat light_position[] = { 0.0f, 0.0f, 2.0f, 0.f }; //позиция источника
GLfloat light_spot_direction[] = {0.0, 0.0, -2.0, 1.0}; // позиция цели
GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f }; //параметры
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //параметры
GLfloat light_specular[] = { 0.2f, 0.2f, 0.2f, 32.0f }; //параметры

bool x = true;

void Init_Light()
{
     glEnable(GL_LIGHTING); //общее освещения для всего пространства
     glShadeModel(GL_SMOOTH);

     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
     glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 15); // конус для
    //направленного источника
     glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot_direction);
     glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 8.0); // экспонента
    //убывания интенсивности
    //задействование настроек для источника LIGHT0
     glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
     glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
     glEnable(GL_LIGHT0); // источник света LIGHT0


}

void Init_Material()
{
    glEnable(GL_COLOR_MATERIAL); //разрешения использования
    //материала
     glShadeModel(GL_SMOOTH); // сглаживает границы
     GLfloat material_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
     GLfloat material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
     GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 32.0f };
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

