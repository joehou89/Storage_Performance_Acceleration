#include "../include/ARCCacheHeader.h"

// 使用要求: 可以一次输入多个词组，并将每一个词组放置在缓存中，
int main()
{
    ARCcache container(5, 3); 
    // 注意: 这个是生成一个大小为10 (5x2, LRU和LFU初始尺寸均为5)的缓存
    // 并且transform_time为3, 即输入3次相同单词，会帮你加入last frequent used 表中
    DataType input; 
    while(true) {
        getline(cin, input);
        container.cache_insert(input);
        if (input == "exit") break;
        container.cache_show(true);  // 改为false时，不显示ghost表中的内容
    }
    return 0;
}

/* TestCode : 
ARCcache container(5, 3);
container.put("hello");
container.put("hella");
container.put("hellb");
container.put("world");
container.put("hellc");
container.put("hellk");
container.put("world");
container.put("world");
container.put("world");
container.put("hellb");
container.put("hellt");
container.Show(true);
container.put("hella");
container.Show(true);
*/
