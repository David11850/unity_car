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
    printf("[INFO]add client %d into map\n",cli_sfd);
}

//Function : add order to deque(orders)
void Manager::addOrder(Order_info order){
    if(order.priority>3) orders.push_front(order);
    else orders.push_back(order);
    printf("[INFO]add order %s into queue\n",order.food);
}

//Function : add unity car to unordered_map(unitys)
void Manager::addUnity(Unity_info unity){
    int uni_sfd=unity.uni_sfd;
    unitys[uni_sfd]=unity;
    printf("[INFO]add unity %d into map\n",uni_sfd);
}

//Function : be used when get existed sfd in epoll
//reminder : how to address client and unity are not finished
void Manager::addressSfd(int sfd){
    //sfd is client
    if(clients.find(sfd)!=clients.end()){
        //address client's order request

    }
    
    //sfd is unity car
    else if(unitys.find(sfd)!=unitys.end()){
        //accept unity's data and calculate the next spot
        
    }
    
    //new client or unity car need to be verify
    else{
        //recv message from sfd to verify identity(client/unity)
        char buf[128];
        memset(buf,0,sizeof(buf));
        int num=recv(sfd,buf,sizeof(buf),0);
        if(num<0){
            perror("recv for verify error");
            return;
        }
        else if(num==0){
            printf("[INFO]socket %d end connection\n",sfd);
            close(sfd);
            return;
        }
        printf("[INFO]recv from %d :%s\n",sfd,buf);
        
        //if client
        if(strcmp(buf,"LOGIN,CLIENT")==0){
            //use getpeername to get sockaddr in kernal
            sockaddr_in cli_ad;
            socklen_t len=sizeof(cli_ad);
            if(getpeername(sfd,(sockaddr*)&cli_ad,&len)==-1){
                perror("getpeername error");
                return;
            }
            printf("[INFO]get sockaddr of %d stored in kernal\n",sfd);

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
            printf("[INFO]get sockaddr of %d stored in kernal\n",sfd);

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
            printf("[INFO]unknown protocol to add to new client or unity\n");
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


void Manager::deleteOrder(Order_info&order){
    auto it = find(orders.begin(), orders.end(), order);                      
      if(it==orders.end()){
        printf("[ERROR]no such order %s in orders queue\n",order.food);
        return;
      }                                     
      orders.erase(it);
      printf("[INFO]delete order %s from queue\n",order.food);
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
    for(auto&cur:orders){
        printf("Order %s to x:%lf y:%lf with priority %d\n",cur.food,cur.dx,cur.dy,cur.priority);
    }
}

//Function : intreface to get Manager::clients
const unordered_map<int,Client_info>&Manager::giveClients(){
    return clients;
}

//Function : intreface to get Manager::orders
const deque<Order_info>&Manager::giveOrders(){
    return orders;
}

//Function : intreface to get Manager::unitys
const unordered_map<int,Unity_info>&Manager::giveUnitys(){
    return unitys;
}