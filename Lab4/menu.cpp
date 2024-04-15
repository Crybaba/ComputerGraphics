#include "menu.hpp"
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "stb-master_lib/stb_easy_font.h"
#include "texture.hpp"

Button *buttons;

int button_Counter = 0;
int button_Moved = -1;

int button_first = 0;
int button_last = 0;

float mouseX, mouseY;
int menuState = 0;

Button* getButtons() {
    return buttons;
}

int getState() {
     return menuState;
}

void setState(int state) {
    menuState = state;
}

int Menu_AddButton(char *name, float x, float y, float width, float height, float sizeTextOnButton) {
    buttons = (Button*)realloc(buttons, sizeof(buttons[0])*(++button_Counter));

    snprintf(buttons[button_Counter-1].name, 20, "%s", name);
    float *vert = buttons[button_Counter-1].vert;
    vert[0]=vert[6]=x;
    vert[2]=vert[4]=x+width;
    vert[1]=vert[3]=y;
    vert[5]=vert[7]=y+height;
    buttons[button_Counter-1].isHover = false;
    buttons[button_Counter-1].isDown = false;
    buttons[button_Counter-1].isActive = true;

    Button *button = buttons + button_Counter - 1;
    button->num_quads = stb_easy_font_print(0, 0, name,0,button->buffer, sizeof(button->buffer));
    button->textPosX = x +(width-stb_easy_font_width(name)*sizeTextOnButton)/2.0;
    button->textPosY = y +(height-stb_easy_font_height(name)*sizeTextOnButton)/2.0;
    button->textPosY+= sizeTextOnButton*2;
    button->sizeTextOnButton = sizeTextOnButton;

    return button_Counter-1;
}

void ShowButton(int buttonId) {
    Button btn1 = buttons[buttonId];
    glVertexPointer(2, GL_FLOAT, 0, btn1.vert);
    glEnableClientState(GL_VERTEX_ARRAY);
    if (!btn1.isActive) glColor3f(0.3, 0.3, 0.3);
    else if(btn1.isDown)glColor3f(0.7, 0.7, 0.7);
    else if (btn1.isHover)glColor3f(0.8, 0.8, 1);
    else glColor3f(0.6, 0.6, 0.6);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glColor3f(0.5,0.5,0.5);
    glLineWidth(1);
    glDrawArrays(GL_LINE_LOOP,0,4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
    glColor3f(0.1,0.1,0.1);
    glTranslatef(btn1.textPosX,btn1.textPosY,0);
    glScalef(btn1.sizeTextOnButton,btn1.sizeTextOnButton,0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, btn1.buffer);
    glDrawArrays(GL_QUADS, 0, btn1.num_quads*4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
}


/*NASTROIKI KNOPKI*/
void Menu_Button__Setting(){
    if(menuState == 1){
        button_first = 1;
        button_last = 5;
    } else if (menuState == 2){
        button_first = 5;
        button_last = 5;
    } else {
        button_first = 0;
        button_last = 3;
    }
}


/* KOORDINATI KNOPKI */
bool isCordInButton(int buttonID, float x, float y) {
    float *vert = buttons[buttonID].vert;
    return (x > vert[0]) && (y > vert[1]) && (x < vert[4]) && (y < vert[5]);
}


/* NASTROYKI KNOPKI NAJATA OTJATA */
void Menu_Button__Down_Hover(){
    for(int i= button_first; i < button_last; i++){
        if (isCordInButton(i, mouseX, mouseY)) {
            buttons[i].isHover = 1;
            button_Moved = i;
        } else {
            buttons[i].isDown = 0;
            buttons[i].isHover = 0;
        }
    }
}


/* VIVOD MENU */
void Menu_ShowMenu() {
    Menu_Button__Setting();
    for(int i = button_first; i < button_last; i++) ShowButton(i);
}


/* NA KAKUYU KNOPKU NAVELIS */
int Menu_MouseMove (float x, float y) {
    mouseX = x;
    mouseY = y;

    Menu_Button__Setting();
    Menu_Button__Down_Hover();

    return button_Moved;
}


/* KAKAYA KNOPKA NAJATA */
int Menu_MouseDown() {
    int button_Downed = -1;

    Menu_Button__Setting();

    for(int i = button_first; i < button_last; i++)
        if (buttons[i].isActive && isCordInButton(i, mouseX, mouseY)) {
            buttons[i].isDown = 1;
            button_Downed = i;
        }

    return button_Downed;
}


/* NAJATIE KNOPOK */
void MouseDown() {
    int button_ID = Menu_MouseDown();

    if (button_ID<0) return;

    if (button_ID == 0) menuState = 2;
    else if (button_ID == 1) menuState = 1;
    else if (button_ID == 2) PostQuitMessage(0);
    else if (button_ID == 3) nextLine(1);
    else if (button_ID == 4) menuState = 0;
}


/* OTPUSTIT NAJATIYA */
void Menu_MouseUp() {
    for (int i = 0; i < button_Counter; i++) buttons[i].isDown = 0;
}


/* OTCHISTKA MENU */
void Menu_Clear() {
    button_Counter=0;
    free(buttons);
    buttons=0;
}
