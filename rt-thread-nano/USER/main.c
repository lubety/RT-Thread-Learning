
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
static rt_thread_t key_thread = RT_NULL;
static void key_thread_entry(void* parameter);

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

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
	{
		rt_thread_delay(10);//去抖动 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(WK_UP==1)return WKUP_PRES;
	}
	else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)
	{
		rt_thread_delay(10);//去抖动 
		key_up=1;
	}


	
 	return 0;// 无按键按下
}


static void key_thread_entry(void* parameter)
{
	rt_uint32_t key = 0, ret = 0;
	
	while(1)
	{
		key=KEY_Scan(0);	//得到键值
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
}	



