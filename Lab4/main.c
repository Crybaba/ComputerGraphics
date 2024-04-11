#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include "stb-master/stb_easy_font.h"
#include "menu.h"

#define WINDOW_HEIGHT 1000
#define WINDOW_WIDTH 1600

#include <float.h>




LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
int button1 = 0;
int button2 = 0;

GLuint Idle_sprite, Walk_sprite, Jump_sprite, background, brickTile, groundTile;

const float blockSize = 40.0f; // размер одного тайла
#define H 25 //Высота экрана в тайлах 1080/40
#define W 40 //Высота экрана в тайлах 1920/40

int currentFrame = 0;          // Текущий кадр анимации
const int totalFrames = 7;     // Всего кадров в спрайт-листе
float frameWidth = 1.0f / 7.0f;  // Ширина одного кадра в текстурных координатах
int isMoving = 0;
float jumpSpeed = 30.0f; // Начальная скорость прыжка
float gravity = -2.0f; // Ускорение, действующее на персонажа при падении
BOOL isAirborne = TRUE;  // Переменная для проверки, находится ли персонаж в воздухе



char TileMap[H][W] = {
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "B                     BBBBBBBBBBBBBBBBBB",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "BBBBBBBBBBBBBB                         B",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "B                 BBBBBBBBBBBBBBBBBBBBBB",
    "B                                      B",
    "BBBB                                   B",
    "BBBB                                   B",
    "BBBBBBBB                     BBBBBBBBBBB",
    "BBBBBBBB                               B",
    "BBBBBBBBBBBB                           B",
    "BBBBBBBBBBBB                           B",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
  };

typedef struct {
    float x, y;          // Позиция
    float dx, dy;        // Скорость
    BOOL isAirborne;     // Находится ли в воздухе
    BOOL isMoving;       // Двигается ли
    float width, height; // Размеры героя
    BOOL faceRight;      //смотрит ли влево
    float scale;
    float currentJumpSpeed;
} Hero;

Hero hero = { .x = 0.0f, .y = 0.0f, .dx = 0.0f, .dy = 0.0f, .isAirborne = TRUE, .isMoving = FALSE, .width = 120.0f, .height = 80.0f, .scale = 1.0f, .currentJumpSpeed = 0.0f};

void DrawMap() {
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            char tile = TileMap[i][j];
            GLuint textureID = 0;

            // Определяем текстуру для текущего тайла
            switch(tile) {
                case 'B': // Кирпич
                    textureID =  brickTile; // Чередование текстур
                    break;
                default:
                    continue; // Пропускаем пустое пространство или неизвестные символы
            }

            // Отрисовка тайла
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glBegin(GL_QUADS);
                glColor3f(1,1,1);
                glTexCoord2f(0.0f, 0.0f); glVertex2f(j * blockSize, i * blockSize);
                glTexCoord2f(1.0f, 0.0f); glVertex2f(j * blockSize + blockSize, i * blockSize);
                glTexCoord2f(1.0f, 1.0f); glVertex2f(j * blockSize + blockSize, i * blockSize + blockSize);
                glTexCoord2f(0.0f, 1.0f); glVertex2f(j * blockSize, i * blockSize + blockSize);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
    }
}


BOOL CollisionHappened(float potentialNewX, float potentialNewY, Hero *hero) {
    for (int i = (int)(potentialNewY / blockSize); i < round((potentialNewY + hero->height * hero->scale) / blockSize); i++) {
        for (int j = (int)(potentialNewX / blockSize); j < round((potentialNewX + hero->width * hero->scale) / blockSize); j++) {
            char tile = TileMap[i][j];
            if (tile == 'B') {
                return TRUE; // Обнаружено столкновение
            }
        }
    }
    return FALSE; // Столкновение не обнаружено
}

