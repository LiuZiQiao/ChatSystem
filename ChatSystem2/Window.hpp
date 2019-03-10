#pragma once

#include<iostream>
#include<ncurses.h>
#include<unistd.h>
#include<string>
#include<vector>
#include<string.h>
#include<pthread.h>

class Window{
	public:
		WINDOW *header;
		WINDOW *output;
		WINDOW *online;
		WINDOW *input;
		pthread_mutex_t lock;
	public:
		Window(){
			initscr();
			curs_set(0);
			pthread_mutex_init(&lock,NULL);
		}

		void SafeWrefresh(WINDOW *w)
		{
			pthread_mutex_lock(&lock);
			wrefresh(w);
			pthread_mutex_unlock(&lock);
		}

		void DrawHeader()
		{
			int h = LINES/5;
			int w = COLS;
			int y = 0;
			int x = 0;
		
			header = newwin(h,w,y,x);
			box(header,0,0);
			SafeWrefresh(header);
		}
		
		void DrawOnline()
		{
			int h = LINES*0.6 ;
			int w = COLS*0.25;
			int y = LINES*0.2;
			int x = COLS*0.75;
		
			online = newwin(h,w,y,x);
			box(online,0,0);
			SafeWrefresh(online);
		}

		void DrawOutput()
		{
			int h = LINES*0.6;
			int w = COLS*0.75;
			int y = LINES*0.2;
			int x = 0;
		
			output = newwin(h,w,y,x);
			box(output,0,0);
			SafeWrefresh(output);
		}

		void DrawInput()
		{
			int h = LINES*0.2;
			int w = COLS;
			int y = LINES*0.8;
			int x = 0;
		
			input = newwin(h,w,y,x);
			box(input,0,0);
			std::string tip = "Enter#";
			PutStringToWin(input,2,2,tip);
			SafeWrefresh(input);
		}

		void PutStringToWin(WINDOW *w,int y,int x,std::string &message)
		{
			mvwaddstr(w,y,x,message.c_str());
			SafeWrefresh(w);
		}	

		void GetStringFromInput(std::string &message)
		{
			char buf[1024];
			memset(buf,0,sizeof(buf));
			wgetnstr(input,buf,sizeof(buf));
			message = buf;
			delwin(input);
			DrawInput();
		}

		void PutStringToOutput(std::string &message)
		{
			static int line = 1;
			int x,y;
			getmaxyx(output,y,x);
			if (line > y-2) {
				delwin(output);
				DrawOutput();
				line = 1;
			}
			PutStringToWin(output,line++,2,message);
		}
		
		void PutUserToOnline(std::vector<std::string> &onlineuser)
		{
			int size = onlineuser.size();
			for(auto i=0; i < size; i++)
			{
				PutStringToWin(online,i+2,2,onlineuser[i]);
				SafeWrefresh(online);
			}
			
		}

		void Welcomde()
		{
			std::string welcome = "Welcome to Team ChatSystem";
			int num = 1;
			int x,y;
			int dir = 0; //left->0  
			for(;;)
			{
				DrawHeader();
				getmaxyx(header,y,x);
				PutStringToWin(header,y/2,num,welcome);
				if (dir == 0) {
					num++;
				}else
				{
					num--;
				}
				if (num > x-welcome.size()-3) {
					dir = 1;
				} 
				if(num <=1){
					dir = 0;
				}
				usleep(100000);
				delwin(header);
			}
		}
		~Window()
		{
			pthread_mutex_destroy(&lock);
			endwin();
		}
		
	
};
