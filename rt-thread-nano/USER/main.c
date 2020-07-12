
#include "board.h"
#include "rtthread.h"


//静态方式创建LED1
static struct rt_thread led1_thread;
static void led1_thread_entry(void* parameter);

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_led1_thread_stack[1024];

//动态方式创建LED2
static rt_thread_t led2_thread = RT_NULL;
static void led2_thread_entry(void* parameter);

//KEY任务创建
#if 0
static rt_thread_t key_thread = RT_NULL;
static void key_thread_entry(void* parameter);
#endif

//消息队列数据结构创建
#if 0
static rt_mq_t test_mq = RT_NULL;
static rt_thread_t receive_thread = RT_NULL;
static rt_thread_t send_thread = RT_NULL;
static void receive_thread_entry(void *parameter);
static void send_thread_entry(void *parameter);
#endif


//信号量实验数据结构定义
#if 0
static rt_sem_t test_sem = RT_NULL;
uint8_t ucValue[2] = {0x00, 0x00};
static rt_thread_t sem_send_thread = RT_NULL;
static rt_thread_t sem_recv_thread = RT_NULL;
static void sem_recv_entry(void *parameter);
static void sem_send_entry(void *parameter);
#endif

//互斥量实验
//#define MUTEX_TEST_EXAM
#ifdef MUTEX_TEST_EXAM
static rt_mutex_t test_mutex = RT_NULL;
uint8_t uvMutexValue[2];
static rt_thread_t mutex_send_thread = RT_NULL;
static rt_thread_t mutex_recv_thread = RT_NULL;
static void mutex_recv_entry(void *parameter);
static void mutex_send_entry(void *parameter);
#endif

#define EVENT_FLAG_EXAM
#ifdef EVENT_FLAG_EXAM
static rt_event_t event_test = RT_NULL;
static rt_thread_t event_send_thread = RT_NULL;
static rt_thread_t event_recv_thread = RT_NULL;
static void event_recv_entry(void *parameter);
static void event_send_entry(void *parameter);

#define KEY1_EVENT		(0x01 << 0)
#define KEY2_EVENT		(0x01 << 1)

#endif



int main(void)
{
	rt_thread_init(&led1_thread,
					"led1",
					led1_thread_entry,
					RT_NULL,
					&rt_led1_thread_stack[0],
					sizeof(rt_led1_thread_stack),
					3,
					20);
				
	led2_thread = 
	rt_thread_create("led2",
					led2_thread_entry,
					RT_NULL,
					512,
					3,
					20);
					
	rt_thread_startup(&led1_thread);
	if(led2_thread != RT_NULL)
		rt_thread_startup(led2_thread);
	else
		return -1;
#if 0		
	key_thread = 
	rt_thread_create("key",
					key_thread_entry,
					RT_NULL,
					512,
					2,
					20);
	if(key_thread != RT_NULL)
		rt_thread_startup(key_thread);
	else
		return -1;
#endif
	
	
//消息队列实验
#if 0	
	test_mq = rt_mq_create("test_mq",
							40,
							20,
							RT_IPC_FLAG_FIFO);
	if(test_mq != RT_NULL)
		rt_kprintf("消息队列创建成功！\n\n");
	
	
	receive_thread = rt_thread_create("receive",
							receive_thread_entry,
							RT_NULL,
							512,
							3,
							20);
	if(receive_thread != RT_NULL)
		rt_thread_startup(receive_thread);
	else
		return -1;
	
	send_thread = rt_thread_create("send",
							send_thread_entry,
							RT_NULL,
							512,
							2,
							20);
	if(send_thread != RT_NULL)
		rt_thread_startup(send_thread);
	else
		return -1;
#endif

//信号量实验
#if 0
	test_sem = rt_sem_create("test_sem",
							5,
							RT_IPC_FLAG_FIFO);
	if(test_sem != RT_NULL)
		rt_kprintf("信号量创建成功!\n\n");
	
	sem_recv_thread = 
	rt_thread_create("sem_receive",
					sem_recv_entry,
					RT_NULL,
					512,
					3,
					20);
	if(sem_recv_thread != RT_NULL)
		rt_thread_startup(sem_recv_thread);
	else
		return -1;
	
	sem_send_thread = 
	rt_thread_create("sem_send",
					sem_send_entry,
					RT_NULL,
					512,
					2,
					20);
	if(sem_send_thread != RT_NULL)
		rt_thread_startup(sem_send_thread);
	else
		return -1;	
#endif
#ifdef MUTEX_TEST_EXAM
	test_mutex = 
	rt_mutex_create("test_mutex", RT_IPC_FLAG_PRIO);
	if(test_mutex != RT_NULL)
		rt_kprintf("创建互斥量成功！\n\n");
	
	mutex_send_thread =
	rt_thread_create("mutex_send_thread",
					mutex_send_entry,
					RT_NULL,
					512,
					2,
					20);
	if(mutex_send_thread != RT_NULL)
		rt_thread_startup(mutex_send_thread);
	else
		return -1;
	
	mutex_recv_thread = 
	rt_thread_create("mutex_recv_thread",
					mutex_recv_entry,
					RT_NULL,
					512,
					3,
					20);
	if(mutex_recv_thread != RT_NULL)
		rt_thread_startup(mutex_recv_thread);
	else
		return -1;
#endif

	
#ifdef EVENT_FLAG_EXAM
	event_test = 
	rt_event_create("event_test", RT_IPC_FLAG_PRIO);
	if(event_test != RT_NULL)
		rt_kprintf("事件创建成功！\n\n");
	
	event_recv_thread = 
	rt_thread_create("event_recv_thread",
					event_recv_entry,
					RT_NULL,
					512,
					3,
					20);
	if(event_recv_thread != RT_NULL)
		rt_thread_startup(event_recv_thread);
	else
		return -1;
	
	event_send_thread = 
	rt_thread_create("event_send_thread",
					event_send_entry,
					RT_NULL,
					512,
					2,
					20);
	if(event_send_thread != RT_NULL)
		rt_thread_startup(event_send_thread);
	else
		return -1;
	
#endif
	
	return 0;
}

