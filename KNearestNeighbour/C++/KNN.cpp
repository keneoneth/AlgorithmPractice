#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <assert.h>

using namespace std;

//defines
#define K_NUM 3
#define CLASS_NUM 3
#define DATA_POINT_NUM 10


//define a data point class
class DataPoint{
private:
    double x,y;
    string _class; 
public:
    DataPoint(double x, double y, string c) : x(x),y(y),_class(c) {}
    double cal_non_sqrt_dist(DataPoint dp) {
        //assume that the powered distance is less than DOUBLE_MAX :)
        //no need to square the distance to reduce computation
        //TODO: implement an overflow check to ensure
        return pow(dp.x-x,2)+pow(dp.y-y,2);
    }
    string get_class() const{return _class;}
};

//define a struct of measurement
struct DATA_POINT_M{
    double non_sqrt_dist;
    string _class;
    DATA_POINT_M(double nsd, string c) : non_sqrt_dist(nsd),_class(c){}
    struct smaller {
        bool operator() (const DATA_POINT_M& x, const DATA_POINT_M& y) const {return x.non_sqrt_dist<y.non_sqrt_dist;}
    };
    string repr() const {return "class:"+_class+" | nsd:"+to_string(non_sqrt_dist);}
};



//main function
int main() {
    
    // gen data points
    srand(123);
    auto rd_double = [] (double a=0.0, double b=100.0) {
        return a + (double) rand() / RAND_MAX * (b-a);
    };

    auto rd_class = [] (int a=0, int b=CLASS_NUM) {
        return a + (int) ((double) rand() / RAND_MAX * (double) (b-a));
    };

    vector<DataPoint> vec_dp;

    for (int _=0; _<DATA_POINT_NUM; _++){
        auto dp = DataPoint(rd_double(),rd_double(),to_string(rd_class()));
        vec_dp.push_back(dp);
    }

    cout << "[KNN] prepared data point num : " << vec_dp.size() << endl;


    // check dist between target data point and the rest of them
    // preserve the K data points that have closest distance to target data point
    auto target_dp = DataPoint(rd_double(),rd_double(),to_string(rd_class()));

    priority_queue<DATA_POINT_M,vector<DATA_POINT_M>,DATA_POINT_M::smaller> max_priq_dp;

    for (const auto &dp:vec_dp) {
        auto dpm = DATA_POINT_M(target_dp.cal_non_sqrt_dist(dp),dp.get_class());
        cout << "select dpm: " << dpm.repr() << endl;
        // if(max_priq_dp.size()>0)cout << "top dpm: " << max_priq_dp.top().repr() << endl;
        if (max_priq_dp.size()<K_NUM||max_priq_dp.top().non_sqrt_dist > dpm.non_sqrt_dist){
            max_priq_dp.push(dpm);
            if(max_priq_dp.size()>K_NUM) {max_priq_dp.pop();}
        }
    }

    assert(max_priq_dp.size()==K_NUM); //safety check

    cout << "[KNN] filtered data point num : " << max_priq_dp.size() << endl;


    //find the majority class of the data points
    map<string,int> class_count_map;
    int max_cnt = 1;
    while(max_priq_dp.size()>0){
        if (class_count_map.count(max_priq_dp.top()._class)){
            class_count_map[max_priq_dp.top()._class] += 1;
            max_cnt = max(max_cnt,class_count_map[max_priq_dp.top()._class]);
        } else {
            class_count_map[max_priq_dp.top()._class] = 1;
        }
        cout << "fil dpm: " << max_priq_dp.top().repr() << endl;
        max_priq_dp.pop();
    }

    //print out freq of each class
    cout << "[KNN] (class x count) : " << endl;
    for (const auto &elem:class_count_map){
        cout << "class " << elem.first << " x " << elem.second << endl;
    }

    //print out the classes of max value
    cout << "[KNN] class(es) of target data point : ";
    for (const auto &elem:class_count_map){
        if (elem.second==max_cnt){
            cout << elem.first << " ;";
        }
    }
    cout << endl;

    return 0;
}