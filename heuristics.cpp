#include "simulator.h"
#include "common.h"
#include<iostream>

// Stay on same partition if possible.
class part_stay : public part_calc {
public:
  part_stay(){
  }

  void part(const vsi_t& P, const int& p, const si_t& data, vi_t& ret, vvi_t& adj){
    for(si_t::iterator it = data.begin(); it != data.end(); ++it){
      if(P[*it].count(p) > 0){
        ret[*it] = p;
      } else {
        si_t::iterator ret_it = P[*it].begin();
        std::advance(ret_it, random_range(0, P[*it].size()));
        ret[*it] = *ret_it;
      }
    }
  }
};

class part_rand : public part_calc {
public:
  part_rand(){
  }

  void part(const vsi_t& P, const int& p, const si_t& data, vi_t& ret, vvi_t& adj){
    for(si_t::iterator it = data.begin(); it != data.end(); ++it){
      si_t::iterator ret_it = P[*it].begin();
      std::advance(ret_it, random_range(0, P[*it].size()));
      ret[*it] = *ret_it;
    }
  }
};

class part_prob : public part_calc {
private:
  double pr;
public:
  part_prob(double p) : pr(p){
  }

  void part(const vsi_t& P, const int& p, const si_t& data, vi_t& ret, vvi_t& adj){
    for(si_t::iterator it = data.begin(); it != data.end(); ++it){
      if(P[*it].count(p) > 0 and random_double() < pr){
        ret[*it] = p;
      } else {
        si_t::iterator ret_it = P[*it].begin();
        std::advance(ret_it, random_range(0, P[*it].size()));
        ret[*it] = *ret_it;
      }
    }
  }
};

class part_balance : public part_calc {
private:
  static int pos_min(vi_t& data){
    int ret = 0, vmin = data[0];
    for(unsigned i = 1; i < data.size(); ++i) if(data[i] < vmin){
      vmin = data[i];
      ret = i;
    }
    return ret;
  }
  
  static int pos_max(vi_t& data){
    int ret = 0, vmax = data[0];
    for(unsigned i = 1; i < data.size(); ++i) if(data[i] > vmax){
      vmax = data[i];
      ret = i;
    }
    return ret;
  }
public:
  part_balance(){
  }

  void part(const vsi_t& P, const int& p, const si_t& data, vi_t& ret, vvi_t& adj){
    if(data.size() == 0)
      return;
    int k = -1;
    for(unsigned i = 0; i < P.size(); ++i)
      for(si_t::const_iterator it = P[i].begin(); it != P[i].end(); ++it)
        if(*it > k)
          k = *it;
    ++k;
    int total = 0;
    vi_t dist(k, 0);
    // Initial distribution.
    for(si_t::iterator it = data.begin(); it != data.end(); ++it){
      int partition = *P[*it].begin();
      assert(partition >= 0 and partition < k);
      ret[*it] = partition;
      dist[partition]++;
      ++total;
    }
    // Balancing.
    int pmin, pmax, cont = false;
    do {
      pmin = pos_min(dist);
      pmax = pos_max(dist);
      assert(pmin >= 0 and pmin < k);
      assert(pmax >= 0 and pmax < k);
      cont = false;
      if(dist[pmax] - dist[pmin] > 1){
        for(si_t::const_iterator data_it = data.begin(); data_it != data.end(); ++data_it) if(ret[*data_it] == pmax){
          bool current_vertex_moved = false;
          for(si_t::const_iterator p_it = P[*data_it].begin(); p_it != P[*data_it].end() and not current_vertex_moved; ++p_it){
            int partition = *p_it;
            assert(partition >= 0 and partition < k);
            if(dist[partition] < dist[pmax] - 1){
              ret[*data_it] = partition;
              dist[partition]++;
              dist[pmax]--;
              current_vertex_moved = true;
              cont = true;
            }
          }
        }
      }
    } while(cont);
  }
};

class part_wbalance : public part_calc {
private:
  static int pos_min(vi_t& data){
    int ret = 0, vmin = data[0];
    for(unsigned i = 1; i < data.size(); ++i) if(data[i] < vmin){
      vmin = data[i];
      ret = i;
    }
    return ret;
  }
  
  static int pos_max(vi_t& data){
    int ret = 0, vmax = data[0];
    for(unsigned i = 1; i < data.size(); ++i) if(data[i] > vmax){
      vmax = data[i];
      ret = i;
    }
    return ret;
  }
public:
  part_wbalance(){
  }

  void part(const vsi_t& P, const int& p, const si_t& data, vi_t& ret, vvi_t& adj){
    if(data.size() == 0)
      return;
    int k = -1;
    for(unsigned i = 0; i < P.size(); ++i)
      for(si_t::const_iterator it = P[i].begin(); it != P[i].end(); ++it)
        if(*it > k)
          k = *it;
    ++k;
    int total = 0;
    vi_t dist(k, 0);
    // Initial distribution.
    for(si_t::iterator it = data.begin(); it != data.end(); ++it){
      int partition = *P[*it].begin();
      assert(partition >= 0 and partition < k);
      ret[*it] = partition;
      dist[partition] += adj[*it].size();
      ++total;
    }
    // Balancing.
    int pmin, pmax, cont = false;
    do {
      pmin = pos_min(dist);
      pmax = pos_max(dist);
      assert(pmin >= 0 and pmin < k);
      assert(pmax >= 0 and pmax < k);
      cont = false;
      if(dist[pmax] - dist[pmin] > 1){
        for(si_t::const_iterator data_it = data.begin(); data_it != data.end(); ++data_it) if(ret[*data_it] == pmax){
          bool current_vertex_moved = false;
          for(si_t::const_iterator p_it = P[*data_it].begin(); p_it != P[*data_it].end() and not current_vertex_moved; ++p_it){
            int partition = *p_it;
            assert(partition >= 0 and partition < k);
            if(dist[partition] + adj[*data_it].size() <= dist[pmax] - adj[*data_it].size()){
              ret[*data_it] = partition;
              dist[partition] += adj[*data_it].size();
              dist[pmax] -= adj[*data_it].size();
              current_vertex_moved = true;
              cont = true;
            }
          }
        }
      }
    } while(cont);
  }
};

