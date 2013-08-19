#ifndef COMMON_H
#define	COMMON_H

#include <cassert>

inline int random_range(int a, int b){
  return a + (rand() % (b - a));
}

inline double random_double(){
  return rand() / (double) RAND_MAX;
}

inline double NextDouble(){
  return random_double();
}

#endif	/* COMMON_H */
