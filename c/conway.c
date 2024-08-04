#include <stdio.h>
#include <stdlib.h>

#define FRAMES 10
#define WIDTH 1280
#define HEIGHT WIDTH

size_t it = 0;

typedef struct rgb {
  unsigned char r, g, b;
} rgb;

void mapValueToImage(size_t *it, unsigned int *arr);
void populate_neighborhood(unsigned int *arr);
void peek(unsigned int *arr);

int main(void) {
  unsigned int nbhd[WIDTH * HEIGHT] = {0};
  populate_neighborhood(nbhd);

  while (it < FRAMES) {
    mapValueToImage(&it, nbhd);
    it++;
  }
  return 0;
}

void mapValueToImage(size_t *it, unsigned int *arr) {
  char output_file[16];
  sprintf(output_file, "output_%04zu.ppm", *it);

  FILE *fp = fopen(output_file, "wb");
  if (fp == NULL) {
    fprintf(stderr, "[ERROR] File not created.");
  }
  fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

  rgb *image = (rgb *)malloc(sizeof(rgb) * WIDTH * HEIGHT);

  rgb pxl;
  for (int i = 0; i < WIDTH; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      pxl.r = (i + 12) % 0xff;
      pxl.g = (i + 32) % 0xff;
      pxl.b = (i + 62) % 0xff;
      if (arr[i * WIDTH + j]) {
        pxl.r = (i + 12) % 0xff;
        pxl.g = (i + 32) % 0xff;
        pxl.b = (i + 62) % 0xff;
      }
      image[i * WIDTH + j] = pxl;
    }
  }

  size_t writtenBytes = fwrite(image, sizeof(rgb), WIDTH * HEIGHT, fp);
  if (writtenBytes != sizeof(rgb) * WIDTH * HEIGHT) {
    fprintf(stderr, "Didn't write enough bytes to file. Only %zu/%d.",
            writtenBytes, WIDTH * HEIGHT);
  }

  free(image);
  fclose(fp);
}

void populate_neighborhood(unsigned int *arr) {
  for (int i = 0; i < WIDTH; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      if (arr[i * WIDTH + j] % 2 == 0) {
        arr[i * WIDTH + j] = 1;
      }
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
