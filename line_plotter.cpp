#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEIGHT 30
#define WIDTH 100

#ifdef _WIN32
#include <windows.h>
#define SLEEP(t) Sleep(t * 1000)
#define CLEAR_SCREEN() system("cls")
#else
#include <unistd.h>
#define SLEEP(t) sleep(t)
#define CLEAR_SCREEN() system("clear")
#endif

int make_screen_array(char *screen_buffer) {
  screen_buffer[0] = '\0'; 
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      strcat(screen_buffer, " "); 
    }                             
    if (y < HEIGHT - 1) {         
      strcat(screen_buffer, "\n");
    }
  }
  return 0;
}

int draw_symbol(int x, int y, char *buffer, char symbol) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return 1;

  int flipped_y = (HEIGHT - 1) - y;
  int location = flipped_y * (WIDTH + 1) + x;
  buffer[location] = symbol;
  return 0; 
}

void render_screen(char *buffer) {
  printf("\x1b[H");
  int len = (WIDTH + 1) * HEIGHT; 
  for (int i = 0; i < len; i++) {
    char c = buffer[i];
    if (c == 'o') {
      printf("\x1b[32m%c\x1b[0m", c);
    } else if (c == '-' || c == '|' || c == '+') {
      printf("\x1b[31m%c\x1b[0m", c);
    } else {
      printf("%c", c);
    }
  }
  fflush(stdout);
}

double ask_for_data(char *question) {
  char input_buffer[64];
  double output;
  char *end_ptr;
  while (1) {
    printf("Enter %s: ", question);
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
      printf("\nInput error. Exiting.\n");
      exit(1);
    }
    input_buffer[strcspn(input_buffer, "\n")] = 0;
    if (strlen(input_buffer) == 0) {
      printf("Error: Input cannot be empty!\n");
      continue;
    }
    output = strtod(input_buffer, &end_ptr);
    if (end_ptr == input_buffer) {
      printf("Error: '%s' is not a valid number!\n", input_buffer);
      continue;
    }
    if (*end_ptr != '\0') {
      printf("Error: Please enter only numbers (found '%s' at end).\n", end_ptr);
      continue;
    }
    return output;
  }
}

int *calc_y_bounds(double slope, double y_intercept) {
  int *arr = (int*)malloc(2 * sizeof(int));
  arr[0] = INT_MAX;
  arr[1] = INT_MIN;
  for (int i = 0; i < WIDTH; i++) {
    int y = (int)(slope * i + y_intercept);
    if (y > arr[1]) arr[1] = y;
    if (y < arr[0]) arr[0] = y;
  }
  return arr;
}

int draw_max_min_scale(int *bounds, char *screen_buffer) {
  int min = bounds[0], max = bounds[1];
  char min_string[20], max_string[20];
  snprintf(min_string, sizeof(min_string), "%d", min);
  snprintf(max_string, sizeof(max_string), "%d", max);
  size_t max_len = strlen(max_string);
  if (max_len > WIDTH) max_len = WIDTH;
  memcpy(&screen_buffer[0], max_string, max_len);
  int location = (HEIGHT - 1) * (WIDTH + 1);
  size_t min_len = strlen(min_string);
  if (min_len > WIDTH) min_len = WIDTH;
  memcpy(&screen_buffer[location], min_string, min_len);
  fflush(stdout);
  return 0;
}

int draw_graph(double slope, double y_intercept, char *screen_buffer) {
  int *bounds = calc_y_bounds(slope, y_intercept);
  int x_axis_location;
  if (bounds[0] == bounds[1]) {
    x_axis_location = (HEIGHT / 2); 
  } else {
    double x_axis_ratio = (0.0 - bounds[0]) / (double)(bounds[1] - bounds[0]);
    x_axis_location = (int)((HEIGHT - 1) * x_axis_ratio);
  }
  if (x_axis_location < 0) x_axis_location = 0;
  if (x_axis_location > HEIGHT - 1) x_axis_location = HEIGHT - 1;
  for (int i = 0; i < WIDTH; i++) {
    draw_symbol(i, x_axis_location, screen_buffer, '-');
  }
  int y_axis_x_location = 0; 
  for (int j = 0; j < HEIGHT; j++) {
    if (j == x_axis_location) {
      draw_symbol(y_axis_x_location, j, screen_buffer, '+');
    } else {
      draw_symbol(y_axis_x_location, j, screen_buffer, '|');
    }
  }
  for (int i = 0; i < WIDTH; i++) {
    double y = slope * i + y_intercept;
    int normal_y;
    if (bounds[0] == bounds[1]) {
      normal_y = HEIGHT / 2;
    } else {
      normal_y = (int)((HEIGHT - 1) * ((y - bounds[0]) / (bounds[1] - bounds[0])));
    }
    draw_symbol(i, normal_y, screen_buffer, 'o');
  }
  draw_max_min_scale(bounds, screen_buffer);
  render_screen(screen_buffer);
  free(bounds); 
  return 0;
}

int main() {
  #ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
  #endif
  double slope = ask_for_data("slope");
  double y_intercept = ask_for_data("y_intercept");
  printf("\x1b[8;%d;%dt", HEIGHT + 1, WIDTH + 2);
  printf("\x1b[?25l");
  CLEAR_SCREEN();
  char *screen_buffer = (char*)malloc((WIDTH + 1) * HEIGHT + 1);
  if (screen_buffer == NULL) {
      fprintf(stderr, "Fatal Error: Could not allocate memory for screen buffer!\n");
      return 1;
  }
  make_screen_array(screen_buffer);
  draw_graph(slope, y_intercept, screen_buffer);
  SLEEP(5);
  CLEAR_SCREEN();
  free(screen_buffer);
  return 0;
}