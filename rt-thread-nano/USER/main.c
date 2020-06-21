
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


