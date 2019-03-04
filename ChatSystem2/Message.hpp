#pragma once
#include<string>
#include"json/json.h"
#include"Util.hpp"
#include<iostream>
class Message
{
private:
    std::string nick_name;
    std::string school;
    std::string text;
    unsigned int id;
public:
    Message(){}
    Message(const std::string &n_,const std::string s_,const std::string t_,const unsigned int id_):
        nick_name(n_),
        school(s_),
        text(t_),
        id(id_)
    {

    }

    void ToSendString(std::string &sendString){
        Json::Value root;
        root["name"] = nick_name;
        root["school"] = school;
        root["text"] = text;
        root["id"] = id;
        Util::Serializ(root,sendString);
    }
    void ToRecvValue(std::string &recvString)
    {
        Json::Value root;
        Util::UnSerializ(recvString,root);
        nick_name = root["name"].asString();
        school = root["school"].asString();
        text = root["text"].asString();
        id = root["id"].asInt();
    }

    const std::string &Nick_Name()
    {
        return nick_name;
    }
    const std::string &School(){
        return school;
    }
    const std::string &Text(){
        return text;
    }

    const unsigned int &Id(){
        return id;
    }
    ~Message(){}
};