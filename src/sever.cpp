#include"../include/allhead.h"

/*--------------------------------------------epoll_sever------------------------------------------*/
Sever::Sever(){
    //init sever sockaddr_in
    sev.sin_family=AF_INET;
    sev.sin_addr.s_addr=inet_addr(EPOLL_IP);
    sev.sin_port=htons(EPOLL_PORT);

    //create epoll within MAX_CLIENT limit
    efd=epoll_create(MAX_CLIENT);
}

Sever::~Sever(){
    //close socket
    close(sfd);
    close(efd);
}

//Function : get single instance for use func in anywhere
//Delight point : single instance
Sever& Sever::getSever(){
    static Sever instance;
    return instance;
}

//function : create socket and epoll in kernal
//           use sfd to wait for newer(client/unity) 
//           and call function to address client and unity request
bool Sever::runSever(){
    //create socket
    sfd=socket(AF_INET,SOCK_STREAM,0);//TCP
    if(sfd==-1){
        perror("socket error");
        return false;
    }
    #ifdef DEBUG
    printf("[INFO]create sever socket %d !\n",sfd);
    #endif

    //setsockopt to be reusable
    int reuse=1;
    socklen_t len=sizeof(reuse);
    if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,len)==-1){
        perror("setsockaddr reuse error");
    }
    else{
        #ifdef DEBUG
        printf("[INFO]set socket option:address reusable\n");
        #endif
    }

    //bind socket to ip and port
    if(bind(sfd,(sockaddr*)&sev,sizeof(sev))==-1){
        perror("bind error");
        return false;
    }
    #ifdef DEBUG
    printf("[INFO]sever socket bind to IP and port\n");
    #endif

    //listen on socket
    if(listen(sfd,MAX_CLIENT)==-1){
        perror("listen error");
        return false;
    }
    #ifdef DEBUG
    printf("[INFO]listening\n");
    #endif

    //add sever socket into epoll(epoll was created in init func)
    sev_ev.events=EPOLLIN;
    sev_ev.data.fd=sfd;
    if(epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&sev_ev)==-1){
        perror("epoll_stl:add sever sfd to epoll error");
    }
    #ifdef DEBUF
    printf("[INFO]add sever sfd %d into epoll\n-------------\n",sfd);
    #endif

    //epoll_wait
    while(!stopFlag){
        //wait for sever , unity and client sfd
        int num=epoll_wait(efd,event,EPOLL_MAX,-1);
        for(int i=0;i<num;++i){
            int acti_sfd=event[i].data.fd;
            //new client or unity car
            if(acti_sfd==sfd){
                addNewConnectionToEpoll(acti_sfd);
            }
            //existed client or unity
            else{
                Manager&manager=Manager::getManager();
                manager.addressSfd(acti_sfd);
            }
        }
    }

    printf("[INFO]sever end running\n");

    return true;
}

//function : stop epoll_wait by change the bool
void Sever::stopSever(){
    stopFlag=1;
}

//function : restart epoll_wait by change the bool
void Sever::restartSever(){
    stopFlag=0;
}

//Function : create new sfd for newer , and add to epoll for next verify send
void Sever::addNewConnectionToEpoll(int cli_sfd){
    //accept new client , and create new sfd for new client
    int new_sfd=accept(sfd,NULL,NULL);
    if(new_sfd==-1){
        perror("accept error");
        return;
    }
    #ifdef DEBUG
    printf("[INFO]accept new_sfd %d\n",new_sfd);
    #endif

    //ban Nagle algorithm , which wait for 40 bits data then send and will delay the reception of unity car
    int enable = 1;
    setsockopt(new_sfd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));

    //create epoll_event for newer
    epoll_event new_ev;
    new_ev.events=EPOLLIN;
    new_ev.data.fd=new_sfd;

    //add to epoll
    if(epoll_ctl(efd,EPOLL_CTL_ADD,new_sfd,&new_ev)==-1){
        perror("epoll add error");
        return;
    }
    #ifdef DEBUG
    printf("[INFO]add newer %d into epoll\n",new_sfd);
    #endif
}


/*------------------------additional------------------------*/
void Sever::deleteClient(int cli_sfd){
    //delete cli_sfd from Manager:clients
    auto&manager=Manager::getManager();
    manager.deleteClientFromManager(cli_sfd);

    //delete cli_sfd from epoll
    if(epoll_ctl(efd,EPOLL_CTL_DEL,cli_sfd,NULL)==-1){
        perror("delete cli_sfd from epoll error");
        return;
    }
    printf("[INFO]delete cli_sfd %d from epoll\n",cli_sfd);
    close(cli_sfd);
}

void Sever::deleteUnity(int uni_sfd){
    //delete uni_sfd from Manager::unitys(map)
    auto&manager=Manager::getManager();
    manager.deleteUnityFromManager(uni_sfd);

    //delete uni_sfd from epoll
    if(epoll_ctl(efd,EPOLL_CTL_DEL,uni_sfd,NULL)==-1){
        perror("delete uni_sfd from epoll error");
        return;
    }
    printf("[INFO]delete uni_sfd %d from epoll\n",uni_sfd);
    close(uni_sfd);
}

