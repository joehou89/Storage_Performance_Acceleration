## 需求来源  
在实际的项目中，也存在缓存的设计，比如像讯飞云对象存储新一代架构演进里，他们就准备在内存里搞一个，比较简单的能够想到的就是LRU(最近最久未使用)算法。  
它是说在一组数据中，根据数据的容量，根据数据的访问时间的先后，去掉时间最老的一个或一批数据。  
比如力扣算法题LCR 031. url:https://leetcode.cn/problems/OrIXps/description/  
  
## 核心数据结构  
1.用于快速索引的一张表，红黑树(map) 或者哈希表(unorderdmap),时间复杂度较低;  
2.双向链表，用于在内存中保存一份脏数据(如果刷盘的话)或数据；  
  
## 伪码分析  
首先要定义一个双向链表的结构体,实现成一个类比较友好  
```c  
/*双向链表*/
  class DlinkedNode 
  {
    public:
      int _key;
      int _value;
      DlinkedNode *_prev;
      DlinkedNode *_next;
      bool _virtual_node;

    public:
      DlinkedNode():_key(0), _value(0), _prev(nullptr), _next(nullptr), _virtual_node(true)
      {
        // std::cout << "DlinkedNode()" << std::endl;
      }
      DlinkedNode(int key, int value): _key(key), _value(value), _prev(nullptr), _next(nullptr), _virtual_node(false)
      {
        // std::cout << "DlinkedNode(), key: " << key << ", value: " << value << std::endl;
      }
  };
```
    
map或者哈希表的数据结构比较好定义，直接采用stl提供的数据即可  
比如:  
```c
这两个结构采用哪个都可以，hashmap更好一些
std::map<key, DlinkedNode *> table;  
std::unorderdmap<key, DlinkedNode *>hash_table;
```
  






