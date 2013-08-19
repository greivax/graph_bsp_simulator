#include "simulator.h"

simulator::simulator(part_calc *p_c, int n, int n_p, std::vector<Cache> *cache)
    :N(n), n_partitions(n_p), pc(p_c), cache(cache){
  n_steps = 0;
}
  
simulator::~simulator(){
}

#include <iostream>

void simulator::loadGraph(std::istream& in){
  adj.clear();
  adj = vvi_t(N);
  int total_edges = 0;
  for(int i = 0; i < N; ++i){
    int n_edges;
    in >> n_edges;
    total_edges += n_edges;
    adj[i] = vi_t(n_edges);
    for(int j = 0; j < n_edges; ++j)
      in >> adj[i][j];
  }
  std::cout << "Graph loaded, " << N << " vertices, " << total_edges << " edges" << std::endl;
}
  
void simulator::loadPartitioning(std::istream& in){
  P.clear();
  P = vsi_t(N, si_t());
  for(int i = 0; i < N; ++i){
    int p, n_p;
    in >> n_p;
    for(int j = 0; j < n_p; ++j){
      in >> p;
      P[i].insert(p);
    }
  }
}
  
// execute a step (hop) on the set of vertices
void simulator::khop_step(const vsi_t& data, vsi_t& ret){
  ret.clear();
  ret = vsi_t(n_partitions);
  vi_t current_load_cached(n_partitions, 0);
  vi_t current_load_nocached(n_partitions, 0);
  vi_t current_send_network(n_partitions, 0);
  vi_t current_recv_network(n_partitions, 0);
  for(int p = 0; p < n_partitions; ++p){
    si_t next_vertices;
    for(si_t::iterator it = data[p].begin(); it != data[p].end(); ++it){
      bool cached = false;
      if(cache != NULL)
        cached = (*cache)[p].access(*it);
      if(cached)
        current_load_cached[p] += adj[*it].size();
      else
        current_load_nocached[p] += adj[*it].size();
      for(unsigned j = 0; j < adj[*it].size(); ++j)
        next_vertices.insert(adj[*it][j]);
    }
    vi_t next_partitions(N, -1);
    pc->part(P, p, next_vertices, next_partitions, adj);
    // update current_network
    for(si_t::iterator it = next_vertices.begin(); it != next_vertices.end(); ++it){
      if(p != next_partitions[*it]){
        ++current_send_network[p];
        ++current_recv_network[next_partitions[*it]];
      }
    }
    // put the next vertices on the corresponding set
    for(si_t::iterator it = next_vertices.begin(); it != next_vertices.end(); ++it)
      ret[next_partitions[*it]].insert(*it);
  }
  load_cached.push_back(current_load_cached);
  load_nocached.push_back(current_load_nocached);
  send_network.push_back(current_send_network);
  recv_network.push_back(current_recv_network);
  ++n_steps;
}

void simulator::rwalk_step(const vvi_t& data, vvi_t& ret){
  ret.clear();
  ret = vvi_t(n_partitions);
  vi_t current_load_cached(n_partitions, 0);
  vi_t current_load_nocached(n_partitions, 0);
  vi_t current_send_network(n_partitions, 0);
  vi_t current_recv_network(n_partitions, 0);
  for(int p = 0; p < n_partitions; ++p){
    vi_t vi_next_vertices;
    for(vi_t::const_iterator it = data[p].begin(); it != data[p].end(); ++it){
      bool cached = false;
      if(cache != NULL)
        cached = (*cache)[p].access(*it);
      if(cached)
        ++current_load_cached[p];
      else
        ++current_load_nocached[p];
      if(adj[*it].size())
        vi_next_vertices.push_back(adj[*it][random_range(0, adj[*it].size())]);
    }
    si_t si_next_vertices(vi_next_vertices.begin(), vi_next_vertices.end());
    vi_t next_partitions(N, -1);
    pc->part(P, p, si_next_vertices, next_partitions, adj);
    // update current_network
    for(vi_t::iterator it = vi_next_vertices.begin(); it != vi_next_vertices.end(); ++it){
      if(p != next_partitions[*it]){
        ++current_send_network[p];
        ++current_recv_network[next_partitions[*it]];
      }
    }
    // put the next vertices on the corresponding set
    for(vi_t::iterator it = vi_next_vertices.begin(); it != vi_next_vertices.end(); ++it)
      ret[next_partitions[*it]].push_back(*it);
  }
  load_cached.push_back(current_load_cached);
  load_nocached.push_back(current_load_nocached);
  send_network.push_back(current_send_network);
  recv_network.push_back(current_recv_network);
  ++n_steps;
}

void simulator::print_info(std::ostream& data_out){
  data_out.precision(4);
  for(int i = 0; i < n_steps; ++i){
    for(int j = 0; j < n_partitions; ++j)
      data_out << load_cached[i][j] << ' ' << load_nocached[i][j] << ' ' << send_network[i][j] << ' ' << recv_network[i][j] << ' ' << (sync_net.size()?sync_net[i][j]:0) << ' ';
    data_out << std::endl;
  }
}

void simulator::synchronize(vsi_t& data){
  assert(n_partitions == (int) data.size());
  vi_t net(n_partitions, 0);
  for(int vertex = 0; vertex < N; ++vertex){
    vi_t partitions;
    for(unsigned j = 0; j < data.size(); ++j)
      if(data[j].count(vertex) > 0)
        partitions.push_back(j);
    if(partitions.size() > 0){
      unsigned p = random_range(0, partitions.size());
      for(unsigned j = 0; j < partitions.size(); ++j){
        if(j != p)
          data[partitions[j]].erase(vertex);
        if(responsible(vertex) != partitions[j])
          net[partitions[j]] += 2;
      }
    }
  }
  sync_net.push_back(net);
}

void simulator::print_responsibles_histogram(std::ostream& data_out){
  vi_t h(n_partitions, 0);
  for(int i = 0; i < N; ++i)
    h[responsible(i)]++;
  for(int i = 0; i < n_partitions; ++i)
    data_out << ' ' << h[i];
  data_out << std::endl;
}

#include <sys/time.h>

void simulator::test_load(){
  timeval tim;
  gettimeofday(&tim, NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  si_t R;
  for(unsigned i = 0; i < adj.size(); ++i)
    for(unsigned j = 0; j < adj[i].size(); ++j)
      R.insert(adj[i][j]);
  gettimeofday(&tim, NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  double total_time = t2 - t1;

  std::cout << "vertices exploded: " << adj.size() << std::endl;
  std::cout << "result size: " << R.size() << std::endl;
  std::cout << "total time: " << total_time << std::endl;
  std::cout << "mean time: " << total_time / (double) adj.size() << std::endl;
}
