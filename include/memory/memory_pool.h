#pragma once
#include <vector>
template<typename T>
class MemoryPool {

private:
    struct Node 
    {
        T data;
        Node* next;
    };

    std::vector<Node> pool;
    Node* freeList;

public:
    MemoryPool(size_t size) : pool(size), freeList(nullptr) 
    {

        for(size_t i = 0; i < size - 1; ++i) 
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
        Node* node = reinterpret_cast<Node*>(obj);
        node->next = freeList;
        freeList = node;
    }
};