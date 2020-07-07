
#include "board.h"
#include "rtthread.h"


//��̬��ʽ����LED1
static struct rt_thread led1_thread;
static void led1_thread_entry(void* parameter);

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_led1_thread_stack[1024];

//��̬��ʽ����LED2
static rt_thread_t led2_thread = RT_NULL;
static void led2_thread_entry(void* parameter);

//KEY���񴴽�
static rt_thread_t key_thread = RT_NULL;
static void key_thread_entry(void* parameter);

//��Ϣ�������ݽṹ����
static rt_mq_t test_mq = RT_NULL;
static rt_thread_t receive_thread = RT_NULL;
static rt_thread_t send_thread = RT_NULL;
static void receive_thread_entry(void *parameter);
static void send_thread_entry(void *parameter);


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
	
	
	//��Ϣ����ʵ��
	test_mq = rt_mq_create("test_mq",
							40,
							20,
							RT_IPC_FLAG_FIFO);
	if(test_mq != RT_NULL)
		rt_kprintf("��Ϣ���д����ɹ���\n\n");
	
	
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


static void key_thread_entry(void* parameter)
{
	rt_uint32_t key = 0, ret = 0, keyBak = 0;
	
	while(1)
	{
		rt_thread_delay(10);
		key=KEY_Scan();	//�õ���ֵ
		if(keyBak != key)
		{
			if(key)
			{						   
				switch(key)
				{				 
					case WKUP_PRES:
						rt_kprintf("�ָ�LED2�߳�!\n");
						ret = rt_thread_resume(led2_thread);
						if(RT_EOK == ret)
							rt_kprintf("�ָ�LED2�̳߳ɹ�!\n");
						else
							rt_kprintf("�ָ�LED2�߳�ʧ��!\n");
						break;
					case KEY2_PRES:	
						rt_kprintf("�ָ�LED1�߳�!\n");
						ret = rt_thread_resume(&led1_thread);
						if(RT_EOK == ret)
							rt_kprintf("�ָ�LED1�̳߳ɹ�!\n");
						else
							rt_kprintf("�ָ�LED1�߳�ʧ��!\n");
						break;
					case KEY1_PRES:	//����LED1��ת	 
						rt_kprintf("����LED2�߳�!\n");
						ret = rt_thread_suspend(led2_thread);
						if(RT_EOK == ret)
							rt_kprintf("����LED2�̳߳ɹ�!\n");
						else
							rt_kprintf("����LED2�߳�ʧ��!\n");
						break;
					case KEY0_PRES:	//ͬʱ����LED0,LED1��ת 
						rt_kprintf("����LED1�߳�!\n");
						ret = rt_thread_suspend(&led1_thread);
						if(RT_EOK == ret)
							rt_kprintf("����LED1�̳߳ɹ�!\n");
						else
							rt_kprintf("����LED1�߳�ʧ��!\n");
						break;
				}
			}
		}
		
		keyBak = key;
	}	
}	




static void receive_thread_entry(void *parameter)
{
	rt_err_t uwRet = RT_EOK;
	uint32_t r_queue;
	
	while(1)
	{
		uwRet = rt_mq_recv(test_mq, &r_queue, sizeof(r_queue), RT_WAITING_FOREVER);
		if(uwRet == RT_EOK)
		{
			rt_kprintf("���ν��յ��������ǣ�%d\n", r_queue);
		}
		else
		{
			rt_kprintf("���ݽ��ճ���������룺0x%lx\n", uwRet);
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
		
		key=KEY_Scan();	//�õ���ֵ
		if(keyBak != key)
		{
			keyBak = key;			
			switch(key)
			{				 
				case WKUP_PRES:
				case KEY2_PRES:	
					uwRet = rt_mq_send(test_mq, &send_data1, sizeof(send_data1));
					if(uwRet != RT_EOK)
						rt_kprintf("���ݲ��ܷ��͵���Ϣ���У�������룺%lx\n", uwRet);
					break;
				case KEY1_PRES:
				case KEY0_PRES:
					uwRet = rt_mq_send(test_mq, &send_data2, sizeof(send_data2));
					if(uwRet != RT_EOK)
						rt_kprintf("���ݲ��ܷ��͵���Ϣ���У�������룺%lx\n", uwRet);
					break;
				default:
					break;
			}
		}
	}
}

