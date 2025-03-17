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
    if (_lrucache->cache_check_ghost(data)) {
        if (_lfucache->cache_evict_and_subtract()) {
            _lrucache->cache_insert_and_extend(data);
        }
        else {
            _lrucache->cache_insert(data);
        }
        find = true;
    }
    if (_lfucache->cache_check_ghost(data)) {
        if (_lrucache->cache_evict_and_subtract()) {
            _lfucache->cache_insert_and_extend(data);
        }
        else {
            _lfucache->cache_insert(data);
        }
        find = true;
    }

    // 如果两个的ghost中都没有对应的数, 则不改变partition(两个容量不改变)
    if (!find) {
        // 优先加入LRUcache中, 作为第一个元素
        if (_lrucache->cache_insert(data)) {
            _lfucache->cache_insert(data);
        }
    }
}

void ARCcache::Show(bool show_ghost) {
    cout << "%%%%%%%%%%%%%%%%%%% Begin %%%%%%%%%%%%%%%%%%%%%%" << endl;
    this->Rcache->Show(show_ghost);
    this->Fcache->Show(show_ghost);
    cout << "%%%%%%%%%%%%%%%%%%%% end %%%%%%%%%%%%%%%%%%%%%%%" << endl;
}
