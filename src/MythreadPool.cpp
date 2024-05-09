#include "MythreadPool.h"
#include "Myfunc.h"
#define NumChange 2
// #define PCDebug
using namespace std;

MythreadPool::MythreadPool(int min , int max, int capacity){
	minNum = min;
	maxNum = max;
	capacityNum = capacity;
	// 创建管理者线程
	pthread_create(&manager, NULL, manageFunc, this);
	// 创建消费线程
	threadList = new pthread_t[max];
	for(int i = 0; i < maxNum; i++ ){
		threadList[i] = 0;
	} 
	if(!threadList) {
		cerr << "threadList init error";
		exit(1);
	}
	for(int i = 0 ; i < min ; i++ ) {
		pthread_create(&threadList[i], NULL, threadWork,this);
	}
	//  线程数据 
	busyNum = 0;
      	liveNum = min;
	exitNum = 0;
        shutDown = false;
	utime = 1000;
	// 初始化锁与条件变量
	if(pthread_mutex_init(&mutexPool, NULL) ||
		pthread_mutex_init(&mutexBusy, NULL) || 
		pthread_cond_init (&notFull,NULL) || 
	        pthread_cond_init(&notEmpty ,NULL) ||
		pthread_mutex_init(&mutexTimer,NULL))
	{
		cerr  << "pthread_mutext or cond init error";
		exit(1);
	}
}
MythreadPool:: ~MythreadPool(){
	// 删除线程申请的数据
	cout << "开始关闭" << endl;
	shutDown = 1;
	pthread_join(manager, NULL);
	pthread_mutex_destroy(&mutexPool);
	pthread_mutex_destroy(&mutexBusy);
	pthread_mutex_destroy(&mutexTimer);
	pthread_cond_destroy(&notFull);
	pthread_cond_destroy(&notEmpty);
	delete [] threadList;
	cout << "call ~ MythreadPool" << endl;
}
void *threadWork(void *arg){
	// 线程处理工作
	MythreadPool * pool = (MythreadPool *) arg;
	while(1){
		usleep(10000);
	// 获取到进程池的锁
		pthread_mutex_lock(&pool->mutexPool);
			//为了避免虚假唤醒导致程序逻辑错误，需要在while循环中重新检查条件，直到条件满足为止。
			while(pool->TaskQ.size() == 0 && !pool->shutDown){
				pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);
				if(pool->exitNum > 0) {
					pool->liveNum--;		
					pool->exitNum--;
					// 关闭进程
					setListZero(pool); // 进程置零
					cout << "d-------------关闭进程--------------d" << endl;

					pthread_mutex_unlock(&pool->mutexPool);
					pthread_exit(NULL);
				}
			}
		pthread_mutex_unlock(&pool->mutexPool);
		if(pool->shutDown) {
			// 进程关闭
			cout << "consumer is shutDowning..." << endl;
			pthread_exit(NULL);
		}
		/*
		 * 1 取出要处理的消息队列数据
		 * 2 处理数据 更改繁忙状态的线程数据
		 * 3 处理数据并且打印
 		*/
		pthread_mutex_lock(&pool->mutexPool);
		Task consum(pool->TaskQ.front());
		pool->TaskQ.pop();
		pthread_cond_signal(&pool->notFull);
		pthread_mutex_unlock(&pool->mutexPool);
		pthread_mutex_lock(&pool->mutexBusy);
		pool->busyNum++;
		pthread_mutex_unlock(&pool->mutexBusy);
		#ifdef PCDegug
		cout << "consum pid is " << pthread_self() << " working numID is";
		(consum.function)(consum.arg);
		cout << endl;
		#endif
		pthread_mutex_lock(&pool->mutexBusy);
		pool->busyNum--;
		pthread_mutex_unlock(&pool->mutexBusy);
	}
}

// 管理者调用函数
void *manageFunc(void *arg) {
	MythreadPool * pool = (MythreadPool * ) arg;
	while(1) {
		sleep(2);
		
		if(pool->shutDown){
			// 关闭线程
			cout << "manager is shutdowing ..." << endl;
			pthread_exit(NULL);
		}
		cout << "pid is " << pthread_self() << " managing" << endl;
		pthread_mutex_lock(&pool->mutexPool);
		int live = pool->liveNum;
		//int exit = pool->exitNum;
		int tk = pool->TaskQ.size(); 
		pthread_mutex_unlock(&pool->mutexPool);
		pthread_mutex_lock(&pool->mutexBusy);
		int busy = pool->busyNum;
		pthread_mutex_unlock(&pool->mutexBusy);
		cout << "live is :" << live << endl;
		cout << "busy is :" << busy << endl;
		cout << "task is :" << tk << endl;
		// 关闭今进程
		// 忙线程数 * 2 < 进程数量  且进程数量 > 最小进程数
		int cnt_a = 0;
		for(int i = 0; cnt_a < NumChange && busy * 2 < live && live > pool->minNum ;i++){
			pthread_mutex_lock(&pool->mutexPool);
			pool->exitNum++;
			// 让进程自杀
			pthread_cond_signal(&pool->notEmpty);
			pthread_mutex_unlock(&pool->mutexPool);
			live--;
			cnt_a++;
		}
		// 新启动进程
		// 当任务数量 > 进程数量 且进程数量 < 最大进程数
		int cnt_b = 0;
		for(int i = 0; i < pool->maxNum &&\
				cnt_b < NumChange && tk > live &&\
				 live < pool->maxNum; i++) {
			
			if(pool->threadList[i] == 0) { // 这个处于休眠状态
				pthread_create(&pool->threadList[i], NULL, threadWork, pool);
				live++;
				cout << "d-------------进程创建--------------d" << endl;
				pthread_mutex_lock(&pool->mutexPool);
				pool->liveNum++;
				pthread_mutex_unlock(&pool->mutexPool);
				cnt_b++;
				
			}
		}
	}

}
void setListZero(MythreadPool *pool)
{	
	pthread_t tid =  pthread_self();
	for(int i = 0; i < pool->maxNum; i++) {
		if(pool->threadList[i] == tid){
			pool->threadList[i] = 0;
			break;
		}
	}	
}
