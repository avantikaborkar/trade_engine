#pragma once
#include <vector>
template<typename T>
class MemoryPool {

private:
    struct Node 
    {
        T data;         //datatype of the object we want to allocate
        Node* next;
    };

    std::vector<Node> pool;
    Node* freeList;

public:
    MemoryPool(size_t size) : pool(size), freeList(nullptr) 
    {
        for(size_t i = 0; i < size - 1; ++i)        //linking all nodes together to form the free list
        {
            pool[i].next = &pool[i + 1];
        }
        pool[size - 1].next = nullptr;
        freeList = &pool[0];
    }

    T* allocate() 
    {

        if(!freeList) return nullptr;

        Node* node = freeList;
        freeList = freeList->next;

        return &node->data;
    }

    void deallocate(T* obj) 
    {
        Node* node = reinterpret_cast<Node*>(obj);      //memory manipulation to get the node from the object pointer
        node->next = freeList;
        freeList = node;
    }
};