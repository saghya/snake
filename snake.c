/*
TODO:- fix random die
     - direction on spawn
*/

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_LINES     7
#define MIN_COLS      15
#define STARTING_SIZE 5
#define MAX_SIZE      1024
#define TIMEOUT       250

enum direction { up, down, left, right };

typedef struct pos {
    int x, y;
} pos;

typedef struct snake {
    pos pos[MAX_SIZE];
    int dir;
    int len;
} snake;

static const char welcome[]   = "press any key to start";
static const char game_over[] = "game over";
static const char restart[]   = "press r to restart";
static const char win[]       = "you won";
static const char paused[]    = "paused";
static snake      snk;
static pos        food;

void init()
{
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    srand(time(NULL));

    box(stdscr, ACS_VLINE, ACS_HLINE);
    mvprintw(LINES / 2 - 1, (COLS - strlen(welcome)) / 2, welcome);
    getch();
    clear();
    box(stdscr, ACS_VLINE, ACS_HLINE);
    timeout(TIMEOUT);
}

void print_food()
{
    food.x = rand() % (COLS - 2) + 1;
    food.y = rand() % (LINES - 2) + 1;
    for (int i = 0; i < snk.len; i++) {
        if (snk.pos[i].x == food.x && snk.pos[i].y == food.y) {
            food.x = rand() % (COLS - 2) + 1;
            food.y = rand() % (LINES - 2) + 1;
            i      = 0;
        }
    }
    mvaddch(food.y, food.x, 'X');
}

void setup()
{
    int x, y;
    snk.len = STARTING_SIZE;
    x       = rand() % (COLS - COLS / 5) + COLS / 10;
    y       = rand() % (LINES - LINES / 5) + LINES / 10;
    for (int i = 0; i < snk.len; i++) {
        snk.pos[i].x = ++x;
        snk.pos[i].y = y;
    }
    snk.dir = rand() % 4;
    snk.dir = left;
    print_food();
}

void print_lose()
{
    mvprintw(LINES / 2 - 2, (COLS - strlen(game_over)) / 2, game_over);
    mvprintw(LINES / 2 - 1, (COLS - 10) / 2, "score: %2d",
             snk.len - STARTING_SIZE);
    mvprintw(LINES / 2 + 1, (COLS - strlen(restart)) / 2, restart);
}

void print_win()
{

    mvprintw(LINES / 2 - 2, (COLS - strlen(win)) / 2, win);
    mvprintw(LINES / 2 - 1, (COLS - 10) / 2, "score: %2d",
             snk.len - STARTING_SIZE);
    mvprintw(LINES / 2 + 1, (COLS - strlen(restart)) / 2, restart);
}

void game_loop()
{
    int over = 0, pause = 0;

    for (int ch = getch(); ch != 27 && ch != 'q'; ch = getch()) {
        if (LINES < MIN_LINES || COLS < MIN_COLS) {
            mvprintw(0, 0, "Please resize your window and press r");
            over = 1;
        }
        switch (ch) {
        case ' ':
            if (over)
                continue;
            if (pause) {
                pause = 0;
                timeout(TIMEOUT);

                // delete pause label. TODO: maybe use windows for labels
                clear();
                box(stdscr, ACS_VLINE, ACS_HLINE);
                mvaddch(food.y, food.x, 'X');
            } else {
                pause = 1;
                timeout(-1);
                mvprintw(LINES / 2 - 1, (COLS - strlen(paused)) / 2, paused);
            }
            break;
        case 'w':
        case 'k':
        case KEY_UP:
            //--y;
            if (snk.dir != down)
                snk.dir = up;
            break;
        case 'a':
        case 'h':
        case KEY_LEFT:
            //--x;
            if (snk.dir != right)
                snk.dir = left;
            break;
        case 's':
        case 'j':
        case KEY_DOWN:
            //++y;
            if (snk.dir != up)
                snk.dir = down;
            break;
        case 'd':
        case 'l':
        case KEY_RIGHT:
            //++x;
            if (snk.dir != left)
                snk.dir = right;
            break;
        case 'r':
            if (!over)
                break;
            clear();
            box(stdscr, ACS_VLINE, ACS_HLINE);
            setup();
            over = 0;
            break;
        }
        if (over || pause)
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

        // SCORE
        // mvprintw(1, 1, "score: %2d", snk.len - STARTING_SIZE);

        // PRINT
        box(stdscr, ACS_VLINE, ACS_HLINE); // TODO: fix corner
        if (snk.pos[0].x == snk.pos[1].x)
            mvaddch(snk.pos[0].y, snk.pos[0].x, '|');
        else
            mvaddch(snk.pos[0].y, snk.pos[0].x, '-');
        for (int i = 1; i < snk.len - 1; i++) {
            if (snk.pos[i - 1].x == snk.pos[i].x &&
                snk.pos[i].x == snk.pos[i + 1].x)
                mvaddch(snk.pos[i].y, snk.pos[i].x, '|');
            else if (snk.pos[i - 1].y == snk.pos[i].y &&
                     snk.pos[i].y == snk.pos[i + 1].y)
                mvaddch(snk.pos[i].y, snk.pos[i].x, '-');
            else
                mvaddch(snk.pos[i].y, snk.pos[i].x, '+');
        }
        if (snk.pos[snk.len - 2].x == snk.pos[snk.len - 1].x)
            mvaddch(snk.pos[snk.len - 1].y, snk.pos[snk.len - 1].x, '|');
        else
            mvaddch(snk.pos[snk.len - 1].y, snk.pos[snk.len - 1].x, '-');

        // FOOD COLLISION
        if (snk.pos[0].x == food.x && snk.pos[0].y == food.y) {
            ++snk.len;
            print_food();
        }

        // CRASH
        if (!snk.pos[0].x || !snk.pos[0].y || snk.pos[0].x == COLS - 1 ||
            snk.pos[0].y == LINES - 1) {
            print_lose();
            over = 1;
        }
        for (int i = 0; i < snk.len - 1; i++) {
            for (int j = i + 1; j < snk.len; j++) {
                if (snk.pos[i].x == snk.pos[j].x &&
                    snk.pos[i].y == snk.pos[j].y) {
                    print_lose();
                    over = 1;
                }
            }
        }

        // WIN
        if (snk.len == MAX_SIZE || snk.len == (LINES - 2) * (COLS - 2)) {
            print_win();
            over = 1;
        }
    }
}

int main()
{
    init();
    setup();
    game_loop();

    endwin();

    return 0;
}
