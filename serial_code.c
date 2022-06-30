#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// we hardcode the image dimensions here
int imgwidth = 512;
int imgheight = 512;
int size = 512 * 512;
int pix_num = 255;

void filewrite(unsigned char *d, int width, int height) {
  struct bmp_id id;
  id.magic1 = 0x42;
  id.magic2 = 0x4D;
  struct bmp_header header;
  header.file_size = width * height + 54 + 2;
  header.pixel_offset = 1078;
  struct bmp_dib_header head;
  head.header_size = 40;
  head.width = width;
  head.height = height;
  head.num_planes = 1;
  head.bit_pr_pixel = 8;
  head.compress_type = 0;
  head.data_size = width * height;
  head.hres = 0;
  head.vres = 0;
  head.num_colors = 256;
  head.num_imp_colors = 0;
  char padding[2];
  unsigned char *arr = (unsigned char *)malloc(1024);
  for (int c = 0; c < 256; c++) {
    arr[c * 4] = (unsigned char)c;
    arr[c * 4 + 1] = (unsigned char)c;
    arr[c * 4 + 2] = (unsigned char)c;
    arr[c * 4 + 3] = 0;
  }
  FILE *fp = fopen("out.bmp", "w+");
  fwrite((void *)&id, 1, 2, fp);
  fwrite((void *)&header, 1, 12, fp);
  fwrite((void *)&head, 1, 40, fp);
  fwrite((void *)arr, 1, 1024, fp);
  fwrite((void *)d, 1, width * height, fp);
  fwrite((void *)&padding, 1, 2, fp);
  fclose(fp);
}

unsigned char *fileread(char *filename) {
  FILE *fp = fopen(filename, "rb");
  int width, height, offset;
  fseek(fp, 18, SEEK_SET);
  fread(&width, 4, 1, fp);
  fseek(fp, 22, SEEK_SET);
  fread(&height, 4, 1, fp);
  fseek(fp, 10, SEEK_SET);
  fread(&offset, 4, 1, fp);
  unsigned char *d =
      (unsigned char *)malloc(sizeof(unsigned char) * height * width);
  fseek(fp, offset, SEEK_SET); // to move the file pointer by our offset value.
  fread(d, sizeof(unsigned char), height * width, fp);
  fclose(fp); // close file
  return d;
}

void flip_horizontal(unsigned char *d, unsigned int cols, unsigned int rows) {
  unsigned int left = 0;
  unsigned int right = cols;
  for (int r = 0; r < rows; r++) {
    while (left != right && right > left) {
      int temp = d[r * cols + left];
      d[(r * cols) + left] = d[(r * cols) + cols - right];
      d[(r * cols) + cols - right] = temp;
      right--;
      left++;
    }
  }
}

int main(int argc, char **argv) {
  int thread_num = atoi(argv[2]);
  unsigned char *image = fileread(argv[1]);
  unsigned char *new_img = malloc(sizeof(unsigned char) * size);
  for (int i = 0; i < size; i++) {
    printf("%d\n ", image[i]);
  }
  int *invert = (int *)calloc(sizeof(int), pix_num);
  for (int i = 0; i < size; i++) {
    invert[image[i]]++;
    // printf("%d\n",invert[image[i]]);
  }
  float *new_temp = (float *)calloc(sizeof(float), pix_num);
  for (int i = 0; i < pix_num; i++) {
    for (int j = 0; j < i + 1; j++) {
      new_temp[i] += pix_num * ((float)invert[j]) / (size);
    }
  }
  for (int i = 0; i < size; i++) {
    new_img[i] = new_temp[image[i]]; // storing the new image
  }
  filewrite(image, imgwidth, imgheight);
}
