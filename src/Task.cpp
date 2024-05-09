#include "Task.h"
#include <iostream>
Task::Task( void (*func) (void *) ,void * func_arg){
	function = func;
	this->arg = func_arg;
}
        // 拷贝构造函数
Task::Task(const Task & task){
	function = task.function;
	arg = task.arg;
}
Task::~Task(){
	//std::cout << "call ~ Task" << std::endl;
}
