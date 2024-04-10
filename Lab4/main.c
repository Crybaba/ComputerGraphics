#include <windows.h>
#include <gl/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include "stb-master/stb_easy_font.h"
#include "menu.h"

#define H 27 // Высота карты
#define W 48 // Ширина карты

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920

#include <float.h>


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
int button1 = 0;
int button2 = 0;

GLuint Idle_sprite, Walk_sprite, Jump_sprite, background, brickTile, groundTile;
int currentFrame = 0;          // Текущий кадр анимации
const int totalFrames = 7;     // Всего кадров в спрайт-листе
float frameWidth = 1.0f / 7.0f;  // Ширина одного кадра в текстурных координатах
int isMoving = 0;
float jumpSpeed = 40.0f; // Начальная скорость прыжка
float gravity = -5.0f; // Ускорение, действующее на персонажа при падении
float verticalVelocity = 0.0f; // Вертикальная скорость персонажа
float maxYVelocity = 40.0f;
BOOL isJumping = FALSE; // Находится ли персонаж в прыжке
float groundLevel = 0.0f; // Уровень "земли", ниже которого персонаж не может опуститься
BOOL isAirborne = TRUE;  // Переменная для проверки, находится ли персонаж в воздухе
int jumpFrame = 0;
int jumpAnimationPlaying = 0;
const int jumpFramesCount = 80;
float jumpAnimationDuration = 0.0f;
float totalJumpDuration;
const float blockSize = 40.0f;


char TileMap[H][W] = {
    "BGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGB",
    "B                                              B",
    "B                                              B",
    "B                                              B",
    "B                                              B",
    "B                                              B",
    "B                                              B",
    "BGGGGGGGGGGGGGGGGGGGGGGGGGGGG                  B",
    "B                                              B",
    "B                                              B",
    "B                                              B",
    "B                                              B",
    "B                                              B",
    "B                         GGGGGGGGGGGGGGGGGGGGGB",
    "B                                              B",
    "B                                              B",
    "B                                              B",
    "B           GGGGGGGGGGGGGG                     B",
    "B                                              B",
    "B                                              B",
    "BGGGG                                          B",
    "BGGGGGG                                        B",
    "BGGGGGGGG                                      B",
    "BGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGB",
    "BGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGB",
    "BGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGB",
    "BGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGB",
};

typedef struct {
    float x, y;          // Позиция
    float dx, dy;        // Скорость
    BOOL isAirborne;     // Находится ли в воздухе
    BOOL isMoving;       // Двигается ли
    float width, height; // Размеры героя
    BOOL jumpPeakReached;// Достигнут ли пик прыжка
} Hero;

Hero hero = { .x = 0.0f, .y = 0.0f, .dx = 0.0f, .dy = 0.0f, .isAirborne = TRUE, .isMoving = FALSE, .width = 80.0f, .height = 80.0f };

//КОЛИЗИЯ

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
                case 'G': // Земля
                    textureID =  groundTile; // Чередование текстур
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

BOOL CheckCollisionWithMap(float newX, float newY, Hero *hero, BOOL* isWallHit) {
    *isWallHit = FALSE; // Сбрасываем флаг столкновения со стеной
    // Проверка на столкновение с тайлами карты
    for (int y = (int)(newY / blockSize); y < (int)((newY + hero->height) / blockSize); y++) {
        for (int x = (int)(newX / blockSize); x < (int)((newX + hero->width) / blockSize); x++) {
            char tile = TileMap[y][x];
            if (tile == 'B') {
                *isWallHit = TRUE; // Устанавливаем флаг столкновения со стеной, если касаемся тайла 'X'
                return TRUE; // Обнаружено столкновение
            } else if (tile == 'G' || tile == 'B'){
                return TRUE; // Обнаружено столкновение с другими непроходимыми тайлами
            }
        }
    }
    return FALSE; // Столкновений не обнаружено
}

BOOL isAtSolidTile(float x, float y) {
    // Преобразуем координаты в индексы массива TileMap
    int tileX = (int)(x / blockSize);
    int tileY = (int)(y / blockSize);

    // Проверяем, не выходят ли индексы за пределы массива
    if (tileX < 0 || tileX >= W || tileY < 0 || tileY >= H) {
        return FALSE; // Возвращаем false, если координаты вне диапазона карты
    }

    // Получаем символ тайла по индексам
    char tile = TileMap[tileY][tileX];

    // Возвращаем true, если тайл является непроходимым
    return (tile == 'B' || tile == 'G');
}

void UpdateGroundLevelForHero(Hero* hero) {
    float nearestGround = FLT_MAX;
    BOOL groundFound = FALSE;

    // Начинаем проверку с позиции героя по X и проверяем только тайлы прямо под ним
    int tileXStart = (int)(hero->x / blockSize);
    int tileXEnd = (int)((hero->x + hero->width) / blockSize);

    // Ищем ближайший непроходимый тайл под героем
    for (int x = tileXStart; x <= tileXEnd; x++) {
        for (int y = (int)(hero->y / blockSize) + 1; y < H; y++) {
            if (isAtSolidTile(x * blockSize, y * blockSize)) {
                float groundY = y * blockSize - hero->height;
                if (groundY < nearestGround) {
                    nearestGround = groundY;
                    groundFound = TRUE;
                }
                break; // Переходим к следующему столбцу, как только найдем землю
            }
        }
    }

    if (groundFound) {
        groundLevel = nearestGround;
    } else {
        // Если под героем нет земли, мы можем установить groundLevel
        // в значение, которое обозначает "падение" или вернуть его на дефолтный уровень
        groundLevel = FLT_MAX; // Означает отсутствие земли под героем
    }
}