void UpdateHeroPositionAndCollisions(Hero *hero) {
    if (hero->y < 0) {
        hero->y = 0;
        hero->dy = 0;
    }
    if (hero->y > 1000.0f - hero->height*hero->scale) {
        hero->y = 1000.0f;
        hero->dy = 0;
    }
    if (hero->x < 0) {
        hero->x = 0;
        hero->dx = 0;
    }
    if (hero->x > 1600 - hero->width) {
        hero->x = 1600;
        hero->dx = 0;
    }

    float potentialNewX = hero->x + hero->dx;
    float potentialNewY = hero->y + hero->dy;

     //проверка коллизии по Y
       if (hero->dy != 0) {
        if (CollisionHappened(hero->x, potentialNewY, hero)) {
            hero->dy = 0;
            hero->isAirborne = FALSE;
            }
        }

        //проверка коллизии по X
    if (hero->dx != 0) {
        if(CollisionHappened(potentialNewX, hero->y, hero))
        {
            hero->dx = 0;
        }
        hero->x += hero->dx;
    }

    if(hero->isAirborne)
    {
        if (hero->currentJumpSpeed > 0 )
        {
            hero->dy = hero->currentJumpSpeed;
            hero->y -= hero->dy;
            hero->currentJumpSpeed += gravity;
        }
        else
        {
            hero->dy -= gravity;
            hero->y += hero->dy;
        }
    }

     // Новая переменная для определения, есть ли блоки вблизи под персонажем
    BOOL hasBlocksBelow = FALSE;

    // Проверяем блоки в нижнем диапазоне под персонажем
    int bottomTileY = round((hero->y + hero->height*hero->scale) / blockSize); // Нижний тайл под персонажем
    int leftTileX = round(hero->x / blockSize); // Левый тайл под персонажем
    int rightTileX = round((hero->x + hero->width * hero->scale) / blockSize); // Правый тайл под персонажем

    for (int j = leftTileX; j <= rightTileX; j++) {
        if (TileMap[bottomTileY][j] == 'B') {
            hasBlocksBelow = TRUE;
            break;
        }
    }

    // Если блоков нет в нижнем диапазоне, персонаж начинает падать
    if (!hasBlocksBelow) {
        hero->isAirborne = TRUE;
    }

        // Проверка коллизии с потолком
    if (hero->dy > 0) { // Проверяем только при подъеме персонажа
        int topTileY = round(hero->y / blockSize); // Верхний тайл над персонажем
        int leftTileX = round(hero->x / blockSize); // Левый тайл над персонажем
        int rightTileX = round((hero->x + hero->width * hero->scale) / blockSize); // Правый тайл над персонажем

        for (int j = leftTileX; j <= rightTileX; j++) {
            if (TileMap[topTileY][j] == 'B') {
                hero->dy = 0;
                hero->y = (topTileY + 1) * blockSize;
                hero->currentJumpSpeed = 0; // Перемещаем персонажа на самый верхний блок
                break;
            }
        }
    }
}

GLuint LoadTexture(const char *filename)
{
    int width, height, cnt;
    unsigned char *image = stbi_load(filename, &width, &height, &cnt, 0);
    if (image == NULL) {-+
        printf("Error in loading the image: %s\n", stbi_failure_reason());
        exit(1);
    }
    printf("Loaded image '%s' with width: %d, height: %d, channels: %d\n", filename, width, height, cnt);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
    return textureID;
}

void DrawBackground(GLuint texture)
{
    // Координаты вершин соответствуют размерам окна
    static float vertices[] = {0.0f, 0.0f,  1920.0f, 0.0f,  1920.0f, 1080.0f,  0.0f, 1080.0f};

    // Координаты текстуры от 0 до 1 для полного покрытия
    static float TexCord[] = {0, 0,  1, 0,  1, 1,  0, 1};

    glClearColor(0, 0, 0, 0); // Задаем цвет очистки экрана, если будет необходимо

    glEnable(GL_TEXTURE_2D); // Включаем 2D текстурирование
    glBindTexture(GL_TEXTURE_2D, texture); // Привязываем текстуру

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, vertices); // Указываем массив вершин
    glTexCoordPointer(2, GL_FLOAT, 0, TexCord); // Указываем массив координат текстуры

    glDrawArrays(GL_QUADS, 0, 4); // Рисуем четырехугольник из 4 вершин

    glDisableClientState(GL_VERTEX_ARRAY); // Отключаем массив вершин
    glDisableClientState(GL_TEXTURE_COORD_ARRAY); // Отключаем массив координат текстуры

    glDisable(GL_TEXTURE_2D); // Отключаем 2D текстурирование
}

// Функция для обновления текущего кадра
void UpdateGeneralAnimationFrame() {

        currentFrame = (currentFrame + 1) % totalFrames; // Циклическое обновление кадра

}

