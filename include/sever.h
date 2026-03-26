#pragma once 
#ifndef SEVER_H
#define SEVER_H

#include"./allhead.h"

//for epoll
#define SEVER_IP "192.168.0.109"
#define SEVER_PORT 8080
#define MAX_CLIENT 1024
#define MAX_MESSAGE_LENGTH 256

/*brief : backend sever run in epoll
contact with : 
    clients : add clients , accept clients' message , show client table
    unity : address clients' message to data , send data to unity
*/ 
class Sever{
private:
    //sever variables
    int sfd,efd;
    sockaddr_in sev;
    bool stopFlag=0;
    epoll_event event[MAX_CLIENT];

    //client variables
    int cur_cli=0;
    vector<int> client_table;

    //unity variables
    int unity_sfd;
    epoll_event unity_ev;
public:
    Sever(int unity_sfd);
    ~Sever();

    //client func

    bool addClient(int cli_sfd);
    bool addressClient(int cli_sfd);
    bool showClient();
    bool deleteClient(int cli_sfd);

    //backend func

    bool createSever();
    void stopSever();
    void restartSever();

    //unity func
    
    bool addressUnityData(int newsfd);
};







#endif