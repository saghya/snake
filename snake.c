/*
TODO:- fix random die
     - splash screen
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
static int        colors;

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

    colors = has_colors();
    if (colors) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_BLUE, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(5, COLOR_WHITE, COLOR_BLACK);
        init_pair(6, COLOR_YELLOW, COLOR_BLACK);
        init_pair(7, COLOR_CYAN, COLOR_BLACK);
        init_pair(8, COLOR_RED, COLOR_BLACK);
    }
}

void print_snake()
{
    attron(A_BOLD);
    if (colors)
        attron(COLOR_PAIR(1));
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
    attroff(A_BOLD);
    if (colors)
        attroff(COLOR_PAIR(1));
}

void print_food()
{
    int color;
    attron(A_BOLD);
    if (colors) {
        color = rand() % 8 + 1;
        attron(COLOR_PAIR(color));
    }
    for (int i = 0; i < snk.len; i++) {
        if (snk.pos[i].x == food.x && snk.pos[i].y == food.y) {
            food.x = rand() % (COLS - 2) + 1;
            food.y = rand() % (LINES - 2) + 1;
            i      = 0;
        }
    }
    // random printable ascii character
    mvaddch(food.y, food.x, rand() % (126 - 33) + 33);
    if (colors)
        attroff(COLOR_PAIR(color));
    attroff(A_BOLD);
}

void print_lose()
{
    if (colors)
        attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(LINES / 2 - 2, (COLS - strlen(game_over)) / 2, game_over);
    if (colors)
        attroff(COLOR_PAIR(3) | A_BOLD);
    mvprintw(LINES / 2 - 1, (COLS - 10) / 2, "score: %2d",
             snk.len - STARTING_SIZE);
    mvprintw(LINES / 2 + 1, (COLS - strlen(restart)) / 2, restart);
}

void print_win()
{
    if (colors)
        attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(LINES / 2 - 2, (COLS - strlen(win)) / 2, win);
    if (colors)
        attroff(COLOR_PAIR(1) | A_BOLD);
    mvprintw(LINES / 2 - 1, (COLS - 10) / 2, "score: %2d",
             snk.len - STARTING_SIZE);
    mvprintw(LINES / 2 + 1, (COLS - strlen(restart)) / 2, restart);
}

void setup()
{
    int x, y;
    snk.len = STARTING_SIZE;
    for (int i = snk.len; i < MAX_SIZE; i++)
        snk.pos[i].x = snk.pos[i].y = -1;
    x       = COLS / 2 - COLS / 5;
    y       = LINES / 2;
    for (int i = 0; i < snk.len; i++) {
        snk.pos[i].x = --x;
        snk.pos[i].y = y;
    }
    snk.dir = right;
    print_snake();
    food.x = COLS / 2 + COLS / 5;
    food.y = LINES / 2;
    print_food();
}

void game_loop()
{
    int over = 0, pause = 0, s_lines = LINES, s_cols = COLS;

    for (int ch = getch(); ch != 27 && ch != 'q'; ch = getch()) {
        if (LINES < MIN_LINES || COLS < MIN_COLS) {
            clear();
            mvprintw(0, 0, "Please resize your window and press r");
            over = 1;
            continue;;
        }
        if (LINES != s_lines || COLS != s_cols) {
            s_lines = LINES;
            s_cols  = COLS;
            clear();
            box(stdscr, ACS_VLINE, ACS_HLINE);
            mvprintw(LINES / 2 - 1, (COLS - strlen(restart)) / 2, restart);
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
                print_food();
            } else {
                pause = 1;
                timeout(-1);
                mvprintw(LINES / 2 - 1, (COLS - strlen(paused)) / 2, paused);
            }
            break;
        case 'w':
        case 'k':
        case KEY_UP:
            if (snk.dir != down)
                snk.dir = up;
            break;
        case 'a':
        case 'h':
        case KEY_LEFT:
            if (snk.dir != right)
                snk.dir = left;
            break;
        case 's':
        case 'j':
        case KEY_DOWN:
            if (snk.dir != up)
                snk.dir = down;
            break;
        case 'd':
        case 'l':
        case KEY_RIGHT:
            if (snk.dir != left)
                snk.dir = right;
            break;
        case 'r':
            if (!over && !pause)
                break;
            clear();
            box(stdscr, ACS_VLINE, ACS_HLINE);
            setup();
            over = 0;
            pause = 0;
            timeout(TIMEOUT);
            break;
        }
        if (over || pause)
            continue;

        // delete last segment
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

        print_snake();

        // FOOD COLLISION
        if (snk.pos[0].x == food.x && snk.pos[0].y == food.y) {
            ++snk.len;
            food.x = rand() % (COLS - 2) + 1;
            food.y = rand() % (LINES - 2) + 1;
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
