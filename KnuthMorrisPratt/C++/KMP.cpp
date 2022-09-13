#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <vector>

using namespace std;

template <typename T>
void print_vec(const vector<T> &v){
    for (auto e:v){
        cout << e << ",";
    }
    cout << endl;
}

void build_table(vector<int> &T, string W){
    /*
    Table T can be interpreted as a lookup table when mismatch occurs
    e.g.
    if T[i] = -1, skip checking current index 
    if T[i] = 0, goto check the first char of W
    if T[i] = 1, goto check the second char of W
    etc.
    */
    int pos = 1;
    int cnd = 0;

    T.resize(W.size()+1);
    
    T[cnd] = -1;

    while (pos < W.size()) {
        if (W[pos]==W[cnd]) {
            // if same as the first substring segment
            T[pos] = T[cnd];
        } else {
            // if different from the first substring segment
            T[pos] = cnd;
            while (cnd>=0 && W[pos]!=W[cnd]) {
                cnd = T[cnd];
                // if W[pos] == first char of W, cnd = -1; else, cnd = 0 
            }
        }
        pos += 1;
        cnd += 1;
    }

    T[pos] = cnd;
}



int main(int argc, char** argv) {

    if (argc >= 3) {
        
        vector<string> arg_list(argv, argv+argc);

        string S = arg_list[1];
        string W = arg_list[2];

        cout << "[kmp] input string: " << S << endl;
        cout << "[kmp] search string: " << W << endl;

        int j = 0; // position of cur char in S
        int k = 0; // position of cur char in W

        vector<int> T; //precompute table T
        build_table(T,W);

        cout << "precompute table T: ";
        print_vec(T); // show table
        
        vector<int> P;

        int nP = 0;
        while (j < S.size()) {
            if (W[k] == S[j]) {
                j += 1;
                k += 1;
                if ( k == W.size()) {
                    P.push_back(j-k);
                    nP += 1;
                    k = T[k]; // goto find next match
                }
            } else {
                k = T[k];
                if (k < 0) {
                    //skip cur search
                    j++;
                    k++;
                }
            }
        }

        cout << "matching indexes: ";
        print_vec(P); // show table
        cout << "total matches: " << nP << endl;

    } else {
        cout << "[warning] please enter input string W and search string S as the first two arguments" << endl;
    }


}