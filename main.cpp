#include "simulator.h"

bool simulate(int argc, char *argv[]);

int main(int argc, char** argv){
  bool usage = false;
  for(int i = 1; i < argc and not usage; i++)
    if(!strcmp(argv[i], "-h"))
      usage = true;
  
  if(not simulate(argc, argv))
    usage = true;

  if(usage){
    printf("usage: %s [OPTIONS]\n", argv[0]);
    printf("   -h      show this message\n");
    printf(" Simulator options\n");
    printf("   -g       <file>  load graph from <file>\n");
    printf("   -hr      <type>  heuristic to use to distribute load\n");
    printf("                    valid types are:\n");
    printf("                      'stay'\n");
    printf("                      'rand'\n");
    printf("                      'mixed'\n");
    printf("                      'balance'\n");
    printf("                      'wbalance'\n");
    printf("   -pr      <prob>  specify probability of stay with mixed heuristic\n");
    printf("   -khop    K       specifies the k-hops\n");
    printf("   -n       N       specifies the number of vertices\n");
    printf("   -np      NP      specifies the number of partitions\n");
    printf("   -p       <file>  load partitioning from <file>\n");
    printf("   -q       query   query to execute. Valid queries:\n");
    printf("                      'khop'\n");
    printf("                      'rwalk'\n");
    printf("                      'pagerank'\n");
    printf("   -qsrc    <file>  file with list of starting vertices\n");
    printf("   -sync            enables sync before computation phase\n");
    printf("   -soutput <file>  file to save the statistics output\n");
    printf("   All options except -sync are mandatory\n");
    return 1;
  }

  return 0;
}
