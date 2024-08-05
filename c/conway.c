#include <stdio.h>
#include <stdlib.h>

#define FRAMES 256
#define WIDTH 128
#define HEIGHT WIDTH
#define N (WIDTH * HEIGHT)
// TODO:  - CLI arguments for frames, fps, and picture size
//        - Option to superscale a picture so one individual in the game
//          of life can occupy more than one pixel
//        - Implement other initial states for neighborhood (glider, ...)
//        - Move to an actual graphical window instead of generating a movie;
//          accept inputs from user mouse clicks to populate neighborhood.

size_t it = 0;

typedef struct rgb {
  unsigned char r, g, b;
} rgb;

void iterateLife(unsigned int *arr);
int mapValueToImage(size_t *it, unsigned int *arr);
int inBounds(int i, int j, size_t direction);
void populateNeighborhood(unsigned int *arr);
void peek(unsigned int *arr);

int main(void) {
  // unsigned int nbhd[N];
  // populateNeighborhood(nbhd);

  // Let's hand-roll a 3x3 glider for this one...
  // 0   0   0   0   0
  // 0   1   0   0   0
  // 0   0   1   1   0
  // 0   1   1   0   0
  // 0   0   0   0   0

  unsigned int nbhd[N] = {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
                          1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0};
  peek(nbhd);

  while (it < FRAMES) {
    mapValueToImage(&it, nbhd);
    iterateLife(nbhd);
    it++;
  }
  return 0;
}

int mapValueToImage(size_t *it, unsigned int *arr) {
  char output_file[25];
  sprintf(output_file, "./tmp/output_%04zu.ppm", *it);

  FILE *fp = fopen(output_file, "wb");
  if (fp == NULL) {
    fprintf(stderr, "[ERROR] File not created.");
    return 1;
  }
  fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

  rgb *image = (rgb *)malloc(sizeof(rgb) * N);

  rgb pxl = {0, 0, 0};
  for (int i = 0; i < WIDTH; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      if (arr[i * WIDTH + j]) {
        pxl.r = 0xff;
        pxl.g = 0xff;
        pxl.b = 0xff;
      }
      else{
        pxl.r = 0x00;
        pxl.g = 0x00;
        pxl.b = 0x00;
      }
      image[i * WIDTH + j] = pxl;
    }
  }

  size_t writtenBytes = fwrite(image, sizeof(rgb), N, fp);
  if (writtenBytes != N) {
    fprintf(stderr, "[ERROR] Didn't write enough bytes to file. Only %zu/%d.",
            writtenBytes, N);
    return 1;
  }

  free(image);
  fclose(fp);
  printf("[STATUS] Done making image %08zu/%08d.\r", *it, N);
  return 0;
}

void populateNeighborhood(unsigned int *arr) {
  int counter = 0;
  for (int i = 0; i < WIDTH; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      if (counter % 2){
        arr[i * WIDTH + j] = 1;
      }
      counter++;
      // if ((i * WIDTH + j) % 2 == 0) {
      //   arr[i * WIDTH + j] = 1;
      // } else {
      //   arr[i * WIDTH + j] = 0;
      // }
    }
  }
}

void peek(unsigned int *arr) {
  // Array variables are secretly pointers to the array's 0th element
  for (int i = 0; i < WIDTH; ++i) {
    printf("[");
    for (int j = 0; j < WIDTH; ++j) {
      printf("%d, ", arr[i * WIDTH + j]);
    }
    printf("]\n");
  }
}

int inBounds(int i, int j, size_t direction) {
  // 0 is up, 1 is top right, 2 is right, ... (clockwise)
  switch (direction) {
  case 0:
    // UP
    if ((i - 1) < 0)
      return 0;
  case 1:
    // TOP_R
    if ((i - 1 < 0) || (j + 1 >= WIDTH))
      return 0;
  case 2:
    // R
    if ((j + 1) >= WIDTH)
      return 0;
  case 3:
    // DWN_R
    if ((i + 1 >= HEIGHT) || (j + 1 >= WIDTH))
      return 0;
  case 4:
    // DWN
    if ((i + 1) >= HEIGHT)
      return 0;
  case 5:
    // DWN_L
    if ((i + 1 >= HEIGHT) || (j - 1 < 0))
      return 0;
  case 6:
    // L
    if ((j - 1) < 0)
      return 0;
  case 7:
    // TOP_L
    if ((i - 1 < 0) || (j - 1 < 0))
      return 0;
  }
  return 1;
}

void iterateLife(unsigned int *arr) {
  // Takes the current state of the neighborhood and computes the next iteration
  size_t UP, TOP_R, RIGHT, DWN_R, DWN, DWN_L, LEFT, TOP_L;

  for (int i = 0; i < WIDTH; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      size_t neighbors = 0;
      // I can't find a better way to enumerate these directions
      // Explore every direction first, then if explored direction's coordinates
      // are in array && alive, increment neighbors counter.

      UP = i * (WIDTH - 1) + (j + 0);
      TOP_R = i * (WIDTH - 1) + (j + 1);
      RIGHT = i * (WIDTH + 0) + (j + 1);
      DWN_R = i * (WIDTH + 1) + (j + 1);
      DWN = i * (WIDTH + 1) + (j + 0);
      DWN_L = i * (WIDTH + 1) + (j - 1);
      LEFT = i * (WIDTH + 0) + (j - 1);
      TOP_L = i * (WIDTH - 1) + (j - 1);

      size_t directions[8] = {UP, TOP_R, RIGHT, DWN_R, DWN, DWN_L, LEFT, TOP_L};
      for (int d = 0; d < 9; d++) {
        int direction = directions[d];
        if (inBounds(i, j, d)) {
          if (arr[direction]) {
            neighbors++;
          }
        }
      }

      // TODO: Instead of making the values 0 or 1, how about actually putting
      // the number of neighbors so we can later use it to show which ones are
      // starving, happy, or overpopulated in their current position.
      switch (neighbors) {
      case 0:
        arr[i * WIDTH + j] = 0;
        break;
      case 1:
        arr[i * WIDTH + j] = 1;
        break;
      case 2:
        arr[i * WIDTH + j] = 1;
        break;
      case 3:
        arr[i * WIDTH + j] = 0;
        break;
      }
    }
  }
}
