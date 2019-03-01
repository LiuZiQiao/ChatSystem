#pragma ocne 
#include"Socket.hpp"
#include"json/json.h"
#include"Util.hpp"
#include"Message.hpp"

#define TCP_PORT 8080
#define UDP_PORT 8888

class ChatClient
{
private:
    std::string ip;
    int tcp_sock;
    int udp_sock;

    unsigned int id;
    std::string nick_name;
    std::string school;
    std::string passwd;
    struct sockaddr_in server;
public:
    ChatClient(std::string ip_):ip(ip_)
    {
        id=0;
        tcp_sock = -1;
        udp_sock = -1;
        server.sin_family=AF_INET;
        server.sin_addr.s_addr=inet_addr(ip.c_str());
        server.sin_port=htons(UDP_PORT);
    }

    void InitClient()
    {
        udp_sock = SocketApi::Socket(SOCK_DGRAM);
    }

    bool ConnectServer()
    {
        tcp_sock = SocketApi::Socket(SOCK_STREAM);
        return SocketApi::Connect(tcp_sock,ip,TCP_PORT);
    }

	bool RegisterEnter(std::string &nick_name,std::string &school,std::string &password)
	{
		std::cout<<"Please Input Nick Name:"<<std::endl;
		std::cin>> nick_name;
		std::cout<<"Please Input School:"<<std::endl;
		std::cin>>school;
		std::cout<<"Please Input password:"<<std::endl;
		std::cin>>password;
		std::string again;
		std::cout<<"Please Input password again:"<<std::endl;
		std::cin>>again;
		if(again == password)
		{
			return true;
		}
		return false;
	}

    bool Register(){
        if(RegisterEnter(nick_name,school,passwd) && ConnectServer()){
            
            Request rq;
            rq.method ="REGISTER\n";
            
            Json::Value root;
            root["name"] = nick_name;
            root["school"] = school;
            root["passwd"] = passwd;

            Util::Serializ(root,rq.text);

		    std::cout<<"RegisterEnter rq.text:"<<rq.text<<std::endl;

            rq.content_length = "Content_length:";
            rq.content_length +=  Util::IntToString((rq.text).size());
            rq.content_length += "\n";
            SocketApi::SendRequest(tcp_sock,rq);    //send request
            recv(tcp_sock,&id,sizeof(id),0);   // response; return  a register id;

            bool res =false;
            if(id >= 10000){
                std::cout<<"Register Success! Your Login ID is:"<<id<<std::endl;
                res = true;
            }else{
                std::cout<<"Register Faild!"<<std::endl; 
            }
            close(tcp_sock);
            return res;
        }
    }

    bool LoginEnter(unsigned int &id,std::string &password)
    {
        std::cout<<"Login ID:"<<std::endl;
		std::cin>>id;
		std::cout<<"Password:"<<std::endl;
        std::cin>>passwd;
        return true;
    }

    bool Login()
    {
        if(LoginEnter(id,passwd) && ConnectServer()){
            
            Request rq;
            rq.method ="LOGIN\n";
            
            Json::Value root;
            root["id"] = id;
            root["passwd"] = passwd;

            Util::Serializ(root,rq.text);

            rq.content_length = "Content_length:"+Util::IntToString((rq.text).size());
            rq.content_length += "\n";
            SocketApi::SendRequest(tcp_sock,rq);
            unsigned int resutl =  0;
            recv(tcp_sock,&resutl,sizeof(resutl),0);
             
            bool res = false;
            if(resutl >= 10000){
                std::cout<<"Login Success!"<<id<<std::endl;
                res = true;
            }else{
                std::cout<<"Login Faild! Code is"<<resutl<<std::endl; 
            }
            close(tcp_sock);
            return res;
        }
    }

    void Chat()
    {
        std::string name;
        std::string school;
        std::cout<<"Input you name:";
        std::cin>>name;
        std::cout<<"Input shool :";
        std::cin>>school;
        Message msg;
        while(1){
            std::string text;
            std::cout<<"Enter";
            std::cin>>text;
            Message msg(name,school,text,id);
            std::string sendString;
            msg.ToSendString(sendString);
            
            std::cout<<"debug ToSendString:"<<sendString<<std::endl;

            Util::SendMessage(udp_sock,sendString,server);
            
            std::string recvString;
            struct sockaddr_in peer;
            Util::RecvMessage(udp_sock,recvString,peer);
            std::cout<<"cliend send debug:"<<recvString<<std::endl;
            msg.ToRecvValue(recvString);
            std::cout<<"name"<<msg.Nick_Name()<<std::endl;
            std::cout<<"school"<<msg.School()<<std::endl;
            std::cout<<"Text"<<msg.Text()<<std::endl;
        }
        
    }
    ~ChatClient(){
        
    }
};
