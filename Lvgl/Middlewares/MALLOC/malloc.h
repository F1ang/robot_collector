/**
 ****************************************************************************************************
 * @file        malloc.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-11-04
 * @brief       �ڴ���� ����
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� F407���������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211104
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __MALLOC_H
#define __MALLOC_H
#include "sys.h"


#ifndef NULL
#define NULL 0
#endif

/* ���������ڴ�� */
#define     SRAMIN                  0                               /* �ڲ��ڴ�� */
#define     SRAMCCM                 1                               /* CCM�ڴ��(�˲���SRAM����CPU���Է���!!!) */
#define     SRAMBANK                2                               /* ����֧�ֵ�SRAM���� */

/* �����ڴ���������,������SDRAM��ʱ�򣬱���ʹ��uint32_t���ͣ��������uint16_t���Խ�ʡ�ڴ�ռ�� */
#define MT_TYPE     uint16_t

/* mem1�ڴ�����趨.mem1��ȫ�����ڲ�SRAM���� */
#define     MEM1_BLOCK_SIZE         32                              /* �ڴ���СΪ32�ֽ� */
#define     MEM1_MAX_SIZE           10 * 1024                       /* �������ڴ� 100K */
#define     MEM1_ALLOC_TABLE_SIZE   MEM1_MAX_SIZE/MEM1_BLOCK_SIZE   /* �ڴ���С */

/* mem2�ڴ�����趨.mem2����CCM,���ڹ���CCM(�ر�ע��,�ⲿ��SRAM,��CPU���Է���!!) */
#define     MEM2_BLOCK_SIZE         32                              /* �ڴ���СΪ32�ֽ� */
#define     MEM2_MAX_SIZE           60 * 1024                       /* �������ڴ� 60K */
#define     MEM2_ALLOC_TABLE_SIZE   MEM2_MAX_SIZE/MEM2_BLOCK_SIZE   /* �ڴ���С */

/* �ڴ��������� */
struct _m_mallco_dev
{
    void (*init)(uint8_t );             /* ��ʼ�� */
    uint8_t (*perused)(uint8_t );       /* �ڴ�ʹ���� */
    uint8_t *membase[SRAMBANK];         /* �ڴ�� ����SRAMBANK��������ڴ� */
    uint16_t *memmap[SRAMBANK];         /* �ڴ����״̬�� */
    uint8_t memrdy[SRAMBANK];           /* �ڴ�����Ƿ���� */
};

extern struct _m_mallco_dev mallco_dev; /* ��mallco.c���涨�� */

void mymemset(void * s, uint8_t c, uint32_t count);         /* �����ڴ� */
void mymemcpy(void * des, void * src, uint32_t n);          /* �����ڴ� */
void my_mem_init(uint8_t memx);                             /* �ڴ�����ʼ������(��/�ڲ�����) */
uint32_t my_mem_malloc(uint8_t memx, uint32_t size);        /* �ڴ����(�ڲ�����) */
uint8_t my_mem_free(uint8_t memx, uint32_t offset);         /* �ڴ��ͷ�(�ڲ�����) */
uint8_t my_mem_perused(uint8_t memx);                       /* ����ڴ�ʹ����(��/�ڲ�����) */

/* �û����ú��� */
void myfree(uint8_t memx, void * ptr);                      /* �ڴ��ͷ�(�ⲿ����) */
void *mymalloc(uint8_t memx, uint32_t size);                /* �ڴ����(�ⲿ����) */
void *myrealloc(uint8_t memx, void * ptr, uint32_t size);   /* ���·����ڴ�(�ⲿ����) */
#endif
