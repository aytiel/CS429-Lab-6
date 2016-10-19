//Alexander Lo atl766

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "cachelab.h"

const char cacheOpt[] = "s:E:b:t:v";

typedef struct
{
  unsigned long long label;
  int time;
  char valid;
} cacheLine;

cacheLine *cache;
int s, E, b, t = 0, v;
int hit_count = 0, miss_count = 0, eviction_count = 0;
char *traceFile;

void getInput(int argc, char *argv[])
{
  int c = 0, n;
  while((c = getopt(argc, argv, cacheOpt)) != -1)
  {
    if(optarg != NULL)
      n = atoi(optarg);
    switch(c)
    {
      case 's':
        s = n;
        break;
      case 'E':
        E = n;
        break;
      case 'b':
        b = n;
        break;
      case 't':
        traceFile = optarg;
        break;
      case 'v':
        v = 1;
        break;
      default:
        break;
    }
  }
}

void runSim(unsigned long long address)
{
  unsigned long long label = address >> b;
  int index = E * (label & ((1 << s) - 1));
  int i, recUp = ++t, empty = -1;
  for(i = 0; i < E; i++)
  {
    if(cache[index + i].valid)
    {
      if(cache[index + i].label == label)
      {
        cache[index + i].time = t;
        hit_count++;
        if(v)
          printf(" hit");
        return;
      }
      if(cache[index + i].time < recUp)
      {
        recUp = cache[index + i].time;
        empty = i;
      }
    }
    else
    {
      empty = i;
      recUp = t;
      break;
    }
  }
  miss_count++;
  if(v)
    printf(" miss");
  if(recUp != t)
  {
    eviction_count++;
    if(v)
      printf(" eviction");
  } 
  cache[index + empty].valid = 1;
  cache[index + empty].time = t;
  cache[index + empty].label = label;
}

int main(int argc, char *argv[])
{
  FILE *filePath;
  char cmd;
  unsigned long addr;
  int size;

  getInput(argc, argv);
 
  cache = (cacheLine *) malloc(sizeof(cacheLine) * E * (1 << s));
  memset(cache, 0, sizeof(cacheLine) * E * (1 << s));
  
  filePath = fopen(traceFile, "r"); 
  if(filePath != NULL)
  {
    while(fscanf(filePath, " %c %lx,%d", &cmd, &addr, &size) == 3)
    {
      switch(cmd)
      {
        case 'I':
          break;
        case 'L':
          if(v)
            printf("%c", cmd);
          runSim(addr);
          break;
        case 'S':
          if(v)
            printf("%c", cmd);
          runSim(addr);
          break;
        case 'M':
          if(v)
            printf("%c", cmd);
          runSim(addr);
          runSim(addr);
          break;
        default:
          break;
      }
      if(v)
        printf("\n");
    } 
  }
  printSummary(hit_count, miss_count, eviction_count);
  fclose(filePath);
  free(cache);
  return 0;
}
