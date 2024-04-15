#ifndef MENU_HPP
#define MENU_HPP

struct Button {
    char name[20];
    float vert[8], buffer[50*20];
    int num_quads;
    float textPosX, textPosY, sizeTextOnButton;
    bool isHover, isDown, isActive;
};

Button* getButtons();
int getState();
void setState(int st);

int Menu_AddButton(char *name, float x, float y, float width, float height, float sizeTextOnButton);
void ShowButton(int buttonId);
void Menu_ShowMenu();
void Menu_Button__Setting();
void Menu_Button__Down_Hover();
int Menu_MouseMove(float x, float y);
int Menu_MouseDown();
void Menu_MouseUp();
char* Menu_GetButtonName(int buttonID);
void Menu_Clear();
void MouseDown();

#endif // MENU_HPP
