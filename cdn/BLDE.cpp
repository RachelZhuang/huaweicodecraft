//
// Created by relaybot on 3/24/17.
//

#include <cstdlib>
#include <cmath>
#include <ctime>
#include "BLDE.h"

BLDE::BLDE(){
   // a=rand()%2; //a为随机数除以2取余,则为0或1
}//iniatal param

void BLDE::initialParam(int _population_number,int _bit_number){
   // vector<vector<bool>> present_population;//u individuals
   // vector<vector<bool>> last_population;//u individuals
    generation_number=0;
    population_number=_population_number;
    bit_number=_bit_number;
    //p=max(0.05,min(0.15,(double)10.0/bit_number));
    //p=0.01;
    if(_bit_number>300){
        p=0.005;
    }
    else if(_bit_number>100){ 
        p=0.01;
    }
    else{
        p=0.1;
    }
    srand(time(NULL)); //以time为生成随机数的种子,以便随机数不重复

    for(int i=0;i<population_number;i++){
        vector<int> tmp;
        vector<int> tmp_2;
        for(int j=0;j<bit_number;j++){
            tmp.push_back(1);
            tmp_2.push_back(1);
        }
        present_population.push_back(tmp);
        last_population.push_back(tmp_2);

    }
}
vector<int> BLDE::getBetterIndidual(const vector<int> &x, const vector<int> &y){
    return Objective(x)>Objective(y)?x:y;
}
void BLDE::step() {
    global_best_individual = present_population[0];
    vector<int> x;
    vector<int> y;
    vector<int> z;
    vector<int> tx;
    for (vector<int> &w:present_population) {
        x = present_population[0];
        y = present_population[0];
        z = last_population[0];
        tx = getBetterIndidual(y, z);
        for (int j = 0; j < bit_number; j++) {
            if (y[j] == z[j]) {
//                if(global_best_individual[j]!=x[j]){
//                    tx[j]=global_best_individual[j];
//                } else
                    {
                    if(rand()%(N+1)/(float)(N+1)<=p){
                       tx[j]=rand()%2;
                       // tx[j]=0;

                    }
                }
            }
        }
        if(Objective(tx)>=Objective(w)){
            w=tx;
        }
    }
    generation_number+=1;
    last_population=present_population;

}

vector<int> BLDE::getBestIndidual(){
    vector<int> res=present_population[0];
    int value=Objective(res);
    for(int i=1;i<present_population.size();i++){
    if(Objective(present_population[i])>value){
        res=present_population[i];
    }
    }
    return res;
}