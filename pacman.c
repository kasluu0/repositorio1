#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define PACMAN_CHAR 'C'
#define GHOST_CHAR 'G'
#define WALL_CHAR '#'
#define PELLET_CHAR '.'
#define POWER_CHAR 'o'
#define EMPTY_CHAR ' '
#define WIDTH 28
#define HEIGHT 20
#define NUM_GHOSTS 4
#define POWER_DURATION 30

HANDLE hConsole;

typedef struct {
    int x, y;
} Entity;

Entity pacman;
Entity ghosts[NUM_GHOSTS];
int score = 0;
int lives = 3;
int powerMode = 0;
int pelletsLeft = 0;
char map[HEIGHT][WIDTH + 1];

// ---- Mapa simples ----
char level[HEIGHT][WIDTH + 1] = {
    "############################",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#o####.#####.##.#####.####o#",
    "#.####.#####.##.#####.####.#",
    "#..........................#",
    "#.####.##.########.##.####.#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "######.##### ## #####.######",
    "     #.##### ## #####.#     ",
    "######.##          ##.######",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#o..##................##..o#",
    "###.##.##.########.##.##.###",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################"
};

void setColor(int color) {
    SetConsoleTextAttribute(hConsole, color);
}

void resetGame() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            map[y][x] = level[y][x];
            if (map[y][x] == PELLET_CHAR || map[y][x] == POWER_CHAR)
                pelletsLeft++;
        }
    }
    pacman.x = 14;
    pacman.y = 17;
    ghosts[0].x = 13; ghosts[0].y = 10;
    ghosts[1].x = 14; ghosts[1].y = 10;
    ghosts[2].x = 12; ghosts[2].y = 10;
    ghosts[3].x = 15; ghosts[3].y = 10;
    score = 0;
    lives = 3;
    powerMode = 0;
}

void drawMap() {
    system("cls");
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char c = map[y][x];
            if (x == pacman.x && y == pacman.y) {
                setColor(14); // Amarelo
                printf("%c", PACMAN_CHAR);
            } else {
                int ghostHere = 0;
                for (int g = 0; g < NUM_GHOSTS; g++) {
                    if (ghosts[g].x == x && ghosts[g].y == y) {
                        setColor(g + 9); // Diferentes cores
                        printf("%c", GHOST_CHAR);
                        ghostHere = 1;
                        break;
                    }
                }
                if (!ghostHere) {
                    if (c == WALL_CHAR) { setColor(11); } // Azul claro
                    else if (c == PELLET_CHAR) { setColor(15); } // Branco
                    else if (c == POWER_CHAR) { setColor(11); } // Ciano
                    else { setColor(7); }
                    printf("%c", c);
                }
            }
        }
        printf("\n");
    }
    setColor(7);
    printf("Score: %d  Lives: %d  %s\n", score, lives, powerMode ? "POWER!" : "");
}

void moveEntity(Entity *e, int dx, int dy) {
    int nx = e->x + dx;
    int ny = e->y + dy;
    if (map[ny][nx] != WALL_CHAR) {
        e->x = nx;
        e->y = ny;
    }
}

void updateGhosts() {
    for (int g = 0; g < NUM_GHOSTS; g++) {
        int dir = rand() % 4;
        if (dir == 0) moveEntity(&ghosts[g], 1, 0);
        else if (dir == 1) moveEntity(&ghosts[g], -1, 0);
        else if (dir == 2) moveEntity(&ghosts[g], 0, 1);
        else moveEntity(&ghosts[g], 0, -1);
    }
}

void checkCollisions() {
    char c = map[pacman.y][pacman.x];
    if (c == PELLET_CHAR) {
        score += 10;
        map[pacman.y][pacman.x] = EMPTY_CHAR;
        pelletsLeft--;
    } else if (c == POWER_CHAR) {
        score += 50;
        map[pacman.y][pacman.x] = EMPTY_CHAR;
        powerMode = POWER_DURATION;
        pelletsLeft--;
    }

    for (int g = 0; g < NUM_GHOSTS; g++) {
        if (pacman.x == ghosts[g].x && pacman.y == ghosts[g].y) {
            if (powerMode > 0) {
                score += 200;
                ghosts[g].x = 14;
                ghosts[g].y = 10;
            } else {
                lives--;
                pacman.x = 14;
                pacman.y = 17;
                if (lives <= 0) {
                    system("cls");
                    printf("GAME OVER! Score: %d\n", score);
                    system("pause");
                    exit(0);
                }
            }
        }
    }
    if (pelletsLeft <= 0) {
        system("cls");
        printf("YOU WIN! Score: %d\n", score);
        system("pause");
        exit(0);
    }
}

void gameLoop() {
    while (1) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 'w') moveEntity(&pacman, 0, -1);
            if (key == 's') moveEntity(&pacman, 0, 1);
            if (key == 'a') moveEntity(&pacman, -1, 0);
            if (key == 'd') moveEntity(&pacman, 1, 0);
        }
        updateGhosts();
        checkCollisions();
        drawMap();
        if (powerMode > 0) powerMode--;
        Sleep(200);
    }
}

int menu() {
    int option = 0;
    while (1) {
        system("cls");
        printf("===== PAC-MAN =====\n");
        printf("%s Start Game\n", option == 0 ? ">" : " ");
        printf("%s Exit\n", option == 1 ? ">" : " ");

        char key = _getch();
        if (key == 'w' && option > 0) option--;
        if (key == 's' && option < 1) option++;
        if (key == '\r') return option;
    }
}

int main() {
    srand(time(NULL));
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    while (1) {
        int choice = menu();
        if (choice == 1) break;
        resetGame();
        gameLoop();
    }
    return 0;
}