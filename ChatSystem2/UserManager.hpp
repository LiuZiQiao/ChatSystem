#pragma once
#include<iostream>
#include<string>
#include<unordered_map>

class User
{
private:
    std::string nick_name;
    std::string school;
    std::string password;
public:
    User(){}
    User(std::string n_,std::string s_,std::string p_):
        nick_name(n_),
        school(s_),
        password(p_)
    {}
    bool IsPasswdOk(const std::string &p){
        return p == password ? true:false;
    }

    std::string getName()
    {
        return nick_name;
    }

    std::string getSchool()
    {
        return school;
    }
    ~User(){}
};

class UserManager
{
private:
    unsigned int assign_id;
    std::unordered_map<unsigned int,User> users;
    std::unordered_map<unsigned int,struct sockaddr_in> onlineusers;
    pthread_mutex_t lock;
    void Lock()
    {
        pthread_mutex_lock(&lock);
    }
    void UnLock(){
        pthread_mutex_unlock(&lock);
    }
public:
    UserManager(unsigned int x = 10000){
        assign_id = x;
        pthread_mutex_init(&lock,NULL);
    }


    unsigned int Check(unsigned int id,std::string passwd){
        Lock();
        // std::cout<<"Check id:"<<id<<std::endl;
        auto it = users.find(id);
        if(it != users.end()){
            // std::cout<<"passwd1:"<<passwd<<std::endl;
            User &u = it->second;
            if(u.IsPasswdOk(passwd))
            {
                // std::cout<<"passwd2:"<<passwd<<std::endl;
                UnLock();
                return id;
            }
        }
        UnLock();
        return 2;
    }

    unsigned int Insert(const std::string &name,const std::string &school,const std::string &passwd){
        Lock();
        unsigned int id = assign_id++;
        User u(name,school,passwd);
        if(users.find(id) == users.end()){
            users.insert({id,u});
            // for(auto &e:users){
            //     std::cout<<"id"<<e.first<<"user"<<e.second<<std::endl;
            // }
            
            UnLock();
            // std::cout<<"Insert success id assign_id:" << id<<"--"<<assign_id<<std::endl;

            return id;
        }
        std::cout<<"Insert error id:" << id<<std::endl;
        return -1;
    }

    void AddOnlineuser(unsigned int id,struct sockaddr_in &peer)    //上线
    {
        Lock();
        auto it = onlineusers.find(id);
        if(it == onlineusers.end()){
            onlineusers.insert({id,peer});
            std::cout<<"add online user success:"<<id<<std::endl;
        }
        UnLock();
    }

    std::unordered_map<unsigned int ,struct sockaddr_in> OnlineUser()
    {
        Lock();
        auto online = onlineusers;
        UnLock();
        return online;
    }
    ~UserManager(){
        pthread_mutex_destroy(&lock);
    }


    void GetInfo(const unsigned int id,std::string &name,std::string &school)
    {
        Lock();
        User u;
        auto us = users.find(id);
        if(us != users.end())
        {
            name = us->second.getName();
            school = us->second.getSchool();
            std::cout<<"Find user"<<std::endl;
        }
    }
};