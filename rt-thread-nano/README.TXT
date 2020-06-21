 学习记录
《13、移植RT-Thread到STM32》
移植过程：
1、下载rtt源码
2、将源码添加入工程
3、修改rtconfig.h头文件配置rtt
4、修改board.c文件
5、注意使用HAL库要重写HAL_InitTick()、HAL_GetTick()、HAL_Delay()三个函数来支持HAL库中（I2C、SPI、SDIO等驱动的延时）