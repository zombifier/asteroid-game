// Khoi Doan - Zappa
// TODO:
// Move all the class and function definitions to another file
// Clean up the code to make it more readable by people other than me (but make it work first!)
#include <cstdlib>
#include <ncurses.h> // also includes stdio.h
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

const int dir_table[4][3] = { {0, 1, 2}, {2, 4, 7}, {5, 6, 7}, {0, 3, 5} };
int score, lose;
int playerposx, playerposy;
WINDOW *battle_win, *score_win;
pthread_mutex_t mutex; // makes sure no more than one thread may access the battle
class Enemy {
protected:
    int posx, posy, pposx, pposy;
    int ver_dir, hor_dir;
public:
    Enemy();
    virtual int move_enemy();
};

// most basic and common enemy, Asteroids move in a straight line
class Asteroid : public Enemy {
public:
    int move_enemy();
};

// Missiles will actually chase after you, homing in your position!
// however they can't change direction that fast due to inertia,
// so you can actually convince them to leave if you are smart...
class Missile : public Enemy {
private:
    int inertia; // they has to move this much before they can change direction
public:
    Missile();
    int move_enemy();
};

// Bombs will explode after a period of time,
// destroying everything in a 3x3 radius, so watch out!
class Bomb : public Enemy {
};

// Comets leave a trail behind which are also deadly if you touch them!
// the trail dissipates after a while, oldest part first
// note: other objects hitting the trail will dissipate that part of the trail
class Comet : public Enemy {
};

// Planetoids. A.K.A Asteroids that are 4 times as large. be afraid!
// they're twice as slow though

Enemy::Enemy() {
    int i = random() % 4;
    switch (i) {
    case 0: // top
        posy = 1;
        posx = random() % 30 + 1;
        ver_dir = 1;
        hor_dir = random() % 3 - 1;
        break;
    case 1: // left
        posx = 1;
        posy = random() % 20 + 1;
        ver_dir = random() % 3 - 1;
        hor_dir = 1;
        break;
    case 2: // bottom
        posy = 20;
        posx = random() % 30 + 1;
        ver_dir = -1;
        hor_dir = random() % 3 - 1;
        break;
    case 3: // right
        posx = 30;
        posy = random() % 20 + 1;
        ver_dir = random() % 3 - 1;
        hor_dir = -1;
        break;
    }
}

Missile::Missile()
{
    inertia = 5;
}

int Enemy::move_enemy() {
    return 0;
}

int Asteroid::move_enemy()
{
    int ret;
    pposx = posx; pposy = posy;
    posx += hor_dir;
    posy += ver_dir;
    pthread_mutex_lock(&mutex);
    mvwaddch(battle_win, pposy, pposx, ' ');
    if ((mvwinch(battle_win, posy, posx)) == '@') { // COLLISION! SUCKS TO BE YOU!
        ret = -1;
    }
    else if ((posx == 0) || (posx == 31) || (posy == 0) || (posy == 21)) {
        ret = 1;
    }
    else {
        mvwaddch(battle_win, posy, posx, 'A');
        ret = 0;
    }
    wrefresh(battle_win);
    pthread_mutex_unlock(&mutex);
    return ret;
}

int Missile::move_enemy()
{
    int ret;
    pposx = posx; pposy = posy;
    // direction change code goes here (there must be a better way!)
    // note that if there's more than one way to change direction,
    // go counter-clockwise
    if (inertia == 0) {
        switch (hor_dir) {
        case -1:
            switch (ver_dir) {
            case -1:
                if (playerposy > posy) ver_dir++;
                else if (playerposx > posx) hor_dir++;
                break;
            case 0:
                if (playerposy < posy) ver_dir--;
                else if (playerposy > posy) ver_dir++;
                else if (playerposx > posx) ver_dir--;
                break;
            case 1:
                if (playerposx > posx) hor_dir++;
                else if (playerposy < posy) ver_dir--;
                break;
            }
            break;
        case 0:
            switch (ver_dir) {
            case -1:
                if (playerposx < posx) hor_dir--;
                else if (playerposx > posx) hor_dir++;
                else if (playerposy > posy) hor_dir--;
                break;
            case 1:
                if (playerposx > posx) hor_dir++;
                else if (playerposx < posx) hor_dir--;
                else if (playerposy < posy) hor_dir++;
                break;
            }
            break;
        case 1:
            switch (ver_dir) {
            case -1:
                if (playerposx < posx) hor_dir--;
                else if (playerposy > posy) ver_dir++;
                break;
            case 0:
                if (playerposy < posy) ver_dir--;
                else if (playerposy > posy) ver_dir++;
                else if (playerposx < posx) ver_dir--;
                break;
            case 1:
                if (playerposy < posy) ver_dir--;
                else if (playerposx < posx) hor_dir--;
                break;
            }
            break;
        }
        inertia = 5;
    }
    else inertia--;
    // end
    posx += hor_dir;
    posy += ver_dir;
    pthread_mutex_lock(&mutex);
    mvwaddch(battle_win, pposy, pposx, ' ');
    if ((mvwinch(battle_win, posy, posx)) == '@') { // COLLISION! SUCKS TO BE YOU!
        ret = -1;
    }
    else if ((posx == 0) || (posx == 31) || (posy == 0) || (posy == 21)) {
        ret = 1;
    }
    else {
        mvwaddch(battle_win, posy, posx, 'H');
        ret = 0;
    }
    wrefresh(battle_win);
    pthread_mutex_unlock(&mutex);
    return ret;
}

