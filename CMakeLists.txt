cmake_minimum_required(VERSION 3.8)
project(JPSPlusGoalBounding)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCE_FILES
        BucketPriorityQueue.cpp
        BucketPriorityQueue.h
        Cases.h
        DijkstraFloodfill.cpp
        DijkstraFloodfill.h
        Entry.cpp
        Entry.h
        FastStack.cpp
        FastStack.h
        FPUtil.cpp
        FPUtil.h
        GenericHeap.cpp
        GenericHeap.h
        JPSPlus.cpp
        JPSPlus.h
        Map.cpp
        Map.h
        PathfindingNode.h
        PrecomputeMap.cpp
        PrecomputeMap.h
        ScenarioLoader.cpp
        ScenarioLoader.h
        SimpleUnsortedPriorityQueue.cpp
        SimpleUnsortedPriorityQueue.h
        stdafx.cpp
        stdafx.h
        targetver.h
        UnsortedPriorityQueue.cpp
        UnsortedPriorityQueue.h
        shell_game_jpsplus_JPSPlus.h
        shell_game_jpsplus_JPSPlus.cpp
#        shell_game_jpsplus_JPSPlusMain.cpp
        MapFile.h
        MapFile.cpp)

include_directories(/System/Library/Frameworks/JavaVM.framework/Headers
        /home/zhangxiangxi/bin/jdk1.8.0_131/include/linux
        /home/zhangxiangxi/bin/jdk1.8.0_131/include)

add_library(JPSPlusGoalBounding SHARED ${SOURCE_FILES})
#add_executable(JPSPlusGoalBounding ${SOURCE_FILES})
