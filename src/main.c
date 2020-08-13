
#define MAX 3
#define LENGTH 255
#define BUFF 80
#define max(a, b) (a > b ? a : b)
#define ESC 27
#define ENTER 13
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dirent.h>
#include <assert.h>
#include <regex.h>
#include <unistd.h>
#include <ncurses.h>
chtype colors[MAX];
char str[LENGTH];
char *songs[LENGTH];
chtype *initcolors()

{
  int i;
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  init_pair(2, COLOR_BLACK, COLOR_RED);
  init_pair(3, COLOR_RED, COLOR_WHITE);
  for (i = 0; i < MAX; i++)
    colors[i] = COLOR_PAIR(i + 1);
  return colors;
}

void readopt(char str[LENGTH], int opts)
{
  int err, start, end, match, i;
  regmatch_t pmatch[1];
  const char *str_regex;
  regex_t preg;
  switch (opts)
  {
  case 0:
    str_regex = "^add\\s*(.+)$";
    break;
  case 1:
    str_regex = "^play\\s*(.+)$";
    break;
  case 2:
    str_regex = "^play\\s*(all)$";
    break;
  }
  err = regcomp(&preg, str_regex, REG_EXTENDED);
  if (err == 0)
  {
    match = regexec(&preg, str, 2, pmatch, 0);
    if (match == 0)
    {
      start = pmatch[1].rm_so;
      end = pmatch[1].rm_eo;
      char res[end - start + 1];
      for (i = start; i <= end; i++)
        res[i - start] = str[i];
      switch (opts)
      {
      case 0:
        chdir(res);
        break;
      case 1:
        play(res);
        break;
      case 2:
        play("*.mp3");
        break;
      }
    }
  }
  regfree(&preg);
}

void domenu(unsigned int opt)
{

  int x, y, j, k, max = 0;
  char cwd[LENGTH];
  int posx = 0;
  int posy = 6;
  getcwd(cwd, sizeof(cwd));
  getmaxyx(stdscr, y, x);
  clean(stdscr);
  unsigned int nsongs = list();
  wrefresh(stdscr);
  mvprintw(2, (x - strlen("MAIN  MENU  MUSIC  PLAYER BY JULIO  &  HONORIO ")) / 2, "MAIN  MENU  MUSIC  PLAYER BY JULIO ");
  mvprintw(3, (x - (strlen("Current working dir: ") + strlen(cwd))) / 2, "Current working dir: %s", cwd);
  if (nsongs > 0)
  {
    for (j = 0; j < nsongs; ++j)
    {
      if (posy > y - 5)
      {
        for (k = j - (posy - 6); k <= j; k++)
          max = max(max, strlen(songs[k]));
        posy = 6;
        posx += max + 4;
      }
      mvprintw(posy, posx, "%d.%s", j + 1, songs[j]);
      if (j == opt)
      {
        attron(colors[1]);
        mvprintw(posy, posx, "%d.%s", j + 1, songs[j]);
        attroff(colors[1]);
      }
      posy++;
    }
  }
  else
  {
    attron(colors[2]);
    mvprintw(y / 2, (x - strlen("Couldn't find any song on this dir")) / 2, "Couldn't find any song on this dir");
    attroff(colors[2]);
  }
}

void fillscreen(WINDOW *win, int id)
{
  start_color();
  chtype *colors = initcolors();
  attron(colors[id - 1]);
  wbkgd(win, colors[id - 1]);
  wrefresh(win);
}

void printtime(float f)
{
  int x, y, ch, i;
  getmaxyx(stdscr, y, x);
  start_color();
  chtype *colors = initcolors();
  attron(colors[3]);
  curs_set(0);
  mvprintw(y - 2, f / 100, " ");
  attroff(colors[3]);
  mvprintw(y - 3, x / 2 - 3, "%2.2f%%", f);
}

void initscreen()
{
  initscr();
  nonl();
  curs_set(0);
  noecho();
  //raw();
  keypad(stdscr, TRUE);
}

void clean(WINDOW *win)
{
  system("clear");
  wclear(win);
}

void end()
{
  int i;
  for (i = 0; i < MAX; i++)
    attroff(colors[i]);
  cbreak();
  echo();
  curs_set(1);
  endwin();
}
bool iscanged(int y, int x)
{
  int x1, y1;
  getmaxyx(stdscr, y1, x1);
  return x1 != x || y1 != y;
}
void readcomand()
{
  int x, y;
  getmaxyx(stdscr, y, x);
  char comand[LENGTH];
  echo();
  curs_set(1);
  wmove(stdscr, y - 1, 0);
  getnstr(comand, LENGTH);
  noecho();
  curs_set(0);
  readopt(comand, 0);
  readopt(comand, 1);
  readopt(comand, 2);
}

unsigned int moove(unsigned int select, int ch)
{
  unsigned int nsongs = list();
  if (nsongs != 0)
  {
    switch (ch)
    {
    case KEY_UP:
      if (select == 0)
        select = nsongs;
      select = (select - 1) % nsongs;
      break;
    case KEY_DOWN:
      if (select == nsongs - 1)
        select = -1;
      select = (select + 1) % nsongs;
      break;
    }
  }
  return select;
}

bool isvalid(char *file)
{
  int len = strlen(file) - 1;
  return file[len - 3] == '.' && file[len - 2] == 'm' && file[len - 1] == 'p' && file[len] == '3';
}
unsigned int list()
{
  unsigned int i = 0;
  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  while ((dir = readdir(d)) != NULL)
  {
    if (isvalid(dir->d_name))
    {
      songs[i] = dir->d_name;
      i++;
    }
  }
  closedir(d);
  return i;
}

void stop() { system("killall play 2>/tmp/stop.txt"); }

void *player(void *_) { system(str); }

void play(char *song)
{
  int i;
  pthread_t thread;
  char aux[strlen(song) + 2];
  aux[0] = '"';
  aux[strlen(song) + 1] = '"';
  for (i = 0; i < strlen(song); i++)
    aux[i + 1] = song[i];
  strcpy(str, 'play "');
  strcat(str, aux);
  strcat(str, ' " 2>/tmp/play.txt');
  stop();
  pthread_create(&thread, NULL, player, NULL);
}

int main(int argc, char const *argv[])
{
  int ch, y, x;
  unsigned int opt = 0;
  initscreen();
  getmaxyx(stdscr, y, x);
  fillscreen(stdscr, 1);
  domenu(opt);
  while ((ch = getch()) != ESC)
  {
    switch (ch)
    {
    case 's':
    case 'S':
      stop();
      break;
    case ENTER:
      play(songs[opt]);
      break;
    case ':':
      readcomand();
      domenu(opt);
      break;
    case KEY_UP:
    case KEY_DOWN:
      opt = moove(opt, ch);
      domenu(opt);
      break;
    }
    if (iscanged(y, x))
    {
      getmaxyx(stdscr, y, x);
      domenu(opt);
    }
  }
  stop();
  end();
  return 0;
}
