
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


u8 KEY_Scan(void)
{
	u32 tempKeyVal, tempMask, i;
	static unsigned short keyFilterTimer[KEY_NUM] = {0};
	static u32 keyValue;
	
	rt_thread_delay(10);
	
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


static void key_thread_entry(void* parameter)
{
	rt_uint32_t key = 0, ret = 0, keyBak = 0;
	
	while(1)
	{
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



