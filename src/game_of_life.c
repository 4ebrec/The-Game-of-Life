#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25
#define INITIAL_SPEED 250000

void main_function_game();
void input(char a[HEIGHT][WIDTH], int *error_flag);
void init(char array[HEIGHT][WIDTH]);
void nextgen(char array[HEIGHT][WIDTH]);
void updateCell(char array[HEIGHT][WIDTH], char newArray[HEIGHT][WIDTH], int x, int y);
int countNeighbours(char array[HEIGHT][WIDTH], int x, int y);
int get_amount(char array[HEIGHT][WIDTH]);
int is_alive(char a[HEIGHT][WIDTH]);
void output(char a[HEIGHT][WIDTH], int age, int speed);
void info(int age, int speed, int hash);
void inChar(int *speed, int *end);
void endgame();

int main() {
    main_function_game();
    return 0;
}

void main_function_game() {
    setlocale(LC_ALL, "");
    char array[HEIGHT][WIDTH];
    int error_flag = 0;

    input(array, &error_flag);

    if (error_flag == 0) {
        if (freopen("/dev/tty", "r", stdin) != NULL) {
            init(array);
        }
    } else {
        if (error_flag == 2) {
            printf("Неверный символ в файле!\n");
        }
    }
}

void input(char a[HEIGHT][WIDTH], int *error_flag) {
    for (int i = 0; i < HEIGHT && *error_flag == 0; i++) {
        for (int j = 0; j < WIDTH && *error_flag == 0; j++) {
            char ch;
            if (scanf(" %c", &ch) != 1 || (ch != '*' && ch != '-')) {
                *error_flag = 2;
            } else {
                a[i][j] = ch;
            }
        }
    }
}

void init(char array[HEIGHT][WIDTH]) {
    int age = 0, speed = 5, end = 0, gen_cnt = 0;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    while (end == 0) {
        int amount_prev = get_amount(array);
        nextgen(array);
        int amount_now = get_amount(array);

        if (amount_prev == amount_now) {
            gen_cnt++;
        } else {
            gen_cnt = 0;
        }

        if (gen_cnt == 128 || !is_alive(array)) {
            end = 1;
        }

        output(array, age, speed);

        for (int i = 0; i < 10 && end == 0; i++) {
            inChar(&speed, &end);
            usleep(INITIAL_SPEED / (speed * 10));
        }

        if (end == 0) {
            age++;
        }
    }
    endgame();
}

void nextgen(char array[HEIGHT][WIDTH]) {
    char newArray[HEIGHT][WIDTH];

    for (int x = 0; x < HEIGHT; x++) {
        for (int y = 0; y < WIDTH; y++) {
            updateCell(array, newArray, x, y);
        }
    }

    for (int x = 0; x < HEIGHT; x++) {
        for (int y = 0; y < WIDTH; y++) {
            array[x][y] = newArray[x][y];
        }
    }
}

void updateCell(char array[HEIGHT][WIDTH], char newArray[HEIGHT][WIDTH], int x, int y) {
    int neighbours = countNeighbours(array, x, y);
    if (array[x][y] == '*') {
        if (neighbours == 2 || neighbours == 3) {
            newArray[x][y] = '*';
        } else {
            newArray[x][y] = '-';
        }
    } else {
        if (neighbours == 3) {
            newArray[x][y] = '*';
        } else {
            newArray[x][y] = '-';
        }
    }
}

int countNeighbours(char array[HEIGHT][WIDTH], int x, int y) {
    int cnt = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx != 0 || dy != 0) {
                int nx = (x + dx + HEIGHT) % HEIGHT;
                int ny = (y + dy + WIDTH) % WIDTH;
                if (array[nx][ny] == '*') {
                    cnt++;
                }
            }
        }
    }
    return cnt;
}

int get_amount(char array[HEIGHT][WIDTH]) {
    int hash = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (array[i][j] == '*') {
                hash++;
            }
        }
    }
    return hash;
}

int is_alive(char a[HEIGHT][WIDTH]) {
    int alive = 0;
    for (int i = 0; i < HEIGHT && alive == 0; i++) {
        for (int j = 0; j < WIDTH && alive == 0; j++) {
            if (a[i][j] == '*') {
                alive = 1;
            }
        }
    }
    return alive;
}

void output(char a[HEIGHT][WIDTH], int age, int speed) {
    clear();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            addch(a[i][j]);
        }
        addch('\n');
    }
    info(age, speed, get_amount(a));
    refresh();
}

void info(int age, int speed, int hash) {
    mvprintw(0, WIDTH + 2, "Поколение: %d", age);
    mvprintw(1, WIDTH + 2, "Скорость: %d", speed);
    mvprintw(2, WIDTH + 2, "Живых клеток: %d", hash);
    mvprintw(HEIGHT + 1, 0, "Управление: 'A' - быстрее, 'Z' - медленнее");
    mvprintw(HEIGHT + 1, WIDTH - 20, "Выход: Space Bar");
}

void inChar(int *speed, int *end) {
    int ch = getch();
    if (ch != ERR) {
        if (ch == 'a' || ch == 'A') {
            (*speed)++;
        } else if (ch == 'z' || ch == 'Z') {
            (*speed)--;
        } else if (ch == 32) {
            *end = 1;
        }
    }

    if (*speed > 20) {
        *speed = 20;
    }
    if (*speed < 1) {
        *speed = 1;
    }
}

void endgame() {
    clear();
    mvprintw(HEIGHT / 2, (WIDTH / 2) - 5, "GAME OVER!");
    refresh();
    sleep(2);
    endwin();
}