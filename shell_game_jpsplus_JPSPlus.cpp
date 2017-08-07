#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <unordered_map>
#include "shell_game_jpsplus_JPSPlus.h"
#include "Entry.h"
#include "MapFile.h"

std::unordered_map<int, void*> pres;

/*
 * Class:     shell_game_jpsplus_JPSPlus
 * Method:    save
 * Signature: ([ZIIILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_shell_game_jpsplus_JPSPlus_save
    (JNIEnv *env, jobject, jbooleanArray blocks, jint width, jint height, jint id, jstring dir)
{
  const char *dir_name = env->GetStringUTFChars(dir, 0);
  std::string map_file_name(dir_name);
  std::string pre_file_name = map_file_name + "/" + std::to_string(id) + ".pre";
  map_file_name = map_file_name + "/" + std::to_string(id) + ".map";

  int block_size = env->GetArrayLength(blocks);
  jboolean* blocks0 = env->GetBooleanArrayElements(blocks, 0);
  std::vector<bool> blocks1(block_size);
  for (int i = 0; i < block_size; ++i) {
    blocks1[i] = blocks0[i];
  }

  int version = 0xffffffff;
  SaveMap(map_file_name.c_str(), blocks1, width, height, version);
  PreprocessMap(blocks1, width, height, pre_file_name.c_str());

  // release
  env->ReleaseBooleanArrayElements(blocks, blocks0, 0);
  env->ReleaseStringUTFChars(dir, dir_name);
}

/*
 * Class:     shell_game_jpsplus_JPSPlus
 * Method:    load
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_shell_game_jpsplus_JPSPlus_load
    (JNIEnv *env, jobject, jint id, jstring dir)
{
  const char *dir_name = env->GetStringUTFChars(dir, 0);
  std::string map_file_name(dir_name);
  std::string pre_file_name = map_file_name + "/" + std::to_string(id) + ".pre";
  map_file_name = map_file_name + "/" + std::to_string(id) + ".map";

  std::vector<bool> blocks;
  int width, height, version;
  printf("load start...\n");
  LoadMap(map_file_name.c_str(), blocks, width, height, version);

  printf("prepare start...\n");
  void * pre = PrepareForSearch(blocks, width, height, pre_file_name.c_str());
  if (pres.find(id) != pres.end()) { // 不允许重新加载
    printf("ignore duplicate file[id:%d,width:%d,height:%d,version:%d]\n", id, width, height, version);
    delete(pre);
    return;
  }
  printf("load file[id:%d,width:%d,height:%d,version:%d]\n", id, width, height, version);
  pres[id] = pre;

  // release
  env->ReleaseStringUTFChars(dir, dir_name);

  printf("done\n");
}

/*
 * Class:     shell_game_jpsplus_JPSPlus
 * Method:    find
 * Signature: (III)[I
 */
JNIEXPORT jintArray JNICALL Java_shell_game_jpsplus_JPSPlus_find
    (JNIEnv *env, jobject, jint id, jint src, jint dst)
{
  std::unordered_map<int, void*>::iterator find = pres.find(id);
  if (find == pres.end()) {
    printf("can not find %d\n", id);
    return NULL;
  }
  void* data = find->second;
  if (data == NULL) {
    printf("%d is null, impossible!!!\n", id);
    return NULL;
  }

  std::vector<xyLoc> path;
  xyLoc s, g;
  s.x = (src >> 16) & 0xffff;
  s.y = src & 0xffff;
  g.x = (dst >> 16) & 0xffff;
  g.y = dst & 0xffff;
//  printf("try find[%d,%d->%d,%d]\n", s.x, s.y, g.x, g.y);
  if (!GetPath(data, s, g, path)) {
//    printf("!!!find->%d\n", path.size());
    jintArray newArray = env->NewIntArray(path.size());
    jint *narr = env->GetIntArrayElements(newArray, NULL);
    for (int i = 0; i < path.size(); ++i) {
//      printf("find->%d,%d\n", path[i].x, path[i].y);
      int v = path[i].x;
      v = v << 16;
      narr[i] = v | path[i].y;
    }

    env->ReleaseIntArrayElements(newArray, narr, NULL);
    return newArray;
  }
  return NULL;
}