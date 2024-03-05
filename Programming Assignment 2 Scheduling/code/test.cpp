#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>

using namespace std;

int main() {
    int latency = 5;
    vector<vector<int>> a;
    vector<int> b(1);
    for(int i=0; i<latency; i++) {
        a.push_back(b);
    }
    cout << a[0].size();
}