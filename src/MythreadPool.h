#ifndef __MYTHREADPOOL_H__
#define __MYTHREADPOOL_H__
#include <iostream>
#include "Task.h"
#include <queue> 
#include <cstdlib>
#include <unistd.h>
#include <pthread.h> 
class MythreadPool{
public:
	MythreadPool()=delete;
	MythreadPool(int min , int max, int capacity);
	~MythreadPool();
	int minNum; // 线程池的最小值
	int maxNum; // 线程池允许的最大值	
	int capacityNum; // 消息队列的最大容积上限 // 防止生产者无限制生产
	int busyNum; // 处于繁忙状态的函数
	int liveNum; // 存活的线程数量
	int exitNum; // 要销毁的线程数
	bool shutDown; // 线程池是否被关闭
	int utime;
	std::queue<Task> TaskQ; // 任务队列
	pthread_mutex_t mutexPool; // 整个线程池的锁
	pthread_mutex_t mutexBusy; // 处于繁忙中的线程数的锁
	pthread_mutex_t mutexTimer;
	pthread_cond_t notFull; 
	pthread_cond_t notEmpty;
	
	pthread_t manager; // 管理线程
 	pthread_t *threadList; // 线程池 消费者
};
// 消费者调用线程池
void *threadWork(void *arg);
// 管理者调用函数
void *manageFunc(void *arg);
// 将退出的线程置为0
void setListZero(MythreadPool *pool);
#endif
