#include"../include/allhead.h"

Manager::Manager(){}
Manager::~Manager(){}

Manager& Manager::getManager(){
    static Manager instance;
    return instance;
}

//Function : add client to unordered_map(clients)
void Manager::addClient(Client_info client){
    int cli_sfd=client.cli_sfd;
    clients[cli_sfd]=client;
    printf("[INFO]add client %d into map\n------------\n",cli_sfd);
}

//Function : add order to deque(orders)
void Manager::addOrder(Order_info order){
    orders[cnt]=order;
    ++cnt;
    printf("[INFO]add order into manager\n-------------\n");
}

//Function : add order to task deque
void Manager::addTask(Order_info order){
    if(order.priority==0)tasks.push_back(order);
    else tasks.push_front(order);
    printf("[INFO]add order %s into queue\n------------\n",order.food.c_str());
}

//Function : add unity car to unordered_map(unitys)
void Manager::addUnity(Unity_info unity){
    int uni_sfd=unity.uni_sfd;
    unitys[uni_sfd]=unity;
    printf("[INFO]add unity car %d into map\n------------\n",uni_sfd);
}

//Function : be used when get existed sfd in epoll
//reminder : how to address client and unity are not finished
void Manager::addressSfd(int sfd){
    //sfd is client , just simulation
    if(clients.find(sfd)!=clients.end()){
        //address client's order request
        #ifdef DEBUG
        printf("[DEBUG]client %d sfd activate\n",sfd);
        #endif
        char buf[MAX_MESSAGE_LENGTH];
        memset(buf,0,sizeof(buf));
        int num=recv(sfd,buf,sizeof(buf)-1,0);
        if(num==-1){
            perror("recv client message error");
            return;
        }
        else if(num==0){
            printf("[INFO]client %d end connection\n",sfd);
            Sever&sever=Sever::getSever();
            sever.deleteClient(sfd);
            return;
        }
        printf("[INFO]recv message %s from client %d\n",buf,sfd);
        /*
        * main logic to address sever's orders , and send tasks to ready cars
        */

       //protocal: ORDER,(priority),(dx),(dy),(food name)
        string str(buf);
        str=str.substr(0,5);
        auto&manager=Manager::getManager();
        Order_info new_order;
        if(str=="ORDER"){
            new_order.priority=buf[7];
            new_order.dx=buf[9];
            new_order.dy=buf[11];
            str=buf;
            str=str.substr(12,strlen(buf)-12);
            new_order.food=str;
            manager.addOrder(new_order);
        }

        //add to task deque
        manager.addTask(new_order);
        printf("[INFO]add task into deque\n");
        
        //send to unity car
        
    }
    
    //sfd is unity car
    else if(unitys.find(sfd)!=unitys.end()){
        char buf[]="receive message from unity car\n";
        write(1,buf,strlen(buf));
        /*
        * accept unity's constant status reports
        */
    }
    
    //new client or unity car need to be verify
    else{
        //recv message from sfd to verify identity(client/unity)
        char buf[MAX_MESSAGE_LENGTH];
        memset(buf,0,sizeof(buf));
        int num=recv(sfd,buf,sizeof(buf)-1,0);
        if(num<0){
            perror("recv for verify error");
            return;
        }
        else if(num==0){
            printf("[INFO]socket %d end connection\n",sfd);
            close(sfd);
            return;
        }
        printf("[INFO]New connection received from %d : %s\n",sfd,buf);
        
        //if client
        if(strcmp(buf,"LOGIN,CLIENT")==0){
            //use getpeername to get sockaddr in kernal
            sockaddr_in cli_ad;
            socklen_t len=sizeof(cli_ad);
            if(getpeername(sfd,(sockaddr*)&cli_ad,&len)==-1){
                perror("getpeername error");
                return;
            }
            printf("[INFO]New client %d !\n",sfd);

            //make epoll_event
            epoll_event cli_ev;
            cli_ev.events=EPOLLIN;
            cli_ev.data.fd=sfd;
            
            //add to Manager::clients(map)
            Client_info info;
            info.cli_ad=cli_ad;
            info.cli_ev=cli_ev;
            info.cli_sfd=sfd;
            addClient(info);
        }
        
        //if unity
        else if(strcmp(buf,"LOGIN,UNITY")==0){
            //use getpeername to get sockaddr from kernal
            sockaddr_in uni_ad;
            socklen_t len=sizeof(uni_ad);
            if(getpeername(sfd,(sockaddr*)&uni_ad,&len)==-1){
                perror("getpeername error");
                return;
            }
            printf("[INFO]New unity car %d !\n",sfd);

            //make epoll_event
            epoll_event uni_ev;
            uni_ev.events=EPOLLIN;
            uni_ev.data.fd=sfd;
            
            //add to Manager::unitys(map)
            Unity_info info;
            info.uni_ad=uni_ad;
            info.uni_ev=uni_ev;
            info.uni_sfd=sfd;
            addUnity(info);
        }

        //unknown protocol
        else{
            printf("[INFO]unknown protocol to add to manager\n------------\n");
            close(sfd);
            return;
        }
    }
}

//Function : delete existed client from Manager::clients
void Manager::deleteClientFromManager(int cli_sfd){
    //delete cli_sfd from Manager
    auto it=clients.find(cli_sfd);
    if(it==clients.end()){
        printf("[ERROR]no such client %d in Manager::clients\n",cli_sfd);
        return;
    }
    clients.erase(it);
    printf("[INFO]remove client %d from Manager::clients\n",cli_sfd);
}

//Function : delete existed unity car from Manager::unitys
void Manager::deleteUnityFromManager(int uni_sfd){
    //delete cli_sfd
    auto it=unitys.find(uni_sfd);
    if(it==unitys.end()){
        printf("[ERROR]no such unity %d in Manager::unitys\n",uni_sfd);
        return;
    }
    unitys.erase(it);
    printf("[INFO]remove unity %d from Manager::unitys\n",uni_sfd);
}


void Manager::deleteOrderFromManager(Order_info&order){
    auto it = std::find_if(orders.begin(), orders.end(),
        [&order](const auto& p) { return p.second == order; });
      if(it==orders.end()){
        printf("[ERROR]no such order %s in orders queue\n",order.food.c_str());
        return;
      }                                     
      orders.erase(it);
      --cnt;
      printf("[INFO]delete order %s from queue\n",order.food.c_str());
}

//Function : show detailed info of every existed client
void Manager::showClient(){
    for(auto&[cli_sfd,info]:clients){
        printf("Client %s:%d has order num %d\n",inet_ntoa(info.cli_ad.sin_addr),
                ntohs(info.cli_ad.sin_port),info.order_num);
    }
}

//Function : show detailed info of every existed unity car
void Manager::showUnity(){
    for(auto&[uni_sfd,info]:unitys){
        printf("Unity car %s:%d\n",inet_ntoa(info.uni_ad.sin_addr),ntohs(info.uni_ad.sin_port));
    }
}

//Function : show detailed info of every existed order
void Manager::showOrder(){
    for(auto&[index,cur]:orders){
        printf("Order %d:%s to x:%lf y:%lf with priority %d\n",index,cur.food.c_str(),cur.dx,cur.dy,cur.priority);
    }
}

//Function : intreface to get Manager::clients
const unordered_map<int,Client_info>&Manager::giveClients(){
    return clients;
}

//Function : intreface to get Manager::orders
const unordered_map<int,Order_info>&Manager::giveOrders(){
    return orders;
}

//Function : intreface to get Manager::unitys
const unordered_map<int,Unity_info>&Manager::giveUnitys(){
    return unitys;
}