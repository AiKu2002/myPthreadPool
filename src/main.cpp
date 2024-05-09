#include <iostream>
#include "Task.h"
#include "MythreadPool.h"
#include "Myfunc.h"
using namespace std;

void * producer(void *arg){
	MythreadPool * pool = (MythreadPool *)arg;
	static long long cnt_st = 0;
	while(1){
		pthread_mutex_lock(&pool->mutexTimer);
		int timer = pool->utime;
		pthread_mutex_unlock(&pool->mutexTimer);
		usleep(timer);
		Task tmp(&printInt, (void *)cnt_st );
               	pthread_mutex_lock(&pool->mutexPool);
		while( pool->TaskQ.size() == pool->maxNum && !pool->shutDown){
			pthread_cond_wait(&pool->notFull, &pool->mutexPool);
		}
		if(pool->shutDown){
			pthread_mutex_unlock(&pool->mutexPool);
			cout << "procucer is shutdowning..." << endl;
			pthread_exit(NULL);
		}
                pool->TaskQ.push(tmp);
                pthread_cond_signal(&pool->notEmpty);
               	pthread_mutex_unlock(&pool->mutexPool);
		cnt_st++;
	}
}

int main(void) {
	
	MythreadPool pool(2,20,128);
	pthread_t prod;
	int utime = 1000;
	pthread_create(&prod, NULL, producer,&pool);
	while(utime > 0){
		cin >> utime;
		pthread_mutex_lock(&pool.mutexTimer);
		pool.utime = utime;
		pthread_mutex_unlock(&pool.mutexTimer);
	}
	cout << "进程结束++++++++++++++++++++++++++++++++++++++++++" << endl;
	/*while(1) {
		char c;
		Task task(&printInt, (void*)cnt);
		cin >> c;
		if(c == 'q') break;
		task.function(task.arg);
		cnt++;
	}
	*/

	return 0;
}
