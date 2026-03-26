#include"../include/allhead.h"

/*--------------------------------------------epoll_sever------------------------------------------*/
Sever::Sever(int unity_sfd):unity_sfd(unity_sfd){
    //init sever sockaddr_in
    sev.sin_family=AF_INET;
    sev.sin_addr.s_addr=inet_addr(SEVER_IP);
    sev.sin_port=htons(SEVER_PORT);

    //init client table
    client_table.resize(MAX_CLIENT,-1);

    //create epoll within MAX_CLIENT limit
    efd=epoll_create(MAX_CLIENT);

    //add unity socket file director into epoll
    unity_ev.events=EPOLLIN;
    unity_ev.data.fd=unity_sfd;
    if(epoll_ctl(efd,EPOLL_CTL_ADD,unity_sfd,&unity_ev)==-1){
        perror("epoll_ctl:add unity sfd into epoll error");
    }
    printf("[INFO]add unity sfd into epoll\n");
}

Sever::~Sever(){
    //close socket
    close(sfd);
    close(unity_sfd);
    close(efd);
}

//function : create socket and epoll in kernal , wait for targetd clients , use addressClient func 
//           and add unity_sfd into epoll , recv unity data
//attention : must be used after addClient , so we have client to listen in epoll(efd)
bool Sever::createSever(){
    //create socket
    sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1){
        perror("socket error");
        return false;
    }
    printf("[INFO]create sever socket\n");

    //bind socket to ip and port
    if(bind(sfd,(sockaddr*)&sev,sizeof(sev))==-1){
        perror("bind error");
        return false;
    }
    printf("[INFO]sever socket bind to IP and port\n");

    //listen on socket
    if(listen(sfd,MAX_CLIENT)==-1){
        perror("listen error");
        return false;
    }
    printf("[INFO]listening\n");

    //add sever socket into epoll
    epoll_event sever_ev;
    sever_ev.events=EPOLLIN;
    sever_ev.data.fd=sfd;
    if(epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&sever_ev)==-1){
        perror("epoll_stl:add sever sfd to epoll error");
    }
    printf("[INFO]add sever sfd %d into epoll\n",sfd);

    //add clients to epoll(epoll was created in init func)
    for(auto&cur:client_table){
        epoll_event ev;
        ev.events=EPOLLIN;
        ev.data.fd=cur;
        if(epoll_ctl(efd,EPOLL_CTL_ADD,cur,&ev)==-1){
            perror("epoll_ctl:add error");
        }
        printf("[INFO]add client %d to epoll\n",cur);
    }
    //epoll_wait
    while(!stopFlag){
        int num=epoll_wait(efd,event,MAX_CLIENT,-1);
        for(int i=0;i<num;++i){
            int acti_sfd=event[i].data.fd;
            //compare to sever , unity and client
            if(acti_sfd==sfd){
                //if sever , need to accept
                sockaddr_in cli;
                socklen_t len=sizeof(cli);
                int new_cli_sfd=accept(acti_sfd,(sockaddr*)&cli,&len);
                if(new_cli_sfd==-1){
                    perror("accept error");
                    close(new_cli_sfd);
                    continue;
                }
                //add client into epoll
                epoll_event client_ev;
                client_ev.events=EPOLLIN;
                client_ev.data.fd=new_cli_sfd;
                if(epoll_ctl(efd,EPOLL_CTL_ADD,new_cli_sfd,&client_ev)==-1){
                    close(new_cli_sfd);
                    perror("add new client into epoll error");  
                }
                printf("[INFO]add new client %s:%d into epoll\n",inet_ntoa(cli.sin_addr),ntohs(cli.sin_port));
            }
            else if(acti_sfd==unity_sfd) addressUnityData(acti_sfd);
            else addressClient(acti_sfd);
        }
    }

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



/*---------------------------------------unity----------------------------------*/
//Function : address unity data , and run algorithm to calculate next step
//reminder : algorithm stills undo
bool Sever::addressUnityData(int newsfd){
    //run algorithm to get next step
    char buf[512];
    memset(buf,0,sizeof(buf));
    //algorithm
    //algorithm
    //algorithm

    //send next step back to unity
    if(send(newsfd,buf,sizeof(buf),0)==-1){
        perror("send error");
        return false;
    }
    printf("[INFO]send next step to unity %d\n",unity_sfd);
}




/*---------------------------------------client-----------------------------------*/
//function : dynamicly add client(cli_sfd) to client table(cur_cli) and epoll(efd)
//attention : must be used before createSever!
bool Sever::addClient(int cli_sfd){
    //add client to client table
    if(cur_cli+1>MAX_CLIENT){
        printf("[ERROR]clients meet max num\n");
        return false;
    }
    for(int i=0;i<MAX_CLIENT;++i){
        if(client_table[i]==-1){
            client_table[i]=cli_sfd;
            break;
        }
    }
    ++cur_cli;
    printf("[INFO]add client %d to client table\n",cli_sfd);
    
    //add client to epoll event
    epoll_event ev;
    ev.events=EPOLLIN;
    ev.data.fd=cli_sfd;
    if(epoll_ctl(efd,EPOLL_CTL_ADD,cli_sfd,&ev)==-1){
        perror("epoll add client error");
        return false;
    }
    printf("[INFO]add client %d into epoll\n",cli_sfd);
    return true;
}

//function : define how to address client(cli_sfd) whose file director is now readable(EPOLLIN)
//reminder : how to fresh message from client to uable data for unity has not been written yet
bool Sever::addressClient(int cli_sfd){
    //receice client message
    char buf[MAX_MESSAGE_LENGTH];
    memset(buf,0,sizeof(buf));
    int num=recv(cli_sfd,buf,sizeof(buf),0);
    if(num==-1){
        perror("recv error");
        return false;
    }
    else if(num==0){
        deleteClient(cli_sfd);
        return true;
    }
    else
        printf("[INFO]receive message(%s) from client %d\n",buf,cli_sfd);


    //fresh message , and make it usable data in buf for unity
    //fresh message , and make it usable data in buf for unity


    //send usable data to unity(unity_sfd)
    if(send(unity_sfd,buf,sizeof(buf),0)==-1){
        perror("send error");
        return false;
    }
    printf("[INFO]send data(%s) to unity%d\n",buf,unity_sfd);

    return true;
}

//function : show infomation of every clients
//reminder : now the infomation of client is just socket file director
bool Sever::showClient(){
    for(auto&cur:client_table){
        printf("%d\n",cur);
    }
}

//function : delete client(cli_sfd) from clirnt table(cur_cli) and epoll(efd)
bool Sever::deleteClient(int cli_sfd){
    //delete client in client table
    auto place=find(client_table.begin(),client_table.end(),cli_sfd);
    if(place==client_table.end()) return true;
    *place=-1;
    --cur_cli;
    printf("[INFO]delete client %d in client table\n",cli_sfd);
   close(cli_sfd);
    
    //delete client in epoll
    if(epoll_ctl(efd,EPOLL_CTL_DEL,cli_sfd,NULL)==-1){
        perror("epoll_stl delete client error");
        return false;
    }
    printf("[INFO]epoll_ctl delete client\n");
 
    return true;
}




