
#include <rtthread.h>
#include "ARMCM3.h"

ALIGN(RT_ALIGN_SIZE)

/* �����߳�ջ */
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];

struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;


unsigned int flag1;
unsigned int flag2;

/* �����ʱ */
void delay(unsigned int count)
{
	for(;count > 0; count--);
}

/* �߳�1 */
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

/* �߳�2 */
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
	/* Ӳ����ʼ�� */
	/* ��Ӳ����صĳ�ʼ��������������������棬��û����س�ʼ������ */
	
	/* ��������ʼ�� */
	rt_system_schedule_init();

	/* ��ʼ���߳� */
	rt_thread_init(&rt_flag1_thread,
					flag1_thread_entry,
					RT_NULL,
					&rt_flag1_thread_stack[0],
					sizeof(rt_flag1_thread_stack));

	/* ���̲߳��뵽������ */
	rt_list_insert_before(&(rt_thread_priority_table[0]), &(rt_flag1_thread.tlist));

	
	/* ��ʼ���߳� */
	rt_thread_init(&rt_flag2_thread,
					flag2_thread_entry,
					RT_NULL,
					&rt_flag2_thread_stack[0],
					sizeof(rt_flag2_thread_stack));

	/* ���̲߳��뵽������ */
	rt_list_insert_before(&(rt_thread_priority_table[1]), &(rt_flag2_thread.tlist));
	

	/* ����ϵͳ������ */
	rt_system_schedule_start();
	
	return 0;
}














