#ifndef __TASK_H__
#define __TASK_H__

class Task{
public:
	// 默认构造函数为删除
	Task()=delete;
	Task( void (*func) (void *) ,void * arg);
	// 拷贝构造函数
	Task(const Task & task);
	// 构建析构函数
	~Task();
	void (*function)(void *);
	void *arg;
};

#endif