static void led1_thread_entry(void* parameter)
{
	while(1)
	{
		LED0 = 0;
		rt_thread_delay(500);
		LED0 = 1;
		rt_thread_delay(500);
	}	
}
	
static void led2_thread_entry(void* parameter)
{
	while(1)
	{
		LED1 = 0;
		rt_thread_delay(300);
		LED1 = 1;
		rt_thread_delay(300);
	}	
}	


u8 KEY_Scan(void)
{
	u32 tempKeyVal, tempMask, i;
	static unsigned short keyFilterTimer[KEY_NUM] = {0};
	static u32 keyValue;
	
	tempKeyVal = KEY_Read();
	tempMask = 0;
	for(i = 0; i < KEY_NUM; i++)
	{
		tempMask = 0x01 << i;
		if((keyValue&tempMask) != (tempKeyVal&tempMask))
		{
			keyFilterTimer[i] += 10;
			if(keyFilterTimer[i] >= 50)
			{
				keyFilterTimer[i] = 0;
				keyValue = (keyValue & (~tempMask)) | (tempKeyVal&tempMask);
			}
		}
		else
		{
			keyFilterTimer[i] = 0;
		}
	}
	
	return keyValue;
}

#if 0
static void key_thread_entry(void* parameter)
{
	rt_uint32_t key = 0, ret = 0, keyBak = 0;
	
	while(1)
	{
		rt_thread_delay(10);
		key=KEY_Scan();	//得到键值
		if(keyBak != key)
		{
			if(key)
			{						   
				switch(key)
				{				 
					case WKUP_PRES:
						rt_kprintf("恢复LED2线程!\n");
						ret = rt_thread_resume(led2_thread);
						if(RT_EOK == ret)
							rt_kprintf("恢复LED2线程成功!\n");
						else
							rt_kprintf("恢复LED2线程失败!\n");
						break;
					case KEY2_PRES:	
						rt_kprintf("恢复LED1线程!\n");
						ret = rt_thread_resume(&led1_thread);
						if(RT_EOK == ret)
							rt_kprintf("恢复LED1线程成功!\n");
						else
							rt_kprintf("恢复LED1线程失败!\n");
						break;
					case KEY1_PRES:	//控制LED1翻转	 
						rt_kprintf("挂起LED2线程!\n");
						ret = rt_thread_suspend(led2_thread);
						if(RT_EOK == ret)
							rt_kprintf("挂起LED2线程成功!\n");
						else
							rt_kprintf("挂起LED2线程失败!\n");
						break;
					case KEY0_PRES:	//同时控制LED0,LED1翻转 
						rt_kprintf("挂起LED1线程!\n");
						ret = rt_thread_suspend(&led1_thread);
						if(RT_EOK == ret)
							rt_kprintf("挂起LED1线程成功!\n");
						else
							rt_kprintf("挂起LED1线程失败!\n");
						break;
				}
			}
		}
		
		keyBak = key;
	}	
}	

#endif

#if 0
static void receive_thread_entry(void *parameter)
{
	rt_err_t uwRet = RT_EOK;
	uint32_t r_queue;
	
	while(1)
	{
		uwRet = rt_mq_recv(test_mq, &r_queue, sizeof(r_queue), RT_WAITING_FOREVER);
		if(uwRet == RT_EOK)
		{
			rt_kprintf("本次接收到的数据是：%d\n", r_queue);
		}
		else
		{
			rt_kprintf("数据接收出错，错误代码：0x%lx\n", uwRet);
		}
		rt_thread_delay(200);
	}
}

