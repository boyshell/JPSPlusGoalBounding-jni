//
//  File.cpp
//  MapAbstraction
//
//  Created by Nathan Sturtevant on 7/11/13.
//  Modified by Steve Rabin 12/15/14
//
//

#include "stdafx.h"
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <numeric>
#include <algorithm>
#include "ScenarioLoader.h"
#include "Entry.h"
#include <stdlib.h>
#include <dirent.h>
#include <sys/time.h>

#include <iostream>
#include <fstream>

const char* const DIR_NAME="/home/zhangxiangxi/Documents/workplace/JPSPlusWithGoalBounding-master/JPSPlusGoalBounding/Maps/";

void LoadMap(const char *fname, std::vector<bool> &map, int &w, int &h);

struct stats {
	std::vector<long> times;
	std::vector<xyLoc> path;
	std::vector<int> lengths;
	
	double GetTotalTime()
	{
		return std::accumulate(times.begin(), times.end(), 0.0);
	}
	double GetMaxTimestep()
	{
		return *std::max_element(times.begin(), times.end());
	}
	double Get20MoveTime()
	{
		for (unsigned int x = 0; x < lengths.size(); x++)
			if (lengths[x] >= 20)
				return std::accumulate(times.begin(), times.begin()+1+x, 0.0);
		return GetTotalTime();
	}
	double GetPathLength()
	{
		double len = 0;
		for (int x = 0; x < (int)path.size()-1; x++)
		{
			if (path[x].x == path[x+1].x || path[x].y == path[x+1].y)
			{
				len++;
			}
			else {
				len += 1.4142;
			}
		}
		return len;
	}
	bool ValidatePath(int width, int height, const std::vector<bool> &mapData)
	{
		for (int x = 0; x < (int)path.size()-1; x++)
		{
			if (abs(path[x].x - path[x+1].x) > 1)
				return false;
			if (abs(path[x].y - path[x+1].y) > 1)
				return false;
			if (!mapData[path[x].y*width+path[x].x])
				return false;
			if (!mapData[path[x+1].y*width+path[x+1].x])
				return false;
			if (path[x].x != path[x+1].x && path[x].y != path[x+1].y)
			{
				if (!mapData[path[x+1].y*width+path[x].x])
					return false;
				if (!mapData[path[x].y*width+path[x+1].x])
					return false;
			}
		}
		return true;
	}
};

long now_millis() {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return tp.tv_sec * 1000000 + tp.tv_usec;
}

