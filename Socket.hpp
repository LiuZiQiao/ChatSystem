#pragma once
#include<iostream>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<string>
#include<sys/socket.h>
#include "Log.hpp"
#include"Request.hpp"
#include"Util.hpp"

#define BACKLOG 5

class SocketApi
{
public:
	static int Socket(int type)
	{
		int fd = socket(AF_INET,type,0);
		if (fd < 0) {
			LOG("socket error",ERROR);
			exit(2);
		}
		return fd;
	}

	static int Bind(int sock,int port)
	{
		struct sockaddr_in local;
		local.sin_family = AF_INET;
		local.sin_addr.s_addr = htonl(INADDR_ANY);
		local.sin_port = htons(port);

		int n = bind(sock,(struct sockaddr*)&local ,sizeof(local));
		if (n < 0) {
			LOG("bind error",ERROR);
			exit(3);
		}
	}

	static int Listen(int sock)
	{
		if(listen(sock,BACKLOG) < 0)
		{
			LOG("listen error",ERROR);
			exit(4);
		}
	}

	static bool Connect(const int &sock,std::string peer_ip,const int &port)
	{
		struct sockaddr_in cli_addr;
		cli_addr.sin_family =AF_INET;
		cli_addr.sin_addr.s_addr = inet_addr(peer_ip.c_str());
		cli_addr.sin_port = htons(port);
		if(connect(sock,(struct sockaddr*)&cli_addr,sizeof(cli_addr))<0){
			perror("connect error:");
			LOG("connect error",WARNING);
			return false;
		}
		return true;
	}

	static int Accept(int sock,std::string &out_ip,int &out_port)
	{
		struct sockaddr_in peer;

		socklen_t len = sizeof(peer);
		int fd = accept(sock,(struct sockaddr*)&peer ,&len);  //返回一个socket套结字
		if (fd < 0) {
			LOG("listen error",WARNING);
			return -1;
		} 
		out_ip = inet_ntoa(peer.sin_addr);
		out_port = htons(peer.sin_port);		
		return fd;
	}

	static void SendRequest(int sock,Request &rq)
    {
		std::string &m_ = rq.method;
		std::string &cl_ = rq.content_length;
		std::string &b_ = rq.blank;
		std::string &t_ = rq.text;
		std::cout<<"SendRequest rq.text:"<<rq.text<<std::endl;

		send(sock,m_.c_str(),m_.size(),0);
		send(sock,cl_.c_str(),cl_.size(),0);
		send(sock,b_.c_str(),b_.size(),0);
		send(sock,t_.c_str(),t_.size(),0);
	}

	static void RecvOneLine(int sock,std::string &outString){
		char c='x';
		while(c != '\n'){
			ssize_t s = recv(sock,&c,1,0);
			if (s>0) {
				if(c == '\n'){
					break;
				}
				outString.push_back(c);
			}else{
				break;
			}
		}
	}

	static void RecvRequest(int sock,Request &rq)
	{
		RecvOneLine(sock,rq.method);
		RecvOneLine(sock,rq.content_length);
		RecvOneLine(sock,rq.blank);

		std::cout<<"RecvRequest rq.content_length:"<<rq.content_length<<std::endl;
		
		std::string &cl = rq.content_length;
		std::size_t pos = cl.find(":");
		if(std::string::npos == pos)
		{
			return;
		}
		std::string sub = cl.substr(pos+1);
		int size = Util::StringToInt(sub);
		char c;
		for(auto i=0;i<size;i++){
			recv(sock,&c,1,0);
			(rq.text).push_back(c);
		}
		std::cout<<"RecvRequest rq.text:"<<rq.text<<"len:"<<rq.text.size()<<std::endl;


	}
};