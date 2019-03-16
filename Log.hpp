#pragma once
#include<iostream>
#include<string>

#define LOG(msg,level) Log(msg,level,__FILE__,__LINE__)
//typedef LOG(msg,level) Log(msg,level,__FILE__,__LINE__)
#define NORMAL 1
#define WARNING 2
#define ERROR 3

const char *log_level[]={
    NULL,
    "Normal",
    "Woring"
    "Error"
};

void Log(std::string msg,int level,const char* file,int line)
{
    std::cout<<'['<<file<<']'<<":"<<line<<":"<<msg<<"-"<<log_level[level]<<std::endl;
}
