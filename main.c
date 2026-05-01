#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ROWS   30
#define COLS   30
#define ESCAPE 27

void configure_buffer(bool buffer[ROWS][COLS]) {
  FILE *pattern = fopen("pattern.txt", "r");
  if (!pattern) {
    endwin();
    printf("Failed to open the pattern file\n");
    exit(1);
  }
  char temp[COLS];
  for (int i = 0; i < ROWS; i++) {
    if (!fgets(temp, COLS, pattern)) { break; }
    for (int j = 0; j < COLS; j++) {
      buffer[i][j] = temp[j] == '#' ? true : false;
    }
  }
  fclose(pattern);
}

void draw_buffer(bool buffer[ROWS][COLS]) {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      mvprintw((rows - ROWS) / 2 + i, cols / 2 - COLS + j * 2, "%c", buffer[i][j] ? '#' : '.');
    }
  }
}

void calculate_neighbours(bool buffer[ROWS][COLS], int i, int j, int *alive, int *died) {
  *alive = *died = 0;
  if (i > 0 && j > 0) buffer[i - 1][j - 1] ? (*alive)++ : (*died)++;
  if (i > 0 && j < COLS - 1) buffer[i - 1][j + 1] ? (*alive)++ : (*died)++;
  if (j < COLS - 1) buffer[i][j + 1] ? (*alive)++ : (*died)++;
  if (i < ROWS - 1 && j > 0) buffer[i + 1][j - 1] ? (*alive)++ : (*died)++;
  if (i < ROWS - 1 && j < COLS - 1) buffer[i + 1][j + 1] ? (*alive)++ : (*died)++;
  if (i < ROWS - 1) buffer[i + 1][j] ? (*alive)++ : (*died)++;
  if (i > 0) buffer[i - 1][j] ? (*alive)++ : (*died)++;
  if (j > 0) buffer[i][j - 1] ? (*alive)++ : (*died)++;
}

void calculate_next_generation(bool buffer[ROWS][COLS]) {
  bool temp[ROWS][COLS];
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      int alive, died;
      calculate_neighbours(buffer, i, j, &alive, &died);
      if (buffer[i][j]) {
        temp[i][j] = alive < 2 || alive > 3 ? false : true;
      } else if (!buffer[i][j]) {
        temp[i][j] = alive >= 3 ? true : false;
      }
    }
  }
  memcpy(buffer, temp, sizeof(temp));
}

int main() {
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);

  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  int cols_by_ratio = COLS * 2;
  if (rows < ROWS || cols < cols_by_ratio) {
    endwin();
    printf("Your terminal too little. It must be %dx%d or above\n", ROWS, cols_by_ratio);
    exit(1);
  }

  bool buffer[ROWS][COLS];
  configure_buffer(buffer);

  int generations = 1;
  while (1) {
    int ch = getch();
    if (ch == ESCAPE) break;
    draw_buffer(buffer);
    refresh();
    calculate_next_generation(buffer);
    mvprintw(1, 1, "Generation %d", generations++);
    usleep(100000 * 2);
  }
  endwin();
}