long test(const char* const file) {
	char filename[2048];
	std::vector<xyLoc> thePath;
	std::vector<bool> mapData;
	int width, height;
	bool pre = false;
	bool run = true;
	bool silenceIndividualTests = false;

	char mapFilename[2048] = "\0";
	char mapScenarioFilename[2048] = "\0";
	char mapPreprocessedFilename[2048] = "\0";
	if (false)
	{
		return 0;
	}
	else
	{
		int filenameLength = 0;
		while(file[++filenameLength] != 0) {}

		if (file[filenameLength-3] == 'm' && file[filenameLength-2] == 'a' && file[filenameLength-1] == 'p')
		{
			char baseFilename[2048];
			for(int i=0; i<filenameLength; i++)
			{
				baseFilename[i] = file[i];
			}
			baseFilename[filenameLength] = '\0';

			sprintf(mapFilename, "%s/%s", DIR_NAME, baseFilename);
			sprintf(mapScenarioFilename, "%s/%s.scen", DIR_NAME, baseFilename);
			sprintf(mapPreprocessedFilename, "%s/%s.pre", DIR_NAME, baseFilename);

			std::ifstream ifile(mapPreprocessedFilename);
			pre = !ifile;
		}
		else
		{
			return 0;
		}
	}

	LoadMap(mapFilename, mapData, width, height);

	if (pre)
	{
		printf("Begin preprocessing map: %s\n", mapFilename);
		PreprocessMap(mapData, width, height, mapPreprocessedFilename);
		printf("Done preprocessing map: %s\n", mapFilename);
	}

	if (!run)
	{
		return 0;
	}

	void *reference = PrepareForSearch(mapData, width, height, mapPreprocessedFilename);

	ScenarioLoader scen(mapScenarioFilename);

//		Timer t;
	std::vector<stats> experimentStats;
	for (int x = 0; x < scen.GetNumExperiments(); x++)
	{
		thePath.resize(0);
		experimentStats.resize(x+1);
		bool done;
		do {
			xyLoc s, g;
			s.x = scen.GetNthExperiment(x).GetStartX();
			s.y = scen.GetNthExperiment(x).GetStartY();
			g.x = scen.GetNthExperiment(x).GetGoalX();
			g.y = scen.GetNthExperiment(x).GetGoalY();

			if(s.x == g.x && s.y == g.y)
			{
				done = true;
			}
			else
			{
				long now = now_millis();
				done = GetPath(reference, s, g, thePath);
				now = now_millis() - now;

				if(thePath.size() > 0)
				{
					experimentStats[x].times.push_back(now);
					experimentStats[x].lengths.push_back(thePath.size());
					for (unsigned int t = experimentStats[x].path.size(); t < thePath.size(); t++)
						experimentStats[x].path.push_back(thePath[t]);
				}
			}
		} while (done == false);

	}
	thePath.clear();
	delete reference;

	long totalTime = 0;
	bool invalid = false;
	bool suboptimal = false;
	for (unsigned int x = 0; x < experimentStats.size(); x++)
	{
		if(!silenceIndividualTests)
		{
			printf("GPPC\t%s\ttotal-time\t%f\tmax-time-step\t%f\ttime-20-moves\t%f\ttotal-len\t%f\tsubopt\t%f\t", mapScenarioFilename,
				   experimentStats[x].GetTotalTime(), experimentStats[x].GetMaxTimestep(), experimentStats[x].Get20MoveTime(),
				   experimentStats[x].GetPathLength(), experimentStats[x].GetPathLength()/scen.GetNthExperiment(x).GetDistance());
		}

		if (experimentStats[x].path.size() != 0 &&
			(experimentStats[x].ValidatePath(width, height, mapData) &&
			 scen.GetNthExperiment(x).GetStartX() == experimentStats[x].path[0].x &&
			 scen.GetNthExperiment(x).GetStartY() == experimentStats[x].path[0].y &&
			 scen.GetNthExperiment(x).GetGoalX() == experimentStats[x].path.back().x &&
			 scen.GetNthExperiment(x).GetGoalY() == experimentStats[x].path.back().y))
		{
			if(!silenceIndividualTests)
				printf("valid\n");
		}
		else
		{
			invalid = true;
			if(!silenceIndividualTests)
				printf("invalid\n");
		}
		if (experimentStats[x].GetPathLength() / scen.GetNthExperiment(x).GetDistance() > 1.000005f)
		{
			suboptimal = true;
		}

		totalTime += experimentStats[x].GetTotalTime();
	}

	printf("Total map time: %ld,\t%s", totalTime, mapFilename);
	if (invalid) { printf(",\tINVALID"); }
	if (suboptimal) { printf(",\tSUBOPTIMAL"); }
	printf("\n");

	for (unsigned int x = 0; x < experimentStats.size(); x++)
	{
		experimentStats[x].path.clear();
		experimentStats[x].times.clear();
		experimentStats[x].lengths.clear();
	}
	experimentStats.clear();
	scen.Clear();
	mapData.clear();

	return totalTime;
}

int main(int argc, char* argv[])
{


//	WIN32_FIND_DATA ffd;
//	LARGE_INTEGER filesize;
//	TCHAR szDir[MAX_PATH];
//	size_t length_of_arg;
//	HANDLE hFind = INVALID_HANDLE_VALUE;
//	DWORD dwError=0;
//
//	StringCchCopy(szDir, MAX_PATH, TEXT("Maps\\*"));
//	hFind = FindFirstFile(szDir, &ffd);

	long allTestsTotalTime = 0;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (DIR_NAME)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			allTestsTotalTime += test(ent->d_name);
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return EXIT_FAILURE;
	}

	printf("All tests total time: %ld\n", allTestsTotalTime);

	while(true) {}

	return 0;
}

void LoadMap(const char *fname, std::vector<bool> &map, int &width, int &height)
{
	FILE *f;
	f = fopen(fname, "r");
	if (f)
	{
		fscanf(f, "type octile\nheight %d\nwidth %d\nmap\n", &height, &width);
		map.resize(height*width);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				char c;
				do {
					fscanf(f, "%c", &c);
				} while (isspace(c));
				map[y*width+x] = (c == '.' || c == 'G' || c == 'S');
			}
		}
		fclose(f);
	}
}
