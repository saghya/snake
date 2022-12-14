/*
 * TODO: - fix random die
 *       - splash screen
 */

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_LINES    7
#define MIN_COLS     15
#define STARTING_LEN 5
#define MAX_LEN      1024
#define TIMEOUT      250

enum direction { up, down, left, right };

typedef struct _position {
    int x, y;
} position;

typedef struct _snake {
    position pos[MAX_LEN];
    int      dir;
    int      len;
    int      color;
} snake;

typedef struct _food {
    position pos;
    int      ch;
    int      color;
} food;

static const char welcome[]   = "press any key to start";
static const char game_over[] = "game over";
static const char restart[]   = "press r to restart";
static const char win[]       = "you won";
static const char paused[]    = "paused";
static snake      snk;
static food       fd;
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
        init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    }
    snk.color = COLOR_GREEN;
}

void print_snake()
{
    attron(A_BOLD);
    if (colors)
        attron(COLOR_PAIR(snk.color));
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
        attroff(COLOR_PAIR(snk.color));
}

void print_food()
{
    attron(A_BOLD);
    if (colors) {
        attron(COLOR_PAIR(fd.color));
    }
    for (int i = 0; i < snk.len; i++) {
        if (snk.pos[i].x == fd.pos.x && snk.pos[i].y == fd.pos.y) {
            fd.pos.x = rand() % (COLS - 2) + 1;
            fd.pos.y = rand() % (LINES - 2) + 1;
            i        = 0;
        }
    }
    mvaddch(fd.pos.y, fd.pos.x, fd.ch);
    if (colors)
        attroff(COLOR_PAIR(fd.color));
    attroff(A_BOLD);
}

void print_lose()
{
    if (colors)
        attron(COLOR_PAIR(COLOR_RED) | A_BOLD);
    mvprintw(LINES / 2 - 2, (COLS - strlen(game_over)) / 2, game_over);
    if (colors)
        attroff(COLOR_PAIR(COLOR_RED) | A_BOLD);
    mvprintw(LINES / 2 - 1, (COLS - 9) / 2, "score: ");
    if (colors)
        attron(COLOR_PAIR(COLOR_BLUE));
    printw("%2d", snk.len - STARTING_LEN);
    if (colors)
        attroff(COLOR_PAIR(COLOR_BLUE));
    mvprintw(LINES / 2 + 1, (COLS - strlen(restart)) / 2, restart);
}

void print_win()
{
    if (colors)
        attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(LINES / 2 - 2, (COLS - strlen(win)) / 2, win);
    if (colors)
        attroff(COLOR_PAIR(COLOR_GREEN) | A_BOLD);
    mvprintw(LINES / 2 - 1, (COLS - 10) / 2, "score: %2d",
             snk.len - STARTING_LEN);
    mvprintw(LINES / 2 + 1, (COLS - strlen(restart)) / 2, restart);
}

void set_food_char()
{
    fd.ch = rand() % (126 - 32) + 33; // random printable ascii char
    if (colors)
        fd.color = rand() % 6 + 1;
}

void setup()
{
    int x, y;
    snk.len = STARTING_LEN;
    for (int i = snk.len; i < MAX_LEN; i++)
        snk.pos[i].x = snk.pos[i].y = -1;
    x = COLS / 2 - COLS / 6;
    y = LINES / 2;
    for (int i = 0; i < snk.len; i++) {
        snk.pos[i].x = --x;
        snk.pos[i].y = y;
    }
    snk.dir = right;
    print_snake();
    fd.pos.x = COLS / 2 + COLS / 6;
    fd.pos.y = LINES / 2;
    set_food_char();
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
            continue;
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
                if (colors)
                    attron(COLOR_PAIR(COLOR_BLUE));
                mvprintw(LINES / 2 - 1, (COLS - strlen(paused)) / 2, paused);
                if (colors)
                    attroff(COLOR_PAIR(COLOR_BLUE));
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
            over  = 0;
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
        if (snk.pos[0].x == fd.pos.x && snk.pos[0].y == fd.pos.y) {
            ++snk.len;
            fd.pos.x = rand() % (COLS - 2) + 1;
            fd.pos.y = rand() % (LINES - 2) + 1;
            set_food_char();
            print_food();
        }

        // CRASH
        if (!snk.pos[0].x || !snk.pos[0].y || snk.pos[0].x == COLS - 1 ||
            snk.pos[0].y == LINES - 1) {
            print_lose();
            over = 1;
        }
        for (int i = 1; i < snk.len - 1; i++) {
            if (snk.pos[0].x == snk.pos[i].x && snk.pos[0].y == snk.pos[i].y) {
                print_lose();
                over = 1;
            }
        }

        // WIN
        if (snk.len == MAX_LEN || snk.len == (LINES - 2) * (COLS - 2)) {
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
}

