#include <ncurses.h>
#define LENGTH 255
chtype *initcolors();
void readopt(char str[LENGTH], int opts);
void domenu(unsigned int opt);
void fillscreen(WINDOW *win, int id);
void printtime(float f);
void initscreen();
void clean(WINDOW *win);
void end();
bool iscanged(int y, int x);
void readcomand();
unsigned int moove(unsigned int select, int ch);
bool isvalid(char *file);
void stop();
void *player(void *_);
void play(char *song);
unsigned int list();