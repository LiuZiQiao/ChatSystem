#pragma once
#include "Socket.hpp"
#include "Request.hpp"
#include "Util.hpp"
#include"Message.hpp"
#include "UserManager.hpp"
#include"DataPool.hpp"
#include"Log.hpp"

class ChatServer;

class Pragma
{
public:
    int sock;
    ChatServer *sp;

    std::string ip;
    int port;
    Pragma(int sock_,ChatServer *sp_, std::string ip_,int p_):
        sock(sock_),
        sp(sp_),
        ip(ip_),
        port(p_)
    {}
};

class ChatServer
{
private:
    int tcp_listen_sock;
    int tcp_port;
    int udp_work_sock;
    int udp_port;

    UserManager um;
    DataPool pool;
public:
    ChatServer(int tcp_port_=8080,int udp_port_=8888):
        tcp_port(tcp_port_),
        udp_port(udp_port_),
        tcp_listen_sock(-1),
        udp_work_sock(-1)
    {

    }

    /*create socket And bind socket And tcp_listen_sock listen*/
    void InitServer()
    {
        //create
        tcp_listen_sock = SocketApi::Socket(SOCK_STREAM);
        udp_work_sock = SocketApi::Socket(SOCK_DGRAM);
        //bind
        SocketApi::Bind(tcp_listen_sock,tcp_port);
        SocketApi::Bind(udp_work_sock,udp_port);
        //listen 
        SocketApi::Listen(tcp_listen_sock);
    }

    unsigned int  RegisterUser(std::string &name,std::string &school,std::string &passwd)
    {
        return um.Insert(name,school,passwd);
    }

    unsigned int LoginUser(unsigned int id,std::string &passwd,const std::string &ip,const unsigned int &port){
        return um.Check(id,passwd);
    }

    void Product()
    {
        std::string message;
        struct sockaddr_in peer;
        Util::RecvMessage(udp_work_sock,message,peer);
        std::cout<<"Product debug"<<message<<std::endl;
        if (!message.empty()) {
            pool.PutMessage(message);
            Message m;
            m.ToRecvValue(message);        //   反序列化 获取id

            um.AddOnlineuser(m.Id(),peer);  //拿着id找sock
            
        }
    }

    void Consume(){
        std::string message;

        pool.GetMessage(message);
        std::cout<<"Consume debug"<<message<<std::endl;
        auto online = um.OnlineUser();      //遍历在线用户，发送消息
        std::cout<<"Online Users:"<<online.size()<<std::endl;
        
        for(auto it = online.begin(); it !=online.end(); it++)
        {
            Util::SendMessage(udp_work_sock,message,it->second);
        }
    }

    static void *HandlerRequest(void *arg)
    {
        Pragma *p = (Pragma*)arg;
        int sock = p->sock;
        ChatServer *cs = p->sp;
        std::string ip = p->ip;
        unsigned int port = p->port;
        delete p;
        pthread_detach(pthread_self());
        
        Request rq;
        SocketApi::RecvRequest(sock,rq);

        Json::Value root;
        LOG(rq.text,NORMAL);
        Util::UnSerializ(rq.text,root);

        std::cout<<"UnSerializ rq.text:"<<rq.text<<std::endl;

        if ("REGISTER" == rq.method) {
            std::string name = root["name"].asString();
            std::string school = root["school"].asString();
            std::string passwd = root["passwd"].asString();

            unsigned int id = cs->RegisterUser(name,school,passwd);
            
            std::cout<<"Register success returnd id:"<<id<<std::endl;       
            
            send(sock,&id,sizeof(id),0);        //return the  register id to client 
        }else if("LOGIN" == rq.method){
            unsigned int id = root["id"].asInt();
            std::string passwd = root["passwd"].asString();
            
            // std::cout<<"LoginUser id:"<<id<<std::endl;

            unsigned int result = cs->LoginUser(id,passwd,ip,port);     //    result == id;

            // 登陆成功之后，将这个登陆的用户信息打包发给client
            // UserManager u;
            // std::string name;
            // std::string school;
            // u.GetInfo(result,name,school);
            // Json::Value root;
            // root["id"] = result;
            // root["name"] = name;
            // root["school"] = school;
            // std::string Info;
            // Util::Serializ(root,Info);
            // std::cout<<"Login Info:"<<Info<<std::endl;
      
            // if(result >= 10000){
            //     // um.AddOnlineuser(result,sock);
            //     struct sockaddr_in peer;
            //     peer.sin_family = AF_INET;
            //     peer.sin_addr.s_addr = inet_addr(ip.c_str());
            //     peer.sin_port = htonl(port);
            //     UserManager u;
            //     u.AddOnlineuser(result,peer);
            // }

            send(sock,&result,sizeof(result),0);    //return the result of login to client
        }else {
        }
        close(sock);
    }
    void StartServer()
    {
        for(;;)
        {
            struct sockaddr_in cli_addr;
            std::string out_ip;
            int out_port;
            int sock = SocketApi::Accept(tcp_listen_sock,out_ip,out_port);
            if (sock > 0) {
                std::cout<<"Accept a new client"<<"ip:"<<out_ip<<"port:"<<out_port<<std::endl;
                
                Pragma *p = new Pragma(sock,this,out_ip,out_port);
                pthread_t tid;
                pthread_create(&tid,NULL,HandlerRequest,p);
            }
        }
    }
    ~ChatServer();
};