static void send_thread_entry(void *parameter)
{
	rt_err_t uwRet = RT_EOK;
	uint32_t send_data1 = 1;
	uint32_t send_data2 = 2;
	uint32_t key = 0;
	static uint32_t keyBak = 0;
	
	while(1)
	{
		rt_thread_delay(10);
		
		key=KEY_Scan();	//得到键值
		if(keyBak != key)
		{
			keyBak = key;			
			switch(key)
			{				 
				case WKUP_PRES:
				case KEY2_PRES:	
					uwRet = rt_mq_send(test_mq, &send_data1, sizeof(send_data1));
					if(uwRet != RT_EOK)
						rt_kprintf("数据不能发送到消息队列！错误代码：%lx\n", uwRet);
					break;
				case KEY1_PRES:
				case KEY0_PRES:
					uwRet = rt_mq_send(test_mq, &send_data2, sizeof(send_data2));
					if(uwRet != RT_EOK)
						rt_kprintf("数据不能发送到消息队列！错误代码：%lx\n", uwRet);
					break;
				default:
					break;
			}
		}
	}
}

#endif

#if 0
static void sem_send_entry(void *parameter)
{
	rt_err_t uwRet = RT_EOK;
	uint32_t key = 0;
	uint32_t keyBak = 0;
	
	while(1)
	{
		#if 0
		rt_sem_take(test_sem, RT_WAITING_FOREVER);
		ucValue[0]++;
		rt_thread_delay(100);
		ucValue[1]++;
		rt_sem_release(test_sem);
		rt_thread_yield();
		#endif
		
		
		rt_thread_delay(10);
		
		key=KEY_Scan();	//得到键值
		if(keyBak != key)
		{
			keyBak = key;			
			switch(key)
			{				 
				case KEY1_PRES:
					if(test_sem->value < 5)
					{
						uwRet = rt_sem_release(test_sem);
						if(uwRet == RT_EOK)
							rt_kprintf("KEY1被按下：释放一个车位，当前车位数：%d\n", test_sem->value);
						else
							rt_kprintf("KEY1被按下：但已无停车位可释放！\n");
					}
					else
					{
						rt_kprintf("KEY1被按下：但已无停车位可释放！\n");
					}
					break;
				default:
					break;
			}
		}
	}
}


static void sem_recv_entry(void *parameter)
{
	rt_err_t uwRet = RT_EOK;
	uint32_t key = 0;
	uint32_t keyBak = 0;
	
	while(1)
	{
		#if 0
		rt_sem_take(test_sem, RT_WAITING_FOREVER);
		if(ucValue[0] == ucValue[1])
			rt_kprintf("Successful!\n");
		else
			rt_kprintf("Failed.\n");
		rt_sem_release(test_sem);
		rt_thread_delay(1000);
		#endif
		
		rt_thread_delay(10);
		
		key=KEY_Scan();	//得到键值
		if(keyBak != key)
		{
			keyBak = key;			
			switch(key)
			{				 
				case KEY0_PRES:
					uwRet = rt_sem_take(test_sem, 0);
					if(uwRet == RT_EOK)
						rt_kprintf("KEY0被按下：成功申请到一个车位，剩余车位数%d.\n", test_sem->value);
					else
						rt_kprintf("KEY0被按下：不好意思，现在停车位已满！\n");
					break;
				default:
					break;
			}
		}
		
	}
}
#endif

#ifdef MUTEX_TEST_EXAM
static void mutex_send_entry(void *parameter)
{
	while(1)
	{
		rt_mutex_take(test_mutex, RT_WAITING_FOREVER);
		uvMutexValue[0]++;
		rt_thread_delay(100);
		uvMutexValue[1]++;
		rt_mutex_release(test_mutex);
		rt_thread_yield();
	}
}


static void mutex_recv_entry(void *parameter)
{
	while(1)
	{
		rt_mutex_take(test_mutex, RT_WAITING_FOREVER);
		if(uvMutexValue[0] == uvMutexValue[1])
			rt_kprintf("Successful!\n");
		else
			rt_kprintf("Failed.\n");
		rt_mutex_release(test_mutex);
		rt_thread_delay(1000);
	}
}
#endif



#ifdef EVENT_FLAG_EXAM
static void event_recv_entry(void *parameter)
{
	rt_uint32_t recved;
	
	while(1)
	{
		rt_event_recv(event_test,
						KEY1_EVENT|KEY2_EVENT,
						RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
						RT_WAITING_FOREVER,
						&recved);
		if(recved == (KEY1_EVENT|KEY2_EVENT))
			rt_kprintf("Key1与Key2都按下！\n");
		else
			rt_kprintf("事件错误！\n");
	}
}

static void event_send_entry(void *parameter)
{
	uint32_t key = 0;
	uint32_t keyBak = 0;
	
	while(1)
	{
		rt_thread_delay(10);
		
		key=KEY_Scan();	//得到键值
		if(keyBak != key)
		{
			keyBak = key;			
			switch(key)
			{				 
				case KEY1_PRES:
					rt_event_send(event_test, KEY1_EVENT);
					break;
				case KEY2_PRES:
					rt_event_send(event_test, KEY2_EVENT);
					break;
				default:
					break;
			}
		}
		
	}
}


#endif
