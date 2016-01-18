// Khoi Doan - TYPING TEST!!!!

#include <ncurses.h> // also includes stdio.h
#include <string.h>
#include <sys/time.h>

double timeval_to_sec (struct timeval t)
{
    double sec = (double) t.tv_sec;
    double usec = (double) t.tv_usec;
    return (sec + usec / 1000000);
}

int main()
{
    int ch, i, error;
    char mesg[] = "All work and no play makes Jack a dull boy";
    char str[80];
    int row, col;
    double time;
    struct timeval begin;
    struct timeval end;
    gettimeofday(&begin, NULL);
    initscr();	// start curses mode
//    raw();	// line buffering disabled, control character gotten
    cbreak();
    noecho();	// we dont what what the user type to get out unless it's correct
//    start_color();
    keypad(stdscr, TRUE);	// we get f1, f2, etc.
    getmaxyx(stdscr, row, col);	// get the number of rows and columns
    attron(A_BOLD);
    printw(mesg);
    attroff(A_BOLD);
    printw("\n");
    i = 0;
    error = 0;
    while (i < strlen(mesg)) {
      ch = getch();
      if (ch == mesg[i]) {
        addch(ch);
        refresh();
        i++;
      }
      else error++;
    }
    gettimeofday(&end, NULL);
    clear();
    time = timeval_to_sec(end) - timeval_to_sec(begin);
    printw("Congrats! You cleared the string in %f seconds!\nWords per minute: %f\nNumber of errors made: %d", time, (double) strlen(mesg) / time / 5 * 60, error);
    refresh();	// print it on to the real screen
    getch();	// wait for user input
    endwin();	// end curses mode
    return 0;
}
