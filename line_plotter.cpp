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

int draw_symbol(int ball_x, int ball_y, char *screen_buffer, char symbol) {
    if (ball_x < 0 || ball_x >= WIDTH || ball_y < 0 || ball_y >= HEIGHT) {
        return 1;
    }
    ball_y = (HEIGHT - 1) - ball_y;
    int location = ball_y * (WIDTH + 1) + ball_x;
    screen_buffer[location] = symbol;
    printf("\x1b[H");
    printf("%s", screen_buffer);
    fflush(stdout);
    return 0;
}

double ask_for_data(char *question) {
  while (1) {
    char input_buffer[10];
    printf("Enter %s: ", question);
    if (fgets(input_buffer, 10, stdin) == NULL) continue;
    char *temp_pointer = strchr(input_buffer, '\n'); 
    if (temp_pointer) *temp_pointer = '\0';
    char *end_ptr;
    double output = strtod(input_buffer, &end_ptr); 
    if (*end_ptr == '\0' && strlen(input_buffer) > 0) { 
      return output;
    } else {
      printf("Please enter a valid number!\n");
    }
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
  printf("\x1b[H");
  printf("%s", screen_buffer);
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
  free(bounds); 
  return 0;
}

int main() {
  double slope = ask_for_data("slope");
  double y_intercept = ask_for_data("y_intercept");
  printf("\x1b[8;%d;%dt", HEIGHT + 1, WIDTH + 2);
  printf("\x1b[?25l");
  CLEAR_SCREEN();
  char *screen_buffer = (char*)malloc((WIDTH + 1) * HEIGHT + 1);
  make_screen_array(screen_buffer);
  draw_graph(slope, y_intercept, screen_buffer);
  SLEEP(5);
  CLEAR_SCREEN();
  free(screen_buffer);
  return 0;
}