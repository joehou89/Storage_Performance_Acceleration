/*******************************************************************************
 Copyright (c) 2025, Hangzhou Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                                      ARCCache.cc
工程代码: ARC
模块名字: ARC缓存
创建时间: 2025-03-16
    作者: joehou89
    描述: ARC类实现
--------------------------------------------------------------------------------
*******************************************************************************/


#include "ARCCacheHeader.h"

// 将元素输入ARC缓存表中
void ARCcache::cache_insert(DataType data) {
    bool find = false;
    // 先在各个表的ghost中搜索
    if (this->Rcache->check_ghost(data)) { // 这个删除
        // 缩减Fcache, 如果不成功,则调用put函数
        if (this->Fcache->Subtract()) {
            this->Rcache->Add(data);  // 扩容Rcache
        }
        else { // Fcache缩容失败,此时直接加入Rcache中
            this->Rcache->put(data);
            // 进行put一次，由于在ghost列表中找到,
            // 原先数组中应该没有这个数，理论上不用if
        }
        find = true;
    }
    if (this->Fcache->check_ghost(data)) {
        if (this->Rcache->Subtract()) {
            this->Fcache->Add(data); // 扩容
        }
        else {
            this->Fcache->put(data); // 不扩容直接放
        }
        find = true;
    }

    // 如果两个的ghost中都没有对应的数, 则不改变partition(两个容量不改变)
    if (!find) {
        // 优先加入LRUcache中, 作为第一个元素
        if (this->Rcache->put(data)) this->Fcache->put(data);
        // 使用put函数向Fcache中也添加对应元素
    }
}

void ARCcache::Show(bool show_ghost) {
    cout << "%%%%%%%%%%%%%%%%%%% Begin %%%%%%%%%%%%%%%%%%%%%%" << endl;
    this->Rcache->Show(show_ghost);
    this->Fcache->Show(show_ghost);
    cout << "%%%%%%%%%%%%%%%%%%%% end %%%%%%%%%%%%%%%%%%%%%%%" << endl;
}
