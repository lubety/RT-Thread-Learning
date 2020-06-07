#ifndef __RT_THREAD_H__
#define __RT_THREAD_H__

#include <rtconfig.h>
//#include <rtdebug.h>
#include <rtdef.h>
#include <rtservice.h>
//#include <rtm.h>



rt_err_t rt_thread_init(struct rt_thread *thread,
						void (*entry)(void *parameter),
						void			*parameter,
						void			*stack_start,
						rt_uint32_t		stack_size);



#endif
