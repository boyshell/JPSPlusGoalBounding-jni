cmake_minimum_required(VERSION 3.8)
project(JPSPlusGoalBounding)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
#        main.cpp
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
        UnsortedPriorityQueue.h)

#add_executable(JPSPlusGoalBounding ${SOURCE_FILES})
add_library(JPSPlusGoalBounding ${SOURCE_FILES})