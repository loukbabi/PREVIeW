//============================================================================
// Name        : tictoc.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef TIC_TOC_H
#define TIC_TOC_H

#include <stack>	// To use stack container

// Shared Timer
extern std::stack<timespec> tic_stack;

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

// Starts the clock
inline void tic() {
  timespec start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  tic_stack.push(start);
}

// Stops the clock and prints the time difference in seconds and nanoseconds
inline void toc(float testSize) {
  timespec end;
  clock_gettime(CLOCK_MONOTONIC, &end);
  double temp_time = (diff(tic_stack.top(), end).tv_sec * 1000 + diff(tic_stack.top(), end).tv_nsec/1000000.0)/testSize;
  PRINTF_PLATFORM("Time elapsed in msec for one output: %lf\n", temp_time);
  tic_stack.pop();
}

inline void toc() {
  timespec end;
  clock_gettime(CLOCK_MONOTONIC, &end);
  double temp_time  = (diff(tic_stack.top(), end).tv_sec * 1000 + diff(tic_stack.top(), end).tv_nsec/1000000.0);
  PRINTF_PLATFORM("Time elapsed in msec: %lf\n", temp_time);
  tic_stack.pop();
}

#endif // TIC_TOC_H
