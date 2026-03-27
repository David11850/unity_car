#include"./include/allhead.h"

/*-------------------unchangeable----------------*/
#define SEV_IP "192.168.1.8"
#define SEV_PORT 10000

/*--------------------changeable-----------------*/
#define CLI_IP "192.168.1.8"
//#define CLI_PORT 10001

int main(int argc,char*argv[]){
    //get port info
    if(argc!=2){
        perror("argc error");
        return -1;
    }
    uint16_t CLI_PORT=argv[1]-"0";

    //create client socket
    int cli_sfd=socket(AF_INET,SOCK_STREAM,0);
    if(cli_sfd==-1){
        perror("create client socket error");
        return -1;
    }
    printf("[INFO]create client socket %d\n",cli_sfd);

    //create sockaddr_in
    sockaddr_in cli_ad;
    cli_ad.sin_family=AF_INET;
    cli_ad.sin_addr.s_addr=inet_addr(CLI_IP);
    cli_ad.sin_port=htons(CLI_PORT);

    //bind with IP and port
    if(bind(cli_sfd,(sockaddr*)&cli_ad,sizeof(cli_ad))==-1){
        perror("bind ip and port to client socket error");
        return -1;
    }
    printf("[INFO]bind ip and port to client socket %d\n",cli_sfd);

    //create sever sockaddr_in
    sockaddr_in sev_ad;
    sev_ad.sin_family=AF_INET;
    sev_ad.sin_addr.s_addr=inet_addr(SEV_IP);
    sev_ad.sin_port=htons(SEV_PORT);

    //connect to the sever
    if(connect(cli_sfd,(sockaddr*)&sev_ad,sizeof(sev_ad))==-1){
        perror("connect to sever error");
        return -1;
    }
    printf("[INFO]client %d connect to sever epoll\n",cli_sfd);

    //send client verify
    char verify_words[]="LOGIN,CLIENT";
    if(send(cli_sfd,verify_words,sizeof(verify_words),0)==-1){
        perror("send verify words to sever error");
    }
    printf("[INFO]send verify words %s to sever\n",verify_words);

    while(1){
        char buf[256];
        memset(buf,0,sizeof(buf));
        printf("enter point x y:");
        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf)-1]='\0';
        if(strcmp(buf,"quit")==0){
            printf("[INFO]user finish input\n");
            break;
        }
        if(send(cli_sfd,buf,strlen(buf),0)==-1){
            perror("send error");
            continue;
        }
        printf("[INFO]send message %s to sever\n",buf);
    }

    close(cli_sfd);

    return 0;
}