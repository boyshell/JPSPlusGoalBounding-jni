//
// Created by 张祥熙 on 17/8/7.
//

#include <string>
#include <vector>
#include <unordered_map>
#include <sys/time.h>
#include "MapFile.h"
#include "Entry.h"
#include "PrecomputeMap.h"
#include "JPSPlus.h"

std::unordered_map<int, JPSPlus*> jps_map;
std::unordered_map<int, PrecomputeMap*> pre_map;

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
  LoadMap(map_file_name.c_str(), blocks, width, height, version);

  PrecomputeMap *p_pre = (PrecomputeMap *)NewPrecomputeMap(blocks, width, height, pre_file_name.c_str());
  JPSPlus *p_jps = (JPSPlus *)PrepareForSearch(p_pre);
  if (jps_map.find(id) != jps_map.end()) { // 不允许重新加载
    printf("ignore duplicate file[id:%d,width:%d,height:%d,version:%d]\n", id, width, height, version);
    delete(p_jps);
    delete(p_pre);
    return;
  }
  printf("load file[id:%d,width:%d,height:%d,version:%d]\n", id, width, height, version);
  jps_map[id] = p_jps;
  pre_map[id] = p_pre;
}

/*
 * Class:     shell_game_jpsplus_JPSPlus
 * Method:    find
 * Signature: (III)[I
 */
void Java_shell_game_jpsplus_JPSPlus_find(int id, short sx, short sy, short dx, short dy) {
  std::unordered_map<int, JPSPlus*>::iterator find = jps_map.find(id);
  if (find == jps_map.end()) {
    printf("can not find %d\n", id);
    return ;
  }
  void* data = find->second;
  if (data == NULL) {
    printf("%d is null, impossible!!!\n", id);
    return ;
  }

  std::vector<xyLoc> path;
  xyLoc s, g;
  s.x = sx;
  s.y = sy;
  g.x = dx;
  g.y = dy;
  if (!GetPath(data, s, g, path)) {
    for (int i = 0; i < path.size(); ++i) {
      printf("%d,%d\n", path[i].x, path[i].y);
    }
  }
}

long now_millis() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return tp.tv_sec * 1000000 + tp.tv_usec;
}

bool Java_shell_game_jpsplus_JPSPlus_walkable(int id, short x, short y)
{
  std::unordered_map<int, PrecomputeMap*>::iterator find = pre_map.find(id);
  if (find == pre_map.end()) {
    printf("can not find %d\n", id);
    return false;
  }

  // TODO 多线程有问题啊。我曹，保存在类当中的
  return !find->second->IsWall(x, y);
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

  long s = now_millis();
//  for (int i = 0; i < 1000000; ++i) {
//    Java_shell_game_jpsplus_JPSPlus_find(id, 0, 0, 99, 99);
//  }
  s = now_millis() - s;
  printf("cost %ld us\n", s);

  for (short y = 0; y < height; ++y) {
    for (short x = 0; x < width; ++x) {
      if (!Java_shell_game_jpsplus_JPSPlus_walkable(id, x, y)) {
        printf("fuck!!!!");
      }
    }
  }
}