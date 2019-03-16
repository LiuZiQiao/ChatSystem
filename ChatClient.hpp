#pragma ocne 
#include"Socket.hpp"
#include"json/json.h"
#include"Util.hpp"
#include"Message.hpp"
#include"Window.hpp"
#include<pthread.h>

#define TCP_PORT 8080
#define UDP_PORT 8888

class ChatClient;

struct Pairparam
{
    Window *wp;
    ChatClient *cp; 
};


class ChatClient
{
private:
    std::string ip;
    int tcp_sock;
    int udp_sock;
    struct sockaddr_in server;
    std::string passwd;
public:
    unsigned int id;
    std::string nick_name;
    std::string school;
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
		std::cout<<"Please Input Nick Name:";
		std::cin>> nick_name;
		std::cout<<"Please Input School:";
		std::cin>>school;
		std::cout<<"Please Input password:";
		std::cin>>password;
		std::string again;
		std::cout<<"Please Input password again:";
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
        std::cout<<"Login ID:";
		std::cin>>id;
		std::cout<<"Password:";
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
           
            // std::string resutl;
            unsigned int result;
            recv(tcp_sock,&result,sizeof(result),0);      //recv the login result,and result include the user info

            // Json::Value rs;
            // Util::UnSerializ(resutl,rs);
            // nick_name = rs["name"].asString();
            // school = rs["school"].asString();

            bool res = false;
            if(result >= 10000){
                res = true;
                std::string name_ = "None";
                std::string school_ = "None";
                std::string text_ = "I Login";
                unsigned int type_ = LOGIN_TYPE;
                unsigned int id_ = result;
                Message mseg(name_,school_,text_,id_,type_);
                std::string sendmsg;
                mseg.ToSendString(sendmsg);
                SendMsg(sendmsg);
                std::cout<<"Login Success!"<<id<<std::endl;
            }else{
                std::cout<<"Login Faild! Code is"<<result<<std::endl; 
            }
            close(tcp_sock);
            return res;
        }
    }


    static void *Welcome(void *arg)
    {
        pthread_detach(pthread_self());
        Window *wp = (Window*)arg;
        wp->Welcomde();
    }

    void SendMsg(std::string &sendString)
    {
        Util::SendMessage(udp_sock,sendString,server);
    }

    static void *Input(void *arg)
    {
        pthread_detach(pthread_self());
        struct Pairparam *pptr = (struct Pairparam*)arg;
        Window *wp = pptr->wp;
        ChatClient *cc = pptr->cp;

        wp->DrawInput();
        std::string text;
        for(;;)
        {
            wp->GetStringFromInput(text);
            Message msg(cc->nick_name,cc->school,text,cc->id);
            std::string sendString;
            msg.ToSendString(sendString);
            cc->SendMsg(sendString);
        }
        
    }

    void RecvMsg(std::string &recvString)
    {
        Util::RecvMessage(udp_sock,recvString,server);
    }
    void Chat()
    {
        Window w;
        pthread_t h,m;
        struct Pairparam pp ={&w,this};

        pthread_create(&h,NULL,Welcome,&w);
        pthread_create(&m,NULL,Input,&pp);      //聊天输入线程
        w.DrawOutput();
        w.DrawOnline();
        std::string recvString;
        std::string showString;
        std::vector<std::string> online;
        for(;;){
            Message message;
            RecvMsg(recvString);
            message.ToRecvValue(recvString);

            if (message.Id() == id && message.Type() == LOGIN_TYPE) {
                nick_name = message.Nick_Name();
                school = message.School();
            }
            showString = message.Nick_Name();
            showString +="-";
            showString += message.School();

            std::string f = showString;
            Util::addUser(online,f);

            showString +="-";
            showString +=message.Text();   
            w.PutStringToOutput(showString);    
            w.PutUserToOnline(online);     
        }
    }

    // void Chat()
    // {

    //         /*以下注释代码 用以上代码 替换*/
    //         // std::cout<<"Input you name:";
    //         // std::cin>>name;
    //         // std::cout<<"Input shool :";
    //         // std::cin>>school;

    //         Message msg;
    //         while(1){
    //             std::string text;
    //             std::cout<<"Enter>>";
    //             std::cin>>text;
    //             Message msg(nick_name,school,text,id);
    //             std::string sendString;
    //             msg.ToSendString(sendString);       //将发送的消息转化成要发送的字符串
                
    //             // std::cout<<"debug ToSendString:"<<sendString<<std::endl;

    //             Util::SendMessage(udp_sock,sendString,server);  //send to server

    //             std::string recvString;
    //             struct sockaddr_in peer;

    //             Util::RecvMessage(udp_sock,recvString,peer);      // recv message from server
    //             std::cout<<"clien RecvMsg Debug:"<<recvString<<std::endl;
    //             msg.ToRecvValue(recvString);    //将消息序列化到message对象中
    //             // std::cout<<"name:"<<msg.Nick_Name()<<std::endl;
    //             // std::cout<<"school:"<<msg.School()<<std::endl;
    //             // std::cout<<"Text:"<<msg.Text()<<std::endl;
    //         }
   
    // }

    ~ChatClient(){}
};
