/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-21     RealThread   first version
 */

#include <rtthread.h>
#include <board.h>
#include <drv_common.h>

#if defined(RT_USING_HEAP)
//typedef struct
//{
//    void *ptr;
//    rt_size_t size;
//}memrec_t;
//
//#define MEM_REC_ARR     500
//memrec_t MemRecArr[MEM_REC_ARR] = {{0}};
//unsigned int MemUsedSlot = 0;
//unsigned int MemUsed = 0;
//
//void MallocHook(void * ptr, rt_size_t size)
//{
//    unsigned int i = 0;
//
//    if(MemUsedSlot >= MEM_REC_ARR)
//    {
//        rt_kprintf("Malloc record array is full.\r\n");
//    }
//    else
//    {
//        do
//        {
//            if(MemRecArr[i].ptr == NULL)
//            {
//                if(i < MEM_REC_ARR)
//                {
//                    MemRecArr[i].ptr = ptr;
//                    MemRecArr[i].size = size;
//                    MemUsed += size;
//                    MemUsedSlot++;
//                    rt_kprintf("MemUsed = %d(%d-%d),MemUsedSlot = %d(%d-%d).\r\n",MemUsed,size,((unsigned int)HEAP_END - (unsigned int)HEAP_BEGIN),MemUsedSlot,i,MEM_REC_ARR);
//                    break;
//                }
//            }
//        }while(i++ < MEM_REC_ARR);
//    }
//
//    return;
//}

//void FreeHook(void * ptr)
//{
//    unsigned int i = 0;
//    do
//    {
//       if(MemRecArr[i].ptr == ptr)
//       {
//           MemUsed -= MemRecArr[i].size;
//           MemRecArr[i].ptr = NULL;
//           MemRecArr[i].size = 0;
//           MemUsedSlot--;
//           break;
//       }
//    }while(i++ < MEM_REC_ARR);
//
//    return;
//}

#endif

RT_WEAK void rt_hw_board_init()
{
    extern void hw_board_init(char *clock_src, int32_t clock_src_freq, int32_t clock_target_freq);

    /* Heap initialization */
#if defined(RT_USING_HEAP)
//    rt_malloc_sethook(MallocHook);
//    rt_free_sethook(FreeHook);
    rt_system_heap_init((void *) HEAP_BEGIN, (void *) HEAP_END);
#endif

    hw_board_init(BSP_CLOCK_SOURCE, BSP_CLOCK_SOURCE_FREQ_MHZ, BSP_CLOCK_SYSTEM_FREQ_MHZ);

    /* Set the shell console output device */
#if defined(RT_USING_DEVICE) && defined(RT_USING_CONSOLE)
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    /* Board underlying hardware initialization */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

}
