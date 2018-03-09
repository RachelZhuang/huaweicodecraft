#include "deploy.h"
#include "Solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>
#include <cstring>

using namespace std;

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{

    int net_number=0, path_number=0, consume_number=0;//网络节点数，链路数，消费节点数
    int deploy_cost=0;//部署成本
    vector<vector<int>> link_cap_cost;//连接矩阵1：描述网络节点间连接关系，带宽，单位费用
    vector<vector<int>> source_target_consume;//连接矩阵2：描述网络节点与消费节点连接关系，需求

    //fuction

    //1.获取网络节点数、链路数、消费节点数
    char *net_path_consume = strtok(topo[0], " ");
    int i = 0;
    while(net_path_consume != NULL){
        if (i == 0)  net_number = atoi(net_path_consume);
        if (i == 1)  path_number = atoi(net_path_consume);
        if (i == 2)  consume_number = atoi(net_path_consume);
        i++;
        net_path_consume = strtok(NULL, " ");
    }


    //2.获取部署成本
    deploy_cost = atoi(topo[2]);

    //3.获取连接矩阵1
    for (i = 4;*topo[i]!='\r';i++) {                 //没有进行到换行符就进行循环
        char *Pointer_link_cap_cost = strtok(topo[i], " ");     //定义字符指针存储切割后的第一个字符串
       // printf(Pointer_link_cap_cost);
        //printf("\n");
        vector<int> link_cap_cost_cycle;						//定义一维整型向量存储每行数据
        for (int j = 0;Pointer_link_cap_cost!=NULL;j++) {
            //循环存储每行切割的字符串转成的数字
            link_cap_cost_cycle.push_back(atoi(Pointer_link_cap_cost));  //将每个切割的字符串转成数字并存储到一维整型向量
            Pointer_link_cap_cost = strtok(NULL, " ");                   //切割字符
            //printf("%d   ",link_cap_cost_cycle[j]);       //输出矩阵，每行4个
        }
        link_cap_cost.push_back(link_cap_cost_cycle);                   //将一维整型向量存储到二维整型向量
        //printf("\n");
    }

    //4.获取连接矩阵2
    for (;i<line_num;i++) {                        //到达最后一行结束
        if (*topo[i] == '\n') continue;
        char *Pointer_source_target_consume = strtok(topo[i], " ");
        vector<int> source_target_consume_cycle;
        for (int j = 0;Pointer_source_target_consume != NULL;j++) {
            //循环存储每行切割的字符串转成的数字
            source_target_consume_cycle.push_back(atoi(Pointer_source_target_consume));
            Pointer_source_target_consume = strtok(NULL, " ");
            //printf("%d   ", source_target_consume_cycle[j]);      //输出矩阵，每行3个
        }
        source_target_consume.push_back(source_target_consume_cycle);
        //printf("\n");
    }
    source_target_consume.erase(source_target_consume.begin());

    Solver so(net_number, path_number, consume_number, deploy_cost, link_cap_cost, source_target_consume);
    string res=so.run();
    const char* topo_file=res.c_str();
    write_result(topo_file, filename);
}
