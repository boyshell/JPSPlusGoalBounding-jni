//
// Created by 张祥熙 on 17/8/7.
//
#include "MapFile.h"

const char TRUE = '.';
const char FALSE = '@';

void LoadMap(const char *fname, std::vector<bool> &map, int &width, int &height, int &version) {
  FILE *f;
  f = fopen(fname, "r");
  if (f) {
    fscanf(f, "version:%d\nheight:%d\nwidth:%d\n", &version, &height, &width);
    map.resize(height * width);
    printf("resize");
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        char c;
        fscanf(f, "%c", &c);
        map[y * width + x] = c == TRUE;
      }
    }
    fclose(f);
  }
}

void SaveMap(const char *fname, const std::vector<bool> &map, const int &width, const int &height, const int &version) {
  FILE *f;
  f = fopen(fname, "w");
  if (f) {
    fprintf(f, "version:%d\nheight:%d\nwidth:%d\n", version, height, width);
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        char c = map[y * width + x] ? TRUE : FALSE;
        fprintf(f, "%c", c);
      }
    }
    fclose(f);
  }
}