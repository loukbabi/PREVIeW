//============================================================================
// Name        : profiler.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef PROFILER_H
#define PROFILER_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <stdio.h>

class Profiler_Engine
{

private:

  std::map<std::string, std::vector<timespec> > timers_tics;
  std::map<std::string, std::vector<timespec> > timers_tocs;
  std::vector<std::string> timersLabels;
  bool allocated;
  unsigned int printingPrecision;

  // Time difference in seconds and nanoseconds (elapsed since the last second)
  inline timespec diff(timespec start, timespec end) {
    timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
      temp.tv_sec = end.tv_sec - start.tv_sec - 1;
      temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
      temp.tv_sec = end.tv_sec - start.tv_sec;
      temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
  }

public:

  Profiler_Engine(std::vector<std::string> *timersLabels_in, unsigned int printingPrecision_in) {
    for (unsigned int i = 0; i < timersLabels_in->size(); ++i)
    {
      std::vector<timespec> tempStack;
      timespec start;
      clock_gettime(CLOCK_MONOTONIC, &start);
      timers_tics.insert( std::pair<std::string, std::vector<timespec> >((*timersLabels_in)[i], tempStack) );
      timers_tocs.insert( std::pair<std::string, std::vector<timespec> >((*timersLabels_in)[i], tempStack) );
    }
    timersLabels = *timersLabels_in;
    printingPrecision = printingPrecision_in;
    allocated = true;
  }

  Profiler_Engine() {
    allocated = false;
    printingPrecision = 10;
  }

  void initialize(std::vector<std::string> *timersLabels_in, unsigned int printingPrecision_in)
  {
    for (unsigned int i = 0; i < timersLabels_in->size(); ++i)
    {
    std::vector<timespec> tempStack;
    timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    timers_tics.insert( std::pair<std::string, std::vector<timespec> >((*timersLabels_in)[i], tempStack) );
    timers_tocs.insert( std::pair<std::string, std::vector<timespec> >((*timersLabels_in)[i], tempStack) );
    }
    timersLabels = *timersLabels_in;
    printingPrecision = printingPrecision_in;
    allocated = true;
  }

  void tic(const std::string &label) {
    if(timers_tics.count(label))
    {
      std::map< std::string, std::vector<timespec> >::iterator it = timers_tics.find(label);
      timespec start;
      clock_gettime(CLOCK_MONOTONIC, &start);
      it->second.push_back(start);
    }else
      PRINTF_PLATFORM( "Label \"%s\" does not exists. You need to initialize it first.\n", label.c_str() );
  }

  void toc(const std::string &label) {
    if(timers_tocs.count(label))
    {
      std::map<std::string, std::vector<timespec> >::iterator it = timers_tocs.find(label);
      timespec end;
      clock_gettime(CLOCK_MONOTONIC, &end);
      it->second.push_back(end);
    }else
      PRINTF_PLATFORM( "Label \"%s\" does not exists. You need to initialize it first.\n", label.c_str() );
  }

  bool save(const std::string &label, const char* outputFolderPath = "") {
    if(timers_tocs.count(label))
    {
      std::vector<double> timings;
      if(!timers_tics.count(label))
	  {
    	  PRINTF_PLATFORM( "Label \"%s\" does not exists. You need to initialize it first.\n", label.c_str() );
    	  return false;
	  }
      std::map<std::string, std::vector<timespec> >::iterator it_tics = timers_tics.find(label);
      std::map<std::string, std::vector<timespec> >::iterator it_tocs = timers_tocs.find(label);

      unsigned int numOfTimings1 = it_tics->second.size();
      unsigned int numOfTimings2 = it_tocs->second.size();
      if(numOfTimings1 != numOfTimings2)
      {
        PRINTF_PLATFORM( "Timing Error! Different number of timing 'Tics' and 'Tocs' for \"%s\" label.\n", label.c_str() );
        return false;
      }
      if (numOfTimings1 == 0)
      {
        PRINTF_PLATFORM( "Timing Error! No timing entry for \"%s\" label.\n", label.c_str() );
        return false;
      }

      double avgTimer = 0.0;
      for (unsigned int i = 0; i < numOfTimings1; i++)
      {
        // Timer in msecs
        double tempTimer = diff(it_tics->second[i], it_tocs->second[i]).tv_sec * 1000 +
                        diff(it_tics->second[i], it_tocs->second[i]).tv_nsec/1000000.0;
        timings.push_back(tempTimer);
        avgTimer += tempTimer;
      }
      avgTimer /= double(numOfTimings1);
      PRINTF_PLATFORM("\"%s\" average time elapsed: %lf msecs\n", label.c_str(), avgTimer );

      char filePath[128];
      sprintf(filePath, "%s/t_%s.txt", outputFolderPath, label.c_str());
      std::ofstream myfile;

      myfile.open(filePath);
      if(!myfile.is_open())
      {
        PRINTF_PLATFORM("Could not create %s.\n", filePath);
        return false;
      }

      int timingsSize = int(timings.size());
      for (int i = 0; i < timingsSize; ++i)
      {
        myfile << std::setprecision(printingPrecision) << timings[i];
        if( i != (timingsSize - 1) )
          myfile << "\n";
      }

      myfile.close();

      #ifdef __ANDROID__
        #ifdef _DEBUG_
          char systemCommand[156];
          sprintf(systemCommand, "chmod 777 %s", filePath);
          system(systemCommand);
        #endif
      #endif
    }else
    {
      PRINTF_PLATFORM("Label \"%s\" does not exists. You need to initialize it first.\n", label.c_str());
      return false;
    }

    return true;
  }

  bool saveAll(const char* outputFolderPath = "") {
    PRINTF_PLATFORM("=====TIMINGS=====\n");
    for(unsigned int i = 0; i < timersLabels.size(); i++)
    {
      PRINTF_PLATFORM("------------\n");
      if(!save(timersLabels[i] , outputFolderPath) ) return false;
      PRINTF_PLATFORM("------------\n");
    }
    PRINTF_PLATFORM("=================\n");

    return true;

  }

  void deallocate()
  {
	  if(allocated)
	  {
		  timers_tics.clear();
		  timers_tocs.clear();
		  timersLabels.clear();
		  allocated = false;
	  }
  }

  ~Profiler_Engine()
  {
	  if(allocated)
	  {
		  timers_tics.clear();
		  timers_tocs.clear();
		  timersLabels.clear();
		  allocated = false;
	  }
  }
};

#endif // PROFILER_H