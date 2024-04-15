#ifndef MENU_HPP
#define MENU_HPP

int Menu_AddButton(char *name, float x, float y, float width, float height, float textScale);
void ShowButton(int buttonId);
void Menu_ShowMenu();
char isCoordInButton(int buttonId, float x, float y);
int Menu_MouseMove(float x, float y);
int Menu_MouseDown();
void Menu_MouseUp();

#endif // MENU_HPP
