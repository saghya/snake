/*
TODO:- direction on spawn
     - disable opposite key to direction
     - fix terminal after exit
     - DRY
*/

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STARTING_LENGTH 5
#define MAX_SIZE        99

enum direction { up, down, left, right };
static const char welcome[]   = "press any key to start";
static const char game_over[] = "game over";
static const char restart[]   = "press r to restart";
static const char win[]       = "you won";

typedef struct pos {
    int x, y;
} pos;

typedef struct snake {
    pos pos[MAX_SIZE];
    int dir;
    int len;
} snake;

void init()
{
    initscr();
    curs_set(0);
    raw();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    srand(time(NULL));

    box(stdscr, ACS_VLINE, ACS_HLINE);
    mvprintw(LINES / 2 - 1, (COLS - strlen(welcome)) / 2, welcome);
    getch();
    clear();
    box(stdscr, ACS_VLINE, ACS_HLINE);
    timeout(250);
}

void game_loop()
{
    int   x, y, over = 0;
    snake snk;
    pos   food;
    snk.len = STARTING_LENGTH;
    x       = rand() % (COLS - COLS / 5) + COLS / 10;
    y       = rand() % (LINES - LINES / 5) + LINES / 10;
    for (int i = MAX_SIZE; i > MAX_SIZE - snk.len; i--) {
        snk.pos[i].x = -1;
        snk.pos[i].y = -1;
    }
    for (int i = 0; i < snk.len; i++) {
        snk.pos[i].x = ++x;
        snk.pos[i].y = y;
    }
    snk.dir = rand() % 4;
    snk.dir = left;
    food.x  = rand() % (COLS - 2) + 1;
    food.y  = rand() % (LINES - 2) + 1;
    mvaddch(food.y, food.x, 'X');

    for (int ch = getch(); ch != 27 && ch != 'q'; ch = getch()) {
        switch (ch) {
        case 'w':
        case 'k':
        case KEY_UP:
            //--y;
            snk.dir = up;
            break;
        case 'a':
        case 'h':
        case KEY_LEFT:
            //--x;
            snk.dir = left;
            break;
        case 's':
        case 'j':
        case KEY_DOWN:
            //++y;
            snk.dir = down;
            break;
        case 'd':
        case 'l':
        case KEY_RIGHT:
            //++x;
            snk.dir = right;
            break;
        case 'r':
            if (!over)
                break;
            clear();
            box(stdscr, ACS_VLINE, ACS_HLINE);
            snk.len = STARTING_LENGTH;
            x       = rand() % (COLS - COLS / 5) + COLS / 10;
            y       = rand() % (LINES - LINES / 5) + LINES / 10;
            for (int i = 0; i < snk.len; i++) {
                snk.pos[i].x = ++x;
                snk.pos[i].y = y;
            }
            snk.dir = rand() % 4;
            snk.dir = left;
            food.x  = rand() % (COLS - 2) + 1;
            food.y  = rand() % (LINES - 2) + 1;
            mvaddch(food.y, food.x, 'X');
            over = 0;
            break;
        }
        if (over)
            continue;

        mvaddch(snk.pos[snk.len - 1].y, snk.pos[snk.len - 1].x, ' ');

        // STEP
        for (int i = snk.len - 1; i > 0; i--) {
            snk.pos[i].x = snk.pos[i - 1].x;
            snk.pos[i].y = snk.pos[i - 1].y;
        }

        switch (snk.dir) {
        case up:
            --snk.pos[0].y;
            break;
        case left:
            --snk.pos[0].x;
            break;
        case down:
            ++snk.pos[0].y;
            break;
        case right:
            ++snk.pos[0].x;
            break;
        }

        for (int i = 0; i < snk.len; i++) {
            mvaddch(snk.pos[i].y, snk.pos[i].x, '+');
        }

        // FOOD COLLISION
        if (snk.pos[0].x == food.x && snk.pos[0].y == food.y) {
            ++snk.len;
            food.x = rand() % (COLS - 2) + 1;
            food.y = rand() % (LINES - 2) + 1;
            mvaddch(food.y, food.x, 'X');
        }

        // CRASH
        if (!snk.pos[0].x || !snk.pos[0].y || snk.pos[0].x == COLS - 1 ||
            snk.pos[0].y == LINES - 1) {
            mvprintw(LINES / 2 - 2, (COLS - strlen(game_over)) / 2, game_over);
            mvprintw(LINES / 2 - 1, (COLS - 10) / 2, "score: %2d", snk.len);
            mvprintw(LINES / 2 + 1, (COLS - strlen(restart)) / 2, restart);
            over = 1;
        }
        // WIN
        if (snk.len == MAX_SIZE || snk.len == (LINES - 2) * (COLS - 2)) {
            mvprintw(LINES / 2 - 2, (COLS - strlen(win)) / 2, win);
            mvprintw(LINES / 2 - 1, (COLS - 10) / 2, "score: %3d", snk.len);
            mvprintw(LINES / 2 + 1, (COLS - strlen(restart)) / 2, restart);
            over = 1;
        }
        for (int i = 0; i < snk.len - 1; i++) {
            for (int j = i + 1; j < snk.len; j++) {
                if (snk.pos[i].x == snk.pos[j].x &&
                    snk.pos[i].y == snk.pos[j].y) {
                    mvprintw(LINES / 2 - 2, (COLS - strlen(game_over)) / 2,
                             game_over);
                    mvprintw(LINES / 2 - 1, (COLS - 10) / 2, "score: %2d",
                             snk.len);
                    mvprintw(LINES / 2 + 1, (COLS - strlen(restart)) / 2,
                             restart);
                    over = 1;
                }
            }
        }
        box(stdscr, ACS_VLINE, ACS_HLINE); // TODO: better fix for border
    }
}

int main()
{
    init();

    game_loop();

    endwin();
    nocbreak();
    return 0;
}
