//
// Created by relaybot on 3/24/17.
//

#ifndef CDN_BLDE_H
#define CDN_BLDE_H

#define N 999
#include <vector>

using namespace std;

class BLDE {
private:
    vector<vector<int>> present_population;//n individuals
    vector<vector<int>> last_population;//n individuals
    int generation_number;
    int population_number;
    int bit_number;
    double p;
    vector<int> global_best_individual;
public:
    BLDE();//iniatal param
    void initialParam(int _population_number,int _bit_number);
    //void setStopCriterion();
    void step();
    vector<int> getBetterIndidual(const vector<int> &x, const vector<int> &y);
    vector<int> getBestIndidual();
    virtual int Objective(const vector<int>&) = 0;

};


#endif //CDN_BLDE_H
