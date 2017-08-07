//
// Created by 张祥熙 on 17/8/7.
//

#ifndef JPSPLUSGOALBOUNDING_MAPFILE_H
#define JPSPLUSGOALBOUNDING_MAPFILE_H

#include <vector>

void LoadMap(const char *fname, std::vector<bool> &map, int &width, int &height, int &version);

void SaveMap(const char *fname, const std::vector<bool> &map, const int &width, const int &height, const int &version);

#endif //JPSPLUSGOALBOUNDING_MAPFILE_H
