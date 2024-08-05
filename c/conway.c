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
void insertSlice(size_t from_w, size_t from_h, size_t startSlice, unsigned int *from_arr, unsigned int *to_arr);
void peek(unsigned int *arr);

int main(void) {
  unsigned int nbhd[N] = {0};
  populateNeighborhood(nbhd);
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
  unsigned int glider_5x6[30] = {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
                          1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0};
  unsigned int glider_5x5[25] = {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1,
                          0, 0, 0, 0, 0, 0, 0};
  // Let's hand-roll a 5x5 glider for this one...
  // 0   0   0   0   0
  // 0   1   1   0   0
  // 0   0   0   1   0
  // 0   1   1   0   0
  // 0   0   0   0   0

  // insertSlice(5, 6, WIDTH * 25 + HEIGHT - 100, glider_5x6, arr);
  insertSlice(5, 6, WIDTH * 25 + HEIGHT - 100, glider_5x5, arr);
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

void insertSlice(size_t from_w, size_t from_h, size_t sliceStart,
                 unsigned int *from_arr, unsigned int *to_arr) {
  // We're going to be putting this slice at position
  // sliceStart, make sure it fits first.
  int row = (int)(sliceStart % WIDTH + 1);
  int col = sliceStart - (row) * WIDTH;

  if ((from_w > WIDTH - col) || (from_h > HEIGHT - row)) {
    fprintf(stderr, "from_w=%zu from_h=%zu WIDTH=%d HEIGHT=%d row=%d col=%d",
                    from_w, from_h, WIDTH, HEIGHT, row, col);
    fprintf(stderr, "[ERROR] Can't put an array with larger size into a slice "
                    "of a smaller one.");
  }

  printf("row=%d col=%d\n\n", row, col);
  size_t idx = 0;
  for (int i = row; i < row + from_w; ++i) {
    for (int j = col; j < col + from_h; ++j) {
      to_arr[i * WIDTH + j] = from_arr[idx];
      idx++;
    }
  }
}
