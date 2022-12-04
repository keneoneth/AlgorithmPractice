#include <iostream>
#include <climits>
#include <math.h>
#include <queue>
#include <cassert>
#include <cstdint>
#include <chrono>
#include <algorithm>
#include <random>

#include <utility>
#include <cfloat>
#include <queue>
#include <string>
#include <map>
#include <list>
#include <assert.h>

#include "boost/heap/priority_queue.hpp"
#include "boost/heap/pairing_heap.hpp"
#include "batch_pairing_heap_priqueue.h"
#include "fast_concurrent_pri_skipqueue.h"

#define _STR(x) #x
#define STR(x) _STR(x)
#define MAX_BATCH_SIZE 1000

#ifndef TCAND
#define TCAND A
#define TCAND_A
#endif

#ifndef TTYPE
#define TTYPE I
#define TTYPE_I
#endif

#ifndef TID
#define TID i
#define TID_i
#endif

enum TEST_CAND
{
    A,B,C,D,E,F,G
};

enum TEST_ID
{
    i,ii,iii,iv,v,vi
};

using namespace std;

//define a strut of node measurement from source node S
struct NODE_M {
    int node;
    double dist_to_s;
    NODE_M (int n, double d) : node(n),dist_to_s(d) {}
    // struct greater {
    //     bool operator() (const NODE_M& x, const NODE_M& y) const {return x.dist_to_s > y.dist_to_s;}
    // };
    friend bool operator>(const NODE_M &x, const NODE_M &y);
    friend bool operator<(const NODE_M &x, const NODE_M &y);
    string repr() const {return "node:" + to_string(node) + " | dist:" + to_string(dist_to_s);}
};




#if defined(TTYPE_I)
typedef int value_type;
#elif defined(TTYPE_II)
typedef NODE_M value_type;
#endif

#if defined(TCAND_A)
using chosen_pri_queue = std::priority_queue<value_type>;
#elif defined(TCAND_B)
using chosen_pri_queue = boost::heap::pairing_heap<value_type>;
#elif defined(TCAND_C)
using chosen_pri_queue = batch_pairing_heap_priqueue<value_type>;
#elif defined(TCAND_D) ||  defined(TCAND_E) ||  defined(TCAND_F) ||  defined(TCAND_G)
using chosen_pri_queue = fast_concurrent_pri_skipqueue<value_type>;
#endif


const size_t sample_size(TEST_ID id) {
    return pow(10,(3+static_cast<int>(id)));
}


//defines
// #define NODE_NUM 10
// #define EDGE_NUM 20

struct Edge{
    int u; // node u
    int v; // node v
    double d; //distance between node u and v
    Edge(int u, int v, double d) : u(u),v(v),d(d){}
    string repr() const {return "u:" + to_string(u) + " | v:" + to_string(v) + " | d:" + to_string(d);}
};


class Graph{
private:
    map<int,list<int>> adj_map;
    map<pair<int,int>,double> dist_map;
public:
    Graph(){}

    bool has_edge(Edge e) const {
        return dist_map.count(make_pair(e.u,e.v)) || dist_map.count(make_pair(e.v,e.u));
    }

    bool has_edge(int u, int v) const {
        return dist_map.count(make_pair(u,v)) || dist_map.count(make_pair(v,u));
    }

    void add_edge(Edge e){
        assert(!has_edge(e)); // do not add repeated edge
        adj_map[e.u].push_back(e.v); // u -> v direction
        adj_map[e.v].push_back(e.u); // v -> u direction
        dist_map[make_pair(e.u,e.v)] = e.d;
    }

    const list<int> get_list(int node) const{
        return (adj_map.count(node)) ? adj_map.at(node) : list<int>(); 
    }

    const double get_dist(int u, int v) const {
        assert(has_edge(u,v));
        if (dist_map.count(make_pair(u,v))) {
            return dist_map.at(make_pair(u,v));
        } else {
            return dist_map.at(make_pair(v,u));
        }
    }

};



bool operator>(const NODE_M &x, const NODE_M &y)
{
   //cout << "in >"  << endl;
   return x.dist_to_s > y.dist_to_s;
}

bool operator<(const NODE_M &x, const NODE_M &y)
{
   //cout << "in >"  << endl;
   return x.dist_to_s < y.dist_to_s;
}

