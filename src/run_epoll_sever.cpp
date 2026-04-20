#include"../include/allhead.h"

int main(int argc,char*argv[]){
    //create single instance epoll sever
    Sever&sever=Sever::getSever();

    //create single instance Manager
    Manager&manager=Manager::getManager();

    //run epoll
    sever.runSever();

    return 0;
}