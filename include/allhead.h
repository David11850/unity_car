#pragma once
#ifndef ALLHEADER_H
#define ALLHEADER_H

/*system lib*/
#include<bits/stdc++.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/epoll.h>
#include <netinet/tcp.h>
using namespace std;


/*my lib*/
#include"sever.h"
#include"manager.h"

/*IP and PORT*/
#define EPOLL_IP "192.168.0.109"
#define EPOLL_PORT 10000

/*open debug mode and more info will be printed*/
#define DEBUG



#endif