#include "Socket.hpp"
#include<iostream>
#include"ChatClient.hpp"

void Usage(){
    std::cout<<"Usage: ip"<<std::endl;
}

static void menu(int &select)
{
    std::cout<<"********************"<<std::endl;
    std::cout<<"* 1.Register******"<<std::endl;
    std::cout<<"* 2.Login 3.exit *"<<std::endl;
    std::cout<<"*********************"<<std::endl;
    std::cin>>select;
}

int main(int argc,char *argv[])
{
    if (argc != 2) {
        Usage();
        exit(0);
    }
    
    std::string ip = argv[1];
    ChatClient *cp =new ChatClient(ip);
    cp->InitClient();
    int select = 0;
    while(1){
        menu(select);
        switch (select)
        {
            case 1:
                cp->Register();
                break;
            case 2:
                if(cp->Login()){
                    cp->Chat();
                }
            case 3:
                exit(0);
                break;
            default:
                break;
        }
    }
    
    // bool flag = cp->ConnectServer();


    // int port = atoi(argv[2]);
    // int sock = SocketApi::Socket(SOCK_STREAM);
    // bool flag = SocketApi::Connect(sock,ip,port);

    // if (flag) {
    //     std::cout<<"Connect success "<<std::endl;
    // }

    return 0;
}