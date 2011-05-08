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
  if (argc != 4)
  {
    printf("rand out.txt byte_count 1 (random)\n");
    printf("rand out.txt byte_count 2 (4096 seq)\n");
    return -1;
  }

  const char *path = argv[1];
  size_t size = (size_t) atoi(argv[2]);
  int mode = atoi(argv[3]);

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
    if (mode == 1)
    {
      chr = rand() %  93 + 32; // 32 - 125
    }
    else
    {
        chr = (i / 4096) + 65;
    }
    fwrite(&chr, 1, 1, fd);
  }
  fclose(fd);

  return 0;
}
