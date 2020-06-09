
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
; * void PendSV_Handler(void);
; * r0 --> switch from thread stack
; * r1 --> switch to thread stack
; * psr, pc, lr, r12, r3, r2, r1, r0 are pushed into [from] stack
; *----------------------------------------------------------------------
; */

PendSV_Handler      PROC
    EXPORT PendSV_Handler

    ;禁止中断，为了保护上下文切换不被中断
    MRS     r2, PRIMASK
    CPSID   I

    ;获取中断标识位
    ;C实现 if(rt_thread_switch_interrupt_flag == 0)
    ;           goto pendsv_exit;

    LDR     r0, =rt_thread_switch_interrupt_flag
    LDR     r1, [r0]
    CBZ     r1, pendsv_exit

    ;清除中断标志位
    ;C实现 rt_thread_switch_interrupt_flag = 0;
    MOV     r1, #0x00
    STR     r1, [r0]

    ;判断rt_interrupt_from_thread的值是否为0
    ;C实现 if(rt_interrupt_from_thread == 0)
    ;           goto switch_to_thread
    LDR     r0, =rt_interrupt_from_thread
    LDR     r1, [r0]
    CBZ     r1, switch_to_thread                ;第一次线程切换肯定为0

    ;========================上文保存========================
    ;将r4-r11入栈
    MRS     r1, psp 
    STMFD   r1!, {r4 - r11}

    ;加载r0指向的值到r0，即r0=rt_interrupt_from_thread
    LDR     r0, [r0]
    ;将r1的值存储到r0，即更新线程栈sp
    STR     r1, [r0]

    ;========================下文切换========================
switch_to_thread
    ;加载rt_interrupt_to_thread的地址到r1
    LDR     r1, =rt_interrupt_to_thread

    ;加载rt_interrupt_to_thread的值到r1，即sp指针的指针
    LDR     r1, [r1]
    ;加载rt_interrupt_to_thread的地址到r1，即sp
    LDR     r1, [r1]

    ;将线程栈指针r1(操作之前先递减)指向的内容加载到CPU寄存器r4-r11
    LDMFD   r1!, {r4 - r11}
    ;将线程栈指针更新到psp
    MSR     psp, r1

pendsv_exit
    ;恢复中断
    MSR     PRIMASK, r2

    ;确保异常返回使用的栈指针是PSP，即LR寄存器的位2要为1
    ORR     lr, lr, #0x04

    ;异常返回
    BX      lr

    ;PendSV_Handler子程序结束
    ENDP

;/*
; *----------------------------------------------------------------------
; * void rt_hw_context_switch(rt_uint32 from, rt_uint32 to);
; * r0 --> from
; * r1 --> to
; *----------------------------------------------------------------------
; */
rt_hw_context_switch    PROC

    LDR r2, =rt_thread_switch_interrupt_flag
    LDR r3, [r2]
    CMP r3, #1
    BEQ _reswitch
    MOV r3, #1
    STR r3, [r2]

    LDR r2, =rt_interrupt_from_thread
    STR     r0, [r2]

_reswitch
    LDR     r2, =rt_interrupt_to_thread
    STR     r1, [r2]

    LDR     r0, =NVIC_INT_CTRL
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]

    BX      LR

    ENDP

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


