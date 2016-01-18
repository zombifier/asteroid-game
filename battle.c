// Khoi Doan - Zappa

#include <ncurses.h> // also includes stdio.h
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

WINDOW *battle_win, *score_win;

void *score(void*i)	// routine run by the scores thread to increment the score
{
    int score = 0;
    while (1) {
        usleep(200000);
        score++;
        mvwprintw(score_win, 0, 0, "Score: %d", score);
        wrefresh(score_win);
    }
}

void *move_player(void*i)	// routine run by the player thread to move the player_thread
{
    int ch, posx, posy;
    posx = 10; posy = 15;
    mvwaddch(battle_win, posy, posx, '@');
    wrefresh(battle_win);
    while (1) {
        mvwaddch(battle_win, posy, posx, ' ');
        ch = getch();
        switch (ch) {
        case KEY_UP:
            if (posy > 1) posy--;
            break;
        case KEY_DOWN:
            if (posy < 20) posy++;
            break;
        case KEY_LEFT:
            if (posx > 1) posx--;
            break;
        case KEY_RIGHT:
            if (posx < 30) posx++;
            break;
        }
        mvwaddch(battle_win, posy, posx, '@');
        wrefresh(battle_win);
    }
}

int main()
{
    int ch, row, col;
    pthread_t score_thread, player_thread;
    initscr();	// start curses mode
//    raw();	// line buffering disabled, control character gotten
    cbreak();
    noecho();	// we dont what what the user type to get out
//    start_color();
    curs_set(0);
    keypad(stdscr, TRUE);	// we get f1, f2, etc.
    getmaxyx(stdscr, row, col);	// get the number of rows and columns
    battle_win = newwin(22, 32, 0, 0);
    box(battle_win, 0, 0);
    score_win = newwin(1, 30, 22, 0);
    refresh();
    wrefresh(score_win);
    // now we create the threads
    pthread_create(&score_thread, NULL, score, NULL);
    pthread_create(&player_thread, NULL, move_player, NULL);
    
    // main thread will control the enemies
    while (1) {
    
    }
    getch();	// wait for user input
    endwin();	// end curses mode
    return 0;
}
