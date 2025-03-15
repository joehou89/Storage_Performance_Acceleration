/*******************************************************************************
 Copyright (c) 2025, Hangzhou Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                                      infra_comp.h
工程代码: infra_comp
模块名字: 核心数据结构体
创建时间: 2025-03-15
    作者: joehou89
    描述: 数据结构体头文件
--------------------------------------------------------------------------------
*******************************************************************************/

#define CACHE_CAPACITY 30
#define CACHE_TRANSFROM_COUNT 3

typedef unsigned long uint64;
typedef string DataType;

//缓存节点列表结构体
typedef struct CacheList {
    DataType key;  
    uint64 time = 0;                 //定义使用计数器，计算使用次数
    CacheList* prev;
    CacheList* next;
};