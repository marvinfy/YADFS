/* 
 * File:   main.cpp
 * Author: marcusviniciusns
 *
 * Created on May 7, 2011, 11:11 PM
 */

#include <stdio.h>
#include <cstdlib>
#include <iostream>

using namespace std;

/*
 * 
 */

int main(int argc, char** argv)
{

  

  if (argc != 3)
  {
    return -1;
  }

  const char *path = argv[1];
  size_t size = (size_t) atoi(argv[2]);

  FILE *fd = fopen(path, "w");
  if (fd == NULL)
  {
    return -1;
  }

  srand(time(NULL));

  int chr;
  int i;
  for (i = 0; i < size; i++)
  {
    chr = rand() %  93 + 32; // 32 - 125
    fwrite(&chr, 1, 1, fd);
  }
  fclose(fd);

  return 0;
}
