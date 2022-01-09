#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "functions.h"

#define TOUCH 1
#define MKDIR 2
#define CHDIR 3
#define MOVE  4
#define COPY  5
#define COMMAND 6

bool quit;
bool user_is_typing;
bool first_letter;
int ymax, xmax;
int old_ymax, old_xmax;
int cur_dir;
int dir_size;
int user_index;
int action;
int limit;
char mode;
char dir_content[256][256];
char user_input[256];
WINDOW *win;

void init() 
{
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  limit = getmaxy(win)*2-4;
  mode = 'n';
  quit = false;
  user_is_typing = false;
  first_letter = false;
  cur_dir = 1;
  user_index = 0;
  dir_size = get_dir_size(".");
  get_dir_content(".", dir_content);
  getmaxyx(stdscr, ymax, xmax);
  old_ymax = ymax;
  old_xmax = xmax;
  win = newwin(ymax-2, xmax-10, 2, 5);
}

void draw_window(WINDOW* window)
{
  box(window,0,0);
  refresh();
  wrefresh(window);
}

void print_current_dir()
{
  for(int i=0; i<(int)sizeof(dir_content); i++) {
    if(i<getmaxy(win)-2) {
      mvwaddstr(win,i+1,3,dir_content[i]); 
    }
    else {
      mvwaddstr(win,(i-getmaxy(win))+3,getmaxx(win)/2+6,dir_content[i]);
    }
  }
}

void print_current_dir_highlight()
{
  for(int i=0; i<(int)sizeof(dir_content); i++) {
    if(i<getmaxy(win)-2) {
      if (i == 0) {
        wattron(win, A_STANDOUT);
      } else wattroff(win, A_STANDOUT);
      mvwaddstr(win,i+1,3,dir_content[i]); 
    }
    else {
      mvwaddstr(win,(i-getmaxy(win))+3,getmaxx(win)/2+6,dir_content[i]);
    }
  }
}

void reset()
{
  werase(win);
  memset(dir_content, 0, sizeof dir_content);
  memset(user_input, 0, sizeof user_input);
  if (!user_is_typing)
    cur_dir = 1;
  user_index = 0;
  limit = getmaxy(win)*2-4;
  get_dir_content(".", dir_content);
  print_current_dir_highlight();
  dir_size = get_dir_size(".");
  mode = 'n';
}

void start_typing()
{
  user_is_typing = first_letter = true;
  reset();
}

int main(int argc, char *argv[])
{
  init();
  if (argv[argc-1] != NULL)
    chdir(argv[argc-1]);
  print_current_dir_highlight();
  reset();
  
  while (!quit) {
    getmaxyx(stdscr, ymax, xmax);
    draw_window(win);
    mvaddch(0, 0, '[');
    mvaddch(0, 1, mode);
    mvaddch(0, 2, ']');

    if (ymax != old_ymax || xmax != old_xmax)
      win = newwin(ymax-2, xmax-10, 2, 5);

    int key = getch();
    if (!user_is_typing) switch(key) {
      case 'q':
        quit = true;
        break;
      case 'j':
        if (cur_dir < dir_size && cur_dir < limit) {
          print_current_dir();
          cur_dir++;
          wattron(win, A_STANDOUT);
          if (cur_dir-1<getmaxy(win)-2) {
            mvwaddstr(win, cur_dir, 3, dir_content[cur_dir-1]);
          } else {
            mvwaddstr(win,cur_dir-ymax+4,xmax/2+1,dir_content[cur_dir-1]);
          }
          wattroff(win, A_STANDOUT);
        }
        break;
      case 'k':
        if (cur_dir > 1) {
          print_current_dir();
          cur_dir--;
          wattron(win, A_STANDOUT);
          if (cur_dir-1<getmaxy(win)-2) {
            mvwaddstr(win, cur_dir, 3, dir_content[cur_dir-1]);
          } else {
            mvwaddstr(win,cur_dir-ymax+4,xmax/2+1,dir_content[cur_dir-1]);
          }
          wattroff(win, A_STANDOUT);
        }
        break;
      case 'h':
        chdir("..");
        reset();
        break;
      case 'l':
        if (is_regular_file(dir_content[cur_dir-1]) == 0) {
          chdir(dir_content[cur_dir-1]);
          reset();
        } 
        else {
          execlp("nvim", "nvim", dir_content[cur_dir-1], NULL);
          quit = true;
        }
        break;
      case 'd':
        mvaddstr(0, xmax/2-15/2, "Delete file y/n");
        if (getch() == 'y')
          remove(dir_content[cur_dir-1]);
        erase();
        reset();
        break;
      case 't':
        start_typing();
        action = TOUCH;
        break;
      case 'T':
        start_typing();
        action = MKDIR;
        break;
      case 'f':
        start_typing();
        action = CHDIR;
        break;
      case 'm':
        start_typing();
        action = MOVE;
        break;
      case 'c':
        start_typing();
        action = COPY;
        break;
      case '!':
        start_typing();
        action = COMMAND;
        break;
    }
    if (user_is_typing) {
      mode = 'i';
      switch(key) {
      case 10:
        user_is_typing = false;
        erase();
        switch (action){
          case TOUCH:
            if (user_index != 0)
              createFile(user_input);
            reset();
            break;
          case MKDIR:
            if (user_index != 0)
              createFolder(user_input);
            reset();
            break;
          case CHDIR:
            chdir(user_input);
            reset();
            break;
          case MOVE:
            if (user_index != 0)
              moveFile(dir_content[cur_dir-1], user_input);
            reset();
            break;
          case COPY:
            if (user_index != 0)
              copyFile(dir_content[cur_dir-1], user_input);
            reset();
            break;
          case COMMAND:
            system(user_input);
            system("clear");
            reset();
            break;
        }
        break;
      case 27:
        user_is_typing = false;
        erase();
        reset();
        break;
      case 127:
        if (user_index > 0) {
          user_input[user_index-1] = 0;
          user_index--;
          mvaddch(0, 5+user_index, ' ');
        }
        break;
      default:
        if (!first_letter) {
          user_index++;
          user_input[user_index-1] = key;
          mvaddstr(0, 5, user_input);
        }
        break;
      }
      first_letter = false;
    }
  }
  endwin();
  return 0;
}