// Функция обновления позиции персонажа и коллизий
void UpdateHeroPositionAndCollisions(Hero *hero) {
    // Обновляем уровень земли для героя
    UpdateGroundLevelForHero(hero);

    // Предполагаемая новая позиция героя
    float potentialNewX = hero->x + hero->dx;
    float potentialNewY = hero->y + hero->dy;
    BOOL isWallHit = FALSE; // Флаг столкновения со стеной

    // Проверка на столкновение и обновление позиции по X
    if (!CheckCollisionWithMap(potentialNewX, hero->y, hero, &isWallHit)) {
        hero->x = potentialNewX;
    } else {
        if (isWallHit) { // Если столкновение со стеной
            hero->dx = 0; // Останавливаем движение
        }
    }

    // Проверка на столкновение и обновление позиции по Y
    if (!CheckCollisionWithMap(hero->x, potentialNewY, hero, &isWallHit)) {
        hero->y = potentialNewY;
    } else {
        if (isWallHit) {
            if (hero->dy > 0) { // Если персонаж двигался вверх
                hero->dy = 0; // Останавливаем вертикальное движение
            } else {
                hero->y = (int)(hero->y / blockSize) * blockSize; // Корректируем позицию на уровень тайла
                hero->dy = 0; // Обнуляем вертикальную скорость после столкновения с землей
            }
        }
    }

    // Применяем гравитацию
    hero->dy -= gravity;

    // Ограничение вертикальной скорости
    if (hero->dy > maxYVelocity) {
        hero->dy = maxYVelocity;
    } else if (hero->dy < -maxYVelocity) {
        hero->dy = -maxYVelocity;
    }

    // Проверка нахождения героя на земле
    if (hero->y >= groundLevel) {
        hero->y = groundLevel; // Коррекция позиции на уровень земли
        hero->isAirborne = FALSE;
        hero->dy = 0; // Прекращаем вертикальное движение
    } else {
        hero->isAirborne = TRUE;
    }
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
    groundTile = LoadTexture("ground.png");
    brickTile = LoadTexture("brick.png");

    hero.dy = 0.0f;

    RECT rct;
    GetClientRect(hwnd, &rct);

    groundLevel = rct.bottom - 300;
    // Инициализация позиции героя
    hero.x = 400.0f;  // Начальная позиция по X
    hero.y = groundLevel;  // Начальная позиция по Y
    hero.isAirborne = FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    glViewport(0, 0, 1000, 1000);
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
            GetClientRect(hwnd, &rct);
            glOrtho(0, rct.right, rct.bottom, 0, 1, -1);
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

            float centerX = rct.right / 2.0f;
            float posY = 150.0f;
            float spriteWidth = 770.0f; // ширина текстуры
            float spriteHeight = 80.0f; // высота текстуры

            if (button1)
            {
                DrawMap();
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                float spriteAspectRatio = (float)110 / (float)80;
                float renderedSpriteWidth = spriteHeight * spriteAspectRatio;
                UpdateHeroPositionAndCollisions(&hero);
                float scale = 1.0f; // Уменьшаем масштаб спрайта

                if (!isAirborne && !isMoving)
                {
                    glPushMatrix();
                    UpdateGeneralAnimationFrame();
                    RenderSpriteAnimation(Idle_sprite, hero.x, hero.y, renderedSpriteWidth, spriteHeight, scale, currentFrame, frameWidth);
                    glPopMatrix();
                }

                else if (isAirborne)
                    {
                        glPushMatrix();

                        UpdateGeneralAnimationFrame();
                        RenderSpriteAnimation(Jump_sprite, hero.x, hero.y, renderedSpriteWidth, spriteHeight, scale, currentFrame, frameWidth);
                        hero.y -= verticalVelocity; // Учитываем гравитацию
                        verticalVelocity += gravity; // Обновляем вертикальную скорость
                        hero.x += hero.dx; // Обновляем координату X

                        hero.y -= verticalVelocity; // Вычитаем, потому что движемся вверх
                        verticalVelocity += gravity; // Добавляем гравитацию

                    if (hero.y >= groundLevel) {
                        hero.y = groundLevel;
                        isAirborne = FALSE;
                        verticalVelocity = 0;
                        }
                    glPopMatrix();
                }
                //Collision(&hero);
                else if (isMoving)
                {
                    glPushMatrix();
                    UpdateGeneralAnimationFrame();
                    hero.x += hero.dx;
                    hero.y += hero.dy;
                    if (hero.x < 0) hero.x = 0;
                    if (hero.x > rct.right - renderedSpriteWidth) hero.x = rct.right - renderedSpriteWidth;
                    RenderSpriteAnimation(Walk_sprite, hero.x, hero.y, renderedSpriteWidth, spriteHeight, scale, currentFrame, frameWidth);
                    glPopMatrix();
                }
                Menu_ShowMenu();

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
                    isMoving = 1;
                    break;
                case VK_RIGHT:
                    hero.dx = 15.0f;
                    isMoving = 1;
                    break;
                case VK_UP:
                case VK_SPACE:
                    if (!isAirborne)
                    {
                        isAirborne = TRUE;
                        verticalVelocity = jumpSpeed;
                    }
                    break;
            }
            break;

        case WM_KEYUP:
            switch(wParam) {
                case VK_LEFT:
                case VK_RIGHT:
                    hero.dx = 0.0f;
                    isMoving = 0;
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