bool simulate(int argc, char *argv[]){
  char *fnode, *fpart, *heuristic, *ostfile, *fqueries, *query;
  int n, np, nq, k_hop, cache_size;
  double probability;
  bool sync_flag;
  fnode = fpart = heuristic = ostfile = fqueries = query = NULL;
  n = np = nq = k_hop = cache_size = -1;
  probability = -1.0;
  sync_flag = false;

  for(int i = 0; i < argc; ++i)
    if(!strcmp(argv[i], "-h")){
    } else if(!strcmp(argv[i], "-g")){ // graph
      ++i;
      fnode = argv[i];
    } else if(!strcmp(argv[i], "-hr")){ // heuristic
      ++i;
      heuristic = argv[i];
    } else if(!strcmp(argv[i], "-pr")){ // probability
      ++i;
      probability = atof(argv[i]);
    } else if(!strcmp(argv[i], "-khop")){ // number of hops
      ++i;
      k_hop = atoi(argv[i]);
    } else if(!strcmp(argv[i], "-n")){ // number of vertices
      ++i;
      n = atoi(argv[i]);
    } else if(!strcmp(argv[i], "-np")){ // number of partitions
      ++i;
      np = atoi(argv[i]);
    } else if(!strcmp(argv[i], "-nq")){ // number of queries
      ++i;
      nq = atoi(argv[i]);
    } else if(!strcmp(argv[i], "-p")){ // partitioning file
      ++i;
      fpart = argv[i];
    } else if(!strcmp(argv[i], "-q")){ // query
      ++i;
      query = argv[i];
    } else if(!strcmp(argv[i], "-qsrc")){ // query source file
      ++i;
      fqueries = argv[i];
    } else if(!strcmp(argv[i], "-sync")){ // synchronize or not
      sync_flag = true;
    } else if(!strcmp(argv[i], "-cache")){
      ++i;
      cache_size = atoi(argv[i]);
    } else if(!strcmp(argv[i], "-soutput")){ // output
      ++i;
      ostfile = argv[i];
    }

  if(fnode == NULL or fpart == NULL or heuristic == NULL or ostfile == NULL or fqueries == NULL or query == NULL)
    return false;
  if(n == -1 or np == -1 or nq == -1 or k_hop == -1)
    return false;

  part_calc *pc;
  if(!strcmp(heuristic, "rand")){
    pc = new part_rand();
  } else if(!strcmp(heuristic, "stay")){
    pc = new part_stay();
  } else if(!strcmp(heuristic, "mixed")){
    if(probability < 0.0){
      printf("please specify probability (-pr) with mixed heuristic\n");
      return true;
    }
    pc = new part_prob(probability);
  } else if(!strcmp(heuristic, "balance")){
    pc = new part_balance();
  } else if(!strcmp(heuristic, "wbalance")){
    pc = new part_wbalance();
  } else{
    return false;
  }

  srand(0);
  std::vector<Cache> v_cache(32, Cache(cache_size));
  std::vector<Cache> *v_cache_pointer = NULL;
  if(cache_size != -1){
    std::cout << "Using cache" << std::endl;
    v_cache_pointer = &v_cache;
  }
  simulator sim(pc, n, np, v_cache_pointer);

  std::ifstream ifnode(fnode);
  sim.loadGraph(ifnode);
  ifnode.close();
  std::ifstream ifpart(fpart);
  sim.loadPartitioning(ifpart);
  ifpart.close();
  sim.print_responsibles_histogram(std::cout);
  std::ifstream ifqueries(fqueries);
  int src;
  if(!strcmp(query, "khop")){
    while(nq-- and ifqueries >> src){
      vsi_t data(np), ret;
      data[pc->choose_first(sim, src)].insert(src);
      for(int k = 0; k < k_hop; ++k){
        sim.khop_step(data, ret);
        data = ret;
        if(sync_flag){
          sim.synchronize(data);
        }
      }
    }
  } else if(!strcmp(query, "pagerank")){
    while(nq--){
      vsi_t data(np), ret;
      for(int i = 0; i < n; ++i) // fill the partitions with all vertices
        data[pc->choose_first(sim, i)].insert(i);
      for(int k = 0; k < k_hop; ++k){
        sim.khop_step(data, ret);
        data = ret;
        if(sync_flag){
          sim.synchronize(data);
        }
      }
    }
  } else if(!strcmp(query, "rwalk")){
    vvi_t data(np), ret;
    while(nq-- and ifqueries >> src)
      data[pc->choose_first(sim, src)].push_back(src);
    for(int k = 0; k < k_hop; ++k){
      sim.rwalk_step(data, ret);
      data = ret;
    }
  } else if(!strcmp(query, "test")){
    sim.test_load();
  } else {
    printf("invalid query %s\n", query);
    return true;
  }
  ifqueries.close();
  std::ofstream st_out(ostfile);
  sim.print_info(st_out);
  delete pc;
  st_out.close();

  return true;
}
