#ifndef SIMULATOR_H
#define	SIMULATOR_H

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <set>
#include <set>
#include <vector>

#include "common.h"

typedef std::set<int> si_t;
typedef std::vector<si_t> vsi_t;
typedef std::vector<int> vi_t;
typedef std::vector<vi_t> vvi_t;
typedef std::vector<double> vd_t;

#define sq(x) ((x)*(x))

class part_calc;

class Cache {
public:
  Cache(unsigned size):size(size){
    first = last = -1;
  }

  bool access(int data){
    if(first == -1){
      insert(data);
      return false;
    }
    bool ret = true;
    item it;
    it.value = data;
    if(cache.count(it) == 0){
      ret = false;
      if(cache.size() == size)
        remove(last);
    } else {
      remove(data);
    }
    insert(data);
    return ret;
  }
private:
  unsigned size;
  int first, last;
  
  struct item {
    int value, prev, next;
    
    bool operator<(const item& other) const {
      return this->value < other.value;
    }
  };
  
  std::set<item> cache;
  
  void remove(int data){
    item it;
    it.value = data;
    item found = *cache.find(it);
    assert(found.value == data);
    if(data == first){
      assert(found.prev == -1);
      first = found.next;
    } else {
      assert(found.prev != -1);
      it.value = found.prev;
      std::set<item>::iterator i = cache.find(it);
      it = *i;
      cache.erase(i);
      it.next = found.next;
      cache.insert(it);
    }
    if(data == last){
      assert(found.next == -1);
      last = found.prev;
    } else {
      assert(found.next != -1);
      it.value = found.next;
      std::set<item>::iterator i = cache.find(it);
      it = *i;
      cache.erase(i);
      it.prev = found.prev;
      cache.insert(it);
    }
    it.value = data;
    cache.erase(it);
  }

  void insert(int data){
    item it, second;
    if(first != -1){
      second.value = first;
      std::set<item>::iterator i = cache.find(second);
      it = *i;
      it.prev = data;
      cache.erase(i);
      cache.insert(it);
    } else {
      assert(last == -1);
      last = data;
    }

    it.value = data;
    it.prev = -1;
    it.next = first;
    first = data;
    cache.insert(it);
  }
};

// this class is the distributed system simulator
//   vertex identifiers are [0,N)
//   adj lists the neighboring vertices
//   P is the partitioning: a vector with (a set of partitions for each vertex)
class simulator {
public:
  simulator(part_calc *p_c, int n, int n_p, std::vector<Cache> *cache = NULL);
  virtual ~simulator();
  void loadGraph(std::istream& in);
  void loadPartitioning(std::istream& in);
  int first_partition(int i) const;
  int random_partition(int i) const;
  void khop_step(const vsi_t& data, vsi_t& ret);
  void rwalk_step(const vvi_t& data, vvi_t& ret);
  void print_info(std::ostream& data_out);
  int responsible(int i);
  void synchronize(vsi_t& data);
  void print_responsibles_histogram(std::ostream& data_out);
  void test_load();

private:
  int N, n_partitions;  // Number of vertices, partitions.
  vvi_t adj; // Adjacency lists.
  vsi_t P;   // Partitioning.
  part_calc *pc;
  std::vector<Cache> *cache;
  // Here we store statistics about the execution on each step.
  int n_steps;
  vvi_t load_cached, load_nocached, send_network, recv_network, sync_net;
};

inline int simulator::first_partition(int i) const {
  return *P[i].begin();
}
  
inline int simulator::random_partition(int i) const {
  si_t::iterator it = P[i].begin();
  std::advance(it, random_range(0, P[i].size()));
  return *it;
}

inline int simulator::responsible(int i){
  int ret = 0;
  for(si_t::iterator it = P[i].begin(); it != P[i].end(); ++it)
    ret += *it;
  si_t::iterator ret_it = P[i].begin();
  std::advance(ret_it, ret % P[i].size());
  return *ret_it;
}

// partition calculator
class part_calc {
public:
  virtual void part(const vsi_t& P, const int& p, const si_t& data, vi_t& ret, vvi_t& adj) = 0;
  virtual int choose_first(const simulator& sim, const int src){
    return sim.random_partition(src);
  }
};

bool simulate(int argc, char *argv[]);

#endif	/* SIMULATOR_H */

