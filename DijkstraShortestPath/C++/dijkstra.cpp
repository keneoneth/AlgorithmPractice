#include <iostream>
#include <utility>
#include <cfloat>
#include <queue>
#include <string>
#include <map>
#include <list>
#include <assert.h>

using namespace std;

//defines
#define NODE_NUM 10
#define EDGE_NUM 20

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

//define a strut of node measurement from source node S
struct NODE_M {
    int node;
    double dist_to_s;
    NODE_M (int n, double d) : node(n),dist_to_s(d) {}
    struct greater {
        bool operator() (const NODE_M& x, const NODE_M& y) const {return x.dist_to_s > y.dist_to_s;}
    };
    string repr() const {return "node:" + to_string(node) + " | dist:" + to_string(dist_to_s);}
};

// main function
int main() {

    // define graph
    Graph g;

    // rand gen data
    int edge_cnt = 0;
    srand(123);
    auto rd_double = [] (double a=0.0, double b=10.0) {
        return a + (double) rand() / RAND_MAX * (b-a);
    };

    auto rd_node = [] (int a=0, int b=NODE_NUM) {
        return a + (int) ((double) rand() / RAND_MAX * (double) (b-a));
    };

    for (int _=0; _<EDGE_NUM; _++){
        auto e = Edge(rd_node(),rd_node(),ceil(rd_double()));
        if(e.u!=e.v && !g.has_edge(e)) { //do not add edge if u==v
            g.add_edge(e);
            cout << "[dij] added edge : " << e.repr() << endl;
            edge_cnt++;
        }
    }

    cout << "[dij] prepared edge size : " << edge_cnt << endl;

    // define source node s
    const int s = rd_node();

    // store all distances from source s
    vector<double> dist_info_vec (NODE_NUM,DBL_MAX); //store distance s to key node
    dist_info_vec[s] = 0.0;

    cout << "[dij] source node : " << s << endl;
    cout << "[dij] prepared dist info of size : " << dist_info_vec.size() << endl;
 
    // build pri queue
    priority_queue<NODE_M,vector<NODE_M>,NODE_M::greater> min_priq_node;
    min_priq_node.push(NODE_M(s,0.0));

    // // find the shortest path from source s to each node
    map<int,int> prev_info_map; //store the prev node to key node

    while(min_priq_node.size()>0){
        auto min_node_m = min_priq_node.top(); 
        min_priq_node.pop();
        cout << "popped min_node:" << min_node_m.repr() << endl;
        for (const auto&neighbour_node : g.get_list(min_node_m.node)){
            double new_dist = dist_info_vec[min_node_m.node] + g.get_dist(min_node_m.node,neighbour_node);
            // cout << "new_dist" << new_dist << "| nn:" << neighbour_node << "," << dist_info_vec[neighbour_node] << endl;
            if (new_dist<dist_info_vec[neighbour_node]) {
                dist_info_vec[neighbour_node] = new_dist;
                prev_info_map[neighbour_node] = min_node_m.node;
                min_priq_node.push(NODE_M(neighbour_node,new_dist));
            }
        }
    }

    // // list all dist and prev info
    for (int i=0; i < NODE_NUM; i++){
        cout << "node : " << i << " | ";
        cout << "dist : " << dist_info_vec[i] << " | ";
        cout << "prev : " << ((prev_info_map.count(i)) ? prev_info_map[i] : -1) << endl;
    }


}