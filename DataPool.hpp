#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<semaphore.h>

class DataPool{
	private:
		std::vector<std::string> pool;
		int cap;
		sem_t data_sem;
		sem_t blank_sem;
		int product_step;
    	int consume_step;
	public:
		// DataPool(){}
		DataPool(int cap_=512):cap(cap_),pool(cap_)
		{
			sem_init(&data_sem,0,0);
			sem_init(&blank_sem,0,cap);			//cap  临界资源数目
			product_step = 0;
    		consume_step = 0;
		}
		void PutMessage(std::string &message){
			sem_wait(&blank_sem);
			pool[product_step++] = message;
			product_step %= cap;
			sem_post(&blank_sem);		
		}

		void GetMessage(std::string &message){
			sem_wait(&data_sem);
			pool[consume_step++] = message;
			consume_step %= cap;
			sem_post(&data_sem);
		}
		~DataPool()
		{
			sem_destroy(&data_sem);
			sem_destroy(&blank_sem);
		}
};

