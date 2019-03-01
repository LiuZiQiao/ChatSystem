#pragma once
#include<iostream>
#include<string>
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
        char msg[BUFSIZ];
        socklen_t len = sizeof(peer);
        ssize_t s = recvfrom(sock,&msg,sizeof(msg)-1,0,(struct sockaddr*)&peer,&len);
        if (s<=0) {
            LOG("recvfrom message error",ERROR);
        }else{
            message = msg;
        }      
    }

    static void SendMessage(int sock,std::string &message,struct sockaddr_in &peer)
    {
        ssize_t s = sendto(sock,message.c_str(),message.size(),0,(struct sockaddr*)&peer,sizeof(peer));
    }
};