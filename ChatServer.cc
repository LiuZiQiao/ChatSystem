#include<iostream>
#include "Socket.hpp"
#include"ChatServer.hpp"

void Usage(){
    std::cout<<"Usage: ip port"<<std::endl;
    exit(1);
}

void *RunProduct(void *arg)
{
    pthread_detach(pthread_self());
    for(;;){
        ChatServer *p = (ChatServer*)arg ;
        p->Product();
    }
}

void *RunConsume(void* arg){
    pthread_detach(pthread_self());
    while(1){
        ChatServer *p = (ChatServer*)arg ;  
        p->Consume(); 
    }
}

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        Usage();
    }
    int tcp_port = atoi(argv[1]);
    int udp_port = atoi(argv[2]);
    ChatServer *sp = new ChatServer(tcp_port,udp_port);
    sp->InitServer();
    pthread_t c,p;
    pthread_create(&p,NULL,RunProduct,sp);
    pthread_create(&c,NULL,RunConsume,sp);

    sp->StartServer();

    // int port = atoi(argv[1]);
    // int sock = SocketApi::Socket(SOCK_STREAM);
    // SocketApi::Bind(sock,port);
    // SocketApi::Listen(sock);
    // std::string out_ip;
    // int out_port;
    // SocketApi::Accept(sock,out_ip,out_port);
    // std::cout<<"out_ip："<<out_ip<<"port："<<port<<std::endl;
    return 0;
}