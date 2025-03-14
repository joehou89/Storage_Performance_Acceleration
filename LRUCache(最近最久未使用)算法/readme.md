## 需求来源  
在实际的项目中，也存在缓存的设计，比如像讯飞云对象存储新一代架构演进里，他们就准备在内存里搞一个，比较简单的能够想到的就是LRU(最近最久未使用)算法。  
它是说在一组数据中，根据数据的容量，根据数据的访问时间的先后，去掉时间最老的一个或一批数据。  
比如力扣算法题LCR 031. url:https://leetcode.cn/problems/OrIXps/description/  
  
## 原理  
我们以词汇的缓存为例分别说明LRU, LFU和ARC算法。首先，LRU算法是一种仅存储最近使用过词语的一种存储结构，其原理是利用链表的有序性，在使用词汇时，如果使用的词汇在缓存中，则对最近使用的词汇进行前置，而每一次缓存区满时，我们会删除最长时间没有使用过的节点(如下图中，python被删除)，并将新词语插入到链表的头部。  
原文链接：https://blog.csdn.net/sbsbsb666666/article/details/130198264  
  
  
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
    
## 优劣分析  
LRU算法的一个不好的地方在于：容易把缓存里的热点数据给干掉， 导致缓存更容易失效，失去作用；  
比如一个小时内前后插入了5个元素：key1, key2, key3, key4, key5  
其中key1在前10min内的访问频率是10000次，但是在后面50min未访问，其他4个元素都是10次，那么key1就是最近最久未使用的数据，就要被干掉，但是却是热点数据。  
所以LRU算法的使用效果其实并不友好，需要根据具体的业务场景或模型。  
  
## 使用场景  
LRU适用于按照写入时间来淘汰的场景；  
不适用于热点数据的缓存；  
  




