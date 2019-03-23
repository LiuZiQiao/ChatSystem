#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include"Request.hpp"
#include"json/json.h"

class Util
{
private:
    /* data */
public:
    static void Serializ(Json::Value &root,std::string &string)
    {
        Json::FastWriter write;
        string = write.write(root);
    }
    
    static void UnSerializ(std::string &string,Json::Value &root)
    {
        Json::Reader read;
        read.parse(string,root,false);
    }

    static std::string IntToString(int x){
        std::stringstream ss;
        ss<<x;
        return ss.str();
    }

    static int StringToInt(std::string &str){
        std::stringstream ss(str);
        int s;
        ss>>s;
        return s;
    }


    static void RecvMessage(int sock,std::string &message,struct sockaddr_in &peer)
    {
        char msg[10240];
        socklen_t len = sizeof(peer);
        ssize_t s = recvfrom(sock,&msg,sizeof(msg),0,(struct sockaddr*)&peer,&len);
        // std::cout<<"RecvMessage:"<<msg<<std::endl;
        if (s<=0) {
            LOG("recvfrom message error",ERROR);
        }else{
            // std::cout<<"RecvMessage:"<<msg<<std::endl;
            message = msg;
        }      
    }

    static void SendMessage(int sock,const std::string &message,struct sockaddr_in &peer)
    {
        // LOG("Sending to ...",NORMAL);
        ssize_t s = sendto(sock,message.c_str(),message.size(),0,(struct sockaddr*)&peer,sizeof(peer));
        if (s <= 0) {
            LOG("sendto error",WARNING);
        }
        // std::cout<<"send over"<<std::endl;
        
    }


    static void addUser(std::vector<std::string> &online,const std::string &f)
    {
        for(auto it = online.begin();it != online.end();it++)
        {
            if(*it == f)
            {
                return ;
            }
        online.push_back(f);
        std::cout<<"addUser to Online "<<std::endl;
        }
    }
};