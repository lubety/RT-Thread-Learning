
#include <rtthread.h>
#include "ARMCM3.h"

ALIGN(RT_ALIGN_SIZE)

/* 定义线程栈 */
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];

struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;


unsigned int flag1;
unsigned int flag2;

/* 软件延时 */
void delay(unsigned int count)
{
	for(;count > 0; count--);
}

/* 线程1 */
void flag1_thread_entry( void *p_arg )
{
	for(;;)
	{
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
	}
}

/* 线程2 */
void flag2_thread_entry( void *p_arg )
{
	for(;;)
	{
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
	}
}


int main(void)
{
	/* 硬件初始化 */
	/* 将硬件相关的初始化放在这里，如果是软件仿真，则没有相关初始化代码 */
	
	/* 调度器初始化 */
	rt_system_schedule_init();

	/* 初始化线程 */
	rt_thread_init(&rt_flag1_thread,
					flag1_thread_entry,
					RT_NULL,
					&rt_flag1_thread_stack[0],
					sizeof(rt_flag1_thread_stack));

	/* 将线程插入到就绪表 */
	rt_list_insert_before(&(rt_thread_priority_table[0]), &(rt_flag1_thread.tlist));

	
	/* 初始化线程 */
	rt_thread_init(&rt_flag2_thread,
					flag2_thread_entry,
					RT_NULL,
					&rt_flag2_thread_stack[0],
					sizeof(rt_flag2_thread_stack));

	/* 将线程插入到就绪表 */
	rt_list_insert_before(&(rt_thread_priority_table[1]), &(rt_flag2_thread.tlist));
	

	/* 启动系统调度器 */
	rt_system_schedule_start();
	
	return 0;
}














