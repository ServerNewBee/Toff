//
// Created by tywang on 17-5-1.
//

#ifndef TOFF_HTTP_SERVER_H
#define TOFF_HTTP_SERVER_H
// System
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
// C++
#include <atomic>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <functional>
// Lib
#include "http_parser/http_parser.h"
#include "http_types/http_request.h"
#include "http_types/http_response.h"

class SimpleHttpServer
{
public:
    SimpleHttpServer() : threadNum(1), isInterrupted(false) {}
    ~SimpleHttpServer() {}
    
    int Init(int portno, int threadNum, std::function<int(HttpRequest&, HttpResponse&, void*)> handler, void* this_ptr);

    int Start();

    int Join();

private:
    int start();
    int process(int sock);
    int sockfd;
    struct sockaddr_in serv_addr;
    int threadNum;
    std::atomic<bool> isInterrupted;
    std::function<int(HttpRequest&, HttpResponse&, void*)> handler;
    void* this_ptr;
};

#endif //TOFF_HTTP_SERVER_H