// Функция рендеринга анимации
void RenderSpriteAnimation(GLuint texture, float posX, float posY, float width, float height, float scale, int currentFrame,float frameWidth) {
    float texLeft = currentFrame * frameWidth;
    float texRight = texLeft + frameWidth;

       // Отражаем спрайт по горизонтали, если герой смотрит влево
    if (hero.faceRight) {
        float temp = texRight;
        texRight = texLeft;
        texLeft = temp;
    }

    // Рассчитываем размеры спрайта с учетом масштаба
    float scaledWidth = width * scale;
    float scaledHeight = height * scale;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        glTexCoord2f(texLeft, 0.0f); glVertex2f(posX, posY);                               // Левый верхний угол
        glTexCoord2f(texRight, 0.0f); glVertex2f(posX + scaledWidth, posY);                 // Правый верхний угол
        glTexCoord2f(texRight, 1.0f); glVertex2f(posX + scaledWidth, posY + scaledHeight);  // Правый нижний угол
        glTexCoord2f(texLeft, 1.0f); glVertex2f(posX, posY + scaledHeight);                 // Левый нижний угол
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void Init(HWND hwnd)
{
    Menu_AddButton("Action",10,10,100,30,1.5);
    Menu_AddButton("Stop",10,50,100,30,1.5);
    Menu_AddButton("Exit",10,90,100,30,2);

    Idle_sprite = LoadTexture("idle.png");
    Walk_sprite = LoadTexture("walk.png");
    Jump_sprite = LoadTexture("jump.png");
    background = LoadTexture("Background.jpg");
    brickTile = LoadTexture("brick.png");

    RECT rct;
    GetClientRect(hwnd, &rct);

    // Инициализация позиции героя
    hero.x = 500;  // Начальная позиция по X
    hero.y = 600;  // Начальная позиция по Y
    hero.isAirborne = TRUE;
    hero.faceRight = TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          WINDOW_WIDTH,
                          WINDOW_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);
    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    RECT rct; //создание переменной с координатами прямоуголника


    glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 1, -1);
            GetClientRect(hwnd, &rct);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();


    Init(hwnd);
    DWORD lastUpdateTime = GetTickCount();
    /* program main loop */
    while (!bQuit)
    {
          /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */


            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            DrawBackground(background);
            Menu_ShowMenu();

            if (button1)
            {
                DrawMap();
                Menu_ShowMenu();
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                UpdateHeroPositionAndCollisions(&hero);

                if (!hero.isAirborne && !hero.isMoving)
                {
                    glPushMatrix();
                    UpdateGeneralAnimationFrame();
                    RenderSpriteAnimation(Idle_sprite, hero.x, hero.y, hero.width, hero.height, hero.scale, currentFrame, frameWidth);
                    glPopMatrix();
                }

                else if (hero.isAirborne)
                    {
                        glPushMatrix();
                        UpdateGeneralAnimationFrame();
                        RenderSpriteAnimation(Jump_sprite, hero.x, hero.y, hero.width, hero.height, hero.scale, currentFrame, frameWidth);
                        glPopMatrix();
                }

                else if (hero.isMoving)
                {
                    glPushMatrix();
                    UpdateGeneralAnimationFrame();
                    RenderSpriteAnimation(Walk_sprite, hero.x, hero.y, hero.width, hero.height, hero.scale, currentFrame, frameWidth);
                    glPopMatrix();
                }

            }
            glDisable(GL_BLEND);
            SwapBuffers(hDC);
            Sleep (80);
        }

    }


    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_MOUSEMOVE:
            Menu_MouseMove(LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_LBUTTONDOWN:
            {
                int buttonId = Menu_MouseDown();
                if (buttonId == 0)
                {
                    button1 = 1;
                    button2 = 0;
                    printf("Button Sprite_1 pressed. State: %d\n", button1);
                }
                else if (buttonId == 1)
                {
                    button1 = 0;
                    button2 = 1;
                    printf("Button Sprite_2 pressed. State: %d\n", button2);
                }
                else if (buttonId == 2)
                    PostQuitMessage(0);
            }
            break;

        case WM_LBUTTONUP:
            Menu_MouseUp();
            break;

        case WM_KEYDOWN:
            switch(wParam) {
                case VK_LEFT:
                    hero.dx = -15.0f;
                    hero.isMoving = TRUE;
                    hero.faceRight = FALSE; // Герой смотрит влево
                    break;
                case VK_RIGHT:
                    hero.dx = 15.0f;
                    hero.isMoving = TRUE;
                    hero.faceRight = TRUE;
                    break;
                case VK_UP:
                case VK_SPACE:
                    if (!hero.isAirborne)
                    {
                        hero.isAirborne = TRUE;
                        hero.currentJumpSpeed = jumpSpeed;
                    }
                    break;
            }
            break;

        case WM_KEYUP:
            switch(wParam) {
                case VK_LEFT:
                case VK_RIGHT:
                    hero.dx = 0.0f;
                    hero.isMoving = FALSE;
                    break;
            }
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
