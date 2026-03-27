//manager.h
//be used to manage client , unity and order

#pragma once
#ifndef MANAGER_H
#define MANAGER_H
#include"allhead.h"


//order
typedef struct Order_info{
    double dx=0,dy=0;
    string food;
    int priority=0;
    bool operator==(const struct Order_info&other)const{
        return (dx==other.dx && dy==other.dy && food==other.food
        && priority==other.priority);
    }
}Order_info;

//client
typedef struct{
    int cli_sfd=-1,order_num=0;
    vector<Order_info>orders; //maybe one client multi orders
    sockaddr_in cli_ad;
    epoll_event cli_ev;
}Client_info;

//unity
typedef struct{
    int uni_sfd=-1;
    sockaddr_in uni_ad;
    epoll_event uni_ev;
}Unity_info;



class Manager{
private:
    unordered_map<int,Client_info>clients;
    deque<Order_info>orders;
    unordered_map<int,Unity_info>unitys;
    
    Manager(); //invoid others use init to create another sever instance
public:
    //manage
    ~Manager();

    //instance
    static Manager& getManager();
    Manager(const Manager&)=delete;
    Manager operator=(const Manager)=delete;

    void addressSfd(int sfd);
    void addClient(Client_info client);
    void addOrder(Order_info order);
    void addUnity(Unity_info unity);
    

    //interface
    const unordered_map<int,Client_info>&giveClients();
    const deque<Order_info>&giveOrders();
    const unordered_map<int,Unity_info>&giveUnitys(); 

    //additional
    void showClient();
    void deleteClientFromManager(int cli_sfd);
    void showOrder();
    void deleteOrder(Order_info&order);
    void showUnity();
    void deleteUnityFromManager(int uni_sfd);
};

#endif