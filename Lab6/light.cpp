#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <iostream>



void Init_Light()
{

     GLfloat light_position[] = { 0.0f, 0.0f, 10, 1}; // ������� ��������� (x, y, z)
     GLfloat light_spot_direction[] = { 0.0f, 0.0f, -1.0f }; // ����������� ���������� (x, y, z)
     GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 2.0f }; // ���������
     GLfloat light_diffuse[] = { 2.0f, 2.0f, 2.0f, 1.0f }; // ���������
     GLfloat light_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // ���������

     glEnable(GL_LIGHTING); // �������� ���������
     glShadeModel(GL_SMOOTH);

     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
     glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0f); // ���� ����� ����������
     glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot_direction);
     glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 10.0f); // ���������� ���������
     glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
     glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
     glEnable(GL_LIGHT0); // �������� �������� ����� GL_LIGHT0
}

void Init_Material()
{
    glEnable(GL_COLOR_MATERIAL); //���������� �������������//���������
    glShadeModel(GL_SMOOTH);
    GLfloat material_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat material_diffuse[] = { 2.0f, 1.0f, 2.0f, 1.0f };
    GLfloat material_specular[] = { 1.0f, 2.0f, 1.0f, 1};
    GLfloat material_shininess[] = { 100.0f }; //����� ���������
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
}

void on_light(){
    Init_Light();
    Init_Material();
}

