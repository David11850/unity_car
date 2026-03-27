//backend sever run in epoll , and wait for sfd to activate

#pragma once 
#ifndef SEVER_H
#define SEVER_H

#include"./allhead.h"

//for epoll
#define EPOLL_IP "192.168.1.8"
#define EPOLL_PORT 10000
#define EPOLL_MAX 512
#define MAX_CLIENT 512
#define MAX_UNITY 512
#define MAX_MESSAGE_LENGTH 256

class Sever{
public:
    static Sever& getSever();
    ~Sever();
    bool runSever();
    void stopSever();
    void restartSever();

    void addNewConnectionToEpoll(int cli_sfd);
    void deleteClient(int cli_sfd);
    void deleteUnity(int uni_sfd);

private:
    Sever(); //signal instance , avoid recreate another sever in program 
    int sfd=-1,efd=-1; //sever sfd and epoll sfd
    bool stopFlag=0; //use to control epoll_wait
    sockaddr_in sev; //use to create socket
    epoll_event sev_ev; //use to add sever sfd into epoll
    epoll_event event[MAX_CLIENT]; //use to contain activate event in epoll_wait

};
#endif