int main()
{
    srand(123);

    printf("[TEST] {\"cand\": [\"%s\"], \"type\": [\"%s\"], \"id\": [\"%s\"]}\n", STR(TCAND), STR(TTYPE), STR(TID));

    bool skip_test = false;
    chosen_pri_queue pq;
    const size_t N = sample_size(TID);
    printf("[TST] {\"size\": [%lu]}\n",N);
    uint32_t total_cnt = N;
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    
//// random add and pop elements
#if defined(TTYPE_I)
    
    while (total_cnt!=0 || !pq.empty()) {
        // random add some elements
        if (total_cnt>0){
            auto push_batch_size = 1 + rand() % total_cnt;
            for(uint32_t n=0;n<push_batch_size;n++) {
                pq.push(total_cnt);
                total_cnt -= 1;
            }
        }
        auto pop_batch_size = 1 + rand() % pq.size();
        for(uint32_t n=0;n<pop_batch_size;n++) {
            pq.pop();
        }
    }

#endif // TTYPE_I


#if defined(TTYPE_II)

    // define graph
    Graph g;

    static uint32_t NODE_NUM = N;
    static uint32_t EDGE_NUM = 2*N;
    // rand gen data
    int edge_cnt = 0;
    srand(123);

    auto rd_double = [] (double a=0.0, double b=10.0) {
        return a + (double) rand() / RAND_MAX * (b-a);
    };

    auto rd_node = [] (int a, int b) {
        return a + (int) ((double) rand() / RAND_MAX * (double) (b-a));
    };

    for (int _=0; _<EDGE_NUM; _++){
        auto e = Edge(rd_node(0,NODE_NUM),rd_node(0,NODE_NUM),ceil(rd_double()));
        if(e.u!=e.v && !g.has_edge(e)) { //do not add edge if u==v
            g.add_edge(e);
            // cout << "[dij] added edge : " << e.repr() << endl;
            edge_cnt++;
        }
    }

    cout << "[dij] prepared edge size : " << edge_cnt << endl;

    // define source node s
    const int s = rd_node(0,NODE_NUM);

    // store all distances from source s
    vector<double> dist_info_vec (NODE_NUM,DBL_MAX); //store distance s to key node
    dist_info_vec[s] = 0.0;

    cout << "[dij] source node : " << s << endl;
    cout << "[dij] prepared dist info of size : " << dist_info_vec.size() << endl;
 
    // build pri queue
    // priority_queue<NODE_M,vector<NODE_M>,NODE_M::greater> min_priq_node;
    // priority_queue<NODE_M> min_priq_node;

    pq.push(NODE_M(s,0.0));

    // find the shortest path from source s to each node
    map<int,int> prev_info_map; //store the prev node to key node

    while(pq.size()>0){
        auto min_node_m = pq.top(); 
        pq.pop();
        // cout << "popped min_node:" << min_node_m.repr() << endl;
        for (const auto&neighbour_node : g.get_list(min_node_m.node)){
            double new_dist = dist_info_vec[min_node_m.node] + g.get_dist(min_node_m.node,neighbour_node);
            // cout << "new_dist" << new_dist << "| nn:" << neighbour_node << "," << dist_info_vec[neighbour_node] << endl;
            if (new_dist<dist_info_vec[neighbour_node]) {
                dist_info_vec[neighbour_node] = new_dist;
                prev_info_map[neighbour_node] = min_node_m.node;
                pq.push(NODE_M(neighbour_node,new_dist));
            }
        }
    }

     // list all dist and prev info
    cout << "[dij] min dist to each node and their prev node : ";
    for (int i=0; i < NODE_NUM; i++){
        cout << "node : " << i << " | ";
        cout << "dist : " << dist_info_vec[i] << " | ";
        cout << "prev : " << ((prev_info_map.count(i)) ? prev_info_map[i] : -1) << endl;
    }


#endif // TTYPE_II

    assert(pq.size()==0);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    uint64_t elapsed_time_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    if (!skip_test)
        printf("[TEST] {\"elapsed_time_us\": [%lu]}",elapsed_time_us);
    else
        printf("[TEST] {\"elapsed_time_us\": [\"nan\"]}");
    return 0;
}