void *move_score(void *i)	// routine run by the scores thread to increment the score
{
    score = 0;
    while (1) {
        usleep(200000);
        score++;
        mvwprintw(score_win, 0, 0, "Score: %d", score);
        wrefresh(score_win);
    }
}

void *move_player(void *i)	// routine run by the player thread to move the player
{
    int ch, pposx, pposy;
    playerposx = 15; playerposy = 10;
    while (1) {
        pposy = playerposy;
        pposx = playerposx;
        ch = getch();
        switch (ch) {
        case KEY_UP:
            if (playerposy > 1) playerposy--;
            break;
        case KEY_DOWN:
            if (playerposy < 20) playerposy++;
            break;
        case KEY_LEFT:
            if (playerposx > 1) playerposx--;
            break;
        case KEY_RIGHT:
            if (playerposx < 30) playerposx++;
            break;
        }
        pthread_mutex_lock(&mutex);
        if ((mvwinch(battle_win, playerposy, playerposx)) != ' ') lose = 0; // COLLISION! BUT THIS TIME YOU DID IT!
        mvwaddch(battle_win, pposy, pposx, ' ');
        mvwaddch(battle_win, playerposy, playerposx, '@');
        wrefresh(battle_win);
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    srandom(time(NULL));
    int ch, row, col, i, status;
    pthread_t score_thread, player_thread;
    Enemy *enemy_list[20];
    pthread_mutex_init (&mutex, NULL);
    initscr();	// start curses mode
//    raw();	// line buffering disabled, control character gotten
    cbreak();
    noecho();	// we dont want what the user type to get out
//    start_color();
    curs_set(0);
    keypad(stdscr, TRUE);	// we get f1, f2, etc.
    getmaxyx(stdscr, row, col);	// get the number of rows and columns
    battle_win = newwin(22, 32, 0, 0);
    box(battle_win, 0, 0);
    score_win = newwin(1, 30, 22, 0);
    mvwaddch(battle_win, 10, 15, '@');
    refresh();
    wrefresh(score_win);
    lose = 1;
    // now we create the threads
    pthread_create(&score_thread, NULL, move_score, NULL);
    pthread_create(&player_thread, NULL, move_player, NULL);
    
    // main thread will control the enemies

    for (i = 0; i < 20; i++) {
        enemy_list[i] = new Asteroid;
    }
    while (lose) {
        for (i = 0; (i < 20) && (lose); i++) {
            status = enemy_list[i]->move_enemy();
            if (status == -1) {
                lose = 0;
            }
            else if (status == 1) {
                delete enemy_list[i];
                enemy_list[i] = new Asteroid;
            }
        }
        usleep(100000);
    }
    pthread_cancel(score_thread);
    pthread_join(score_thread, NULL);
    pthread_cancel(player_thread);
    pthread_join(player_thread, NULL);
    mvwaddch(battle_win, playerposy, playerposx, 'O');
    mvwaddch(battle_win, playerposy-1, playerposx-1, '\\');
    mvwaddch(battle_win, playerposy-1, playerposx, '|');
    mvwaddch(battle_win, playerposy-1, playerposx+1, '/');
    mvwaddch(battle_win, playerposy, playerposx-1, '-');
    mvwaddch(battle_win, playerposy, playerposx+1, '-');
    mvwaddch(battle_win, playerposy+1, playerposx-1, '/');
    mvwaddch(battle_win, playerposy+1, playerposx, '|');
    mvwaddch(battle_win, playerposy+1, playerposx+1, '\\');
    wrefresh(battle_win);
    getch();	// wait for user input
    endwin();	// end curses mode
    printf("Your score is: %d\n", score);
    return 0;
}
