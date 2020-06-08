
;************************************************************************ 
;                               全局变量 
;************************************************************************
    IMPORT rt_thread_switch_interrupt_flag
    IMPORT rt_interrupt_from_thread
    IMPORT rt_interrupt_to_thread

;************************************************************************ 
;                               全局常量 
;************************************************************************
SCB_VTOR        EQU     0xE000ED08               ; Vector Table Offset Register
NVIC_INT_CTRL   EQU     0xE000ED04               ; interrupt control state register
NVIC_SYSPRI2    EQU     0xE000ED20               ; system priority register (2)
NVIC_PENDSV_PRI EQU     0x00FF0000               ; PendSV priority value (lowest)
NVIC_PENDSVSET  EQU     0x10000000               ; value to trigger PendSV exception


;************************************************************************ 
;                               代码产生指令 
;************************************************************************

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

;/*
; *----------------------------------------------------------------------
; * 函数原型:void rt_hw_context_switch_to(rt_uint32 to);
; * r0 --> to
; * 该函数用于开启第一次线程切换
; *----------------------------------------------------------------------
; */
rt_hw_context_switch_to     PROC

    ;导出rt_hw_context_switch_to，让其具有全局属性，可以让C文件调用
    EXPORT rt_hw_context_switch_to

    ;设置rt_interrupt_to_thread的值
    ;C实现  rt_interrupt_to_thread = to;
    LDR r1, =rt_interrupt_to_thread
    STR r0, [r1]

    ;C实现  rt_interrupt_from_thread = 0x00;
    LDR r1, =rt_interrupt_from_thread
    MOV r0, #0x00
    STR r0, [r1]

    ;C实现  rt_thread_switch_interrupt_flag = 1;
    LDR r1, =rt_thread_switch_interrupt_flag
    MOV r0, #1
    STR r0, [r1]

    ;设置PendSV的异常优先级
    ;C实现  NVIC_SYSPRI2 = NVIC_SYSPRI2 | NVIC_PENDSV_PRI
    LDR r0, =NVIC_SYSPRI2
    LDR r1, =NVIC_PENDSV_PRI
    LDR.W r2, [r0,#0x00]        ;读
    ORR r1,r1,r2                ;改
    STR r1, [r0]                ;写

    ;触发PendSV异常，产生上下文中断
    ;C实现  NVIC_INT_CTRL = NVIC_PENDSVSET
    LDR r0, =NVIC_INT_CTRL
    LDR r1, =NVIC_PENDSVSET
    STR r1, [r0]

    ;开中断
    CPSIE F 
    CPSIE I 

    ENDP            ;永远不会到这里


    ALIGN   4       ;当前文件指令代码要求4字节对齐，不然会有警告。

    END             ;汇编文件结束，每个汇编文件都需要一个 END。


