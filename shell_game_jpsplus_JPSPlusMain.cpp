//
// Created by 张祥熙 on 17/8/7.
//

#include <string>
#include <vector>
#include <unordered_map>
#include "MapFile.h"
#include "Entry.h"

static std::unordered_map<int, void *> pres;

void Java_shell_game_jpsplus_JPSPlus_save
    (std::vector<bool> &blocks, int width, int height, int id, std::string map_file_name) {
  std::string pre_file_name = map_file_name + "/" + std::to_string(id) + ".pre";
  map_file_name = map_file_name + "/" + std::to_string(id) + ".map";

  int version = 0xffffffff;
  SaveMap(map_file_name.c_str(), blocks, width, height, version);
  PreprocessMap(blocks, width, height, pre_file_name.c_str());
}

void
Java_shell_game_jpsplus_JPSPlus_load(int id, std::string map_file_name) {
  std::string pre_file_name = map_file_name + "/" + std::to_string(id) + ".pre";
  map_file_name = map_file_name + "/" + std::to_string(id) + ".map";

  std::vector<bool> blocks;
  int width, height, version;
  printf("load start...\n");
  LoadMap(map_file_name.c_str(), blocks, width, height, version);

  printf("prepare start...\n");
  void *pre = PrepareForSearch(blocks, width, height, pre_file_name.c_str());
  if (pres.find(id) != pres.end()) { // 不允许重新加载
    printf("ignore duplicate file[id:%d,width:%d,height:%d,version:%d]\n", id, width, height, version);
    delete (pre);
    return;
  }
  printf("load file[id:%d,width:%d,height:%d,version:%d]\n", id, width, height, version);
  pres[id] = pre;

  printf("done\n");
}

/*
 * Class:     shell_game_jpsplus_JPSPlus
 * Method:    find
 * Signature: (III)[I
 */
void Java_shell_game_jpsplus_JPSPlus_find(int id, int src, int dst) {
  std::unordered_map<int, void *>::iterator find = pres.find(id);
  if (find == pres.end()) {
    printf("can not find %d\n", id);
    return;
  }
  void *data = find->second;
  if (data == NULL) {
    printf("%d is null, impossible!!!\n", id);
    return;
  }

  std::vector<xyLoc> path;
  xyLoc s, g;
  s.x = (src >> 16) & 0xffff;
  s.y = src & 0xffff;
  g.x = (dst >> 16) & 0xffff;
  g.y = dst & 0xffff;
  if (GetPath(data, s, g, path)) {
    for (int i = 0; i < path.size(); ++i) {
      printf("%d,%d\n", path[i].x, path[i].y);
    }
  }
}

int main(int argc, char **argv) {
  int id = 1;
  int width = 100;
  int height = 100;
  std::vector<bool> blocks(width * height);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      blocks[y * width + x] = true;
    }
  }
  std::string dir(".");
  Java_shell_game_jpsplus_JPSPlus_save(blocks, width, height, id, dir.c_str());
  Java_shell_game_jpsplus_JPSPlus_load(id, dir.c_str());

  int src = (50 << 16) | 50;
  int dst = (99 << 16) | 99;
  Java_shell_game_jpsplus_JPSPlus_find(id, src, dst);
}