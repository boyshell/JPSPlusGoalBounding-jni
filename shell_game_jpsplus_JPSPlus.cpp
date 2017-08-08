#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include "shell_game_jpsplus_JPSPlus.h"
#include "Entry.h"
#include "MapFile.h"
#include "JPSPlus.h"

std::unordered_map<int, JPSPlus*> jps_map;
std::unordered_map<int, PrecomputeMap*> pre_map;

/*
 * Class:     shell_game_jpsplus_JPSPlus
 * Method:    save
 * Signature: ([ZIIILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_shell_game_jpsplus_JPSPlus_save(JNIEnv *env, jclass, jbooleanArray blocks, jint width, jint height, jint id, jstring dir)
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
JNIEXPORT void JNICALL Java_shell_game_jpsplus_JPSPlus_load(JNIEnv *env, jclass, jint id, jstring dir)
{
  const char *dir_name = env->GetStringUTFChars(dir, 0);
  std::string map_file_name(dir_name);
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

  // release
  env->ReleaseStringUTFChars(dir, dir_name);
}

/*
 * Class:     shell_game_jpsplus_JPSPlus
 * Method:    find
 * Signature: (III)[I
 */
JNIEXPORT jintArray JNICALL Java_shell_game_jpsplus_JPSPlus_find(JNIEnv *env, jclass, jint id, jshort sx, jshort sy, jshort dx, jshort dy)
{
  std::unordered_map<int, JPSPlus*>::iterator find = jps_map.find(id);
  if (find == jps_map.end()) {
    printf("can not find %d\n", id);
    return NULL;
  }
  void* data = find->second;
  if (data == NULL) {
    printf("%d is null, impossible!!!\n", id);
    return NULL;
  }
  // TODO 多线程有问题啊。我曹，保存在类当中的

  std::vector<xyLoc> path;
  xyLoc s, g;
  s.x = sx;
  s.y = sy;
  g.x = dx;
  g.y = dy;
  if (!GetPath(data, s, g, path)) {
    jintArray newArray = env->NewIntArray(path.size());
    jint *narr = env->GetIntArrayElements(newArray, NULL);
    for (int i = 0; i < path.size(); ++i) {
      int v = path[i].x;
      v = v << 16;
      narr[i] = v | path[i].y;
    }

    env->ReleaseIntArrayElements(newArray, narr, NULL);
    return newArray;
  }
  return NULL;
}

/*
 * Class:     shell_game_jpsplus_JPSPlus
 * Method:    walkable
 * Signature: (ISS)Z
 */
JNIEXPORT jboolean JNICALL Java_shell_game_jpsplus_JPSPlus_walkable(JNIEnv *env, jclass, jint id, jshort x, jshort y)
{
  std::unordered_map<int, PrecomputeMap*>::iterator find = pre_map.find(id);
  if (find == pre_map.end()) {
    printf("can not find %d\n", id);
    return false;
  }

  return !find->second->IsWall(x, y);
}