/*******************************************************************************
 Copyright (c) 2025, Hangzhou Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                                      LFUCacheHeader.h
工程代码: LFU
模块名字: LFU缓存
创建时间: 2025-03-15
    作者: joehou89
    描述: LFU类头文件
--------------------------------------------------------------------------------
*******************************************************************************/
#pragma once 
#include <iostream>
#include <string>
#include <unordered_map>

#ifndef ARC_CACHE_HEADER_H
#define ARC_CACHE_HEADER_H
#define CACHE_CAPACITY 30
#define CACHE_TRANSFROM_COUNT 3
using namespace std;

typedef unsigned long uint64;