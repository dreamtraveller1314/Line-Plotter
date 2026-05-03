#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEIGHT 30
#define WIDTH 100

int main() {
  printf("\x1b[8;%d;%dt", HEIGHT + 1, WIDTH + 2);
  printf("\x1b[?25l");                        
  CLEAR_SCREEN();

  SLEEP(5);
  CLEAR_SCREEN();

  return 0;
}