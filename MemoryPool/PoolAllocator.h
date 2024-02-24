//
// Created by Administrator on 2024-02-23.
//

#ifndef MEMORYPOOL_POOLALLOCATOR_H
#define MEMORYPOOL_POOLALLOCATOR_H
#include <mutex>

template<typename _Tp>
class myallocator
{
public:
    using value_type = _Tp;
    using size_type = size_t;
    using defference_type = ptrdiff_t;
#if __cplusplus <= 201703L
    // These were removed for C++20.
      typedef _Tp*       pointer;
      typedef const _Tp* const_pointer;
      typedef _Tp&       reference;
      typedef const _Tp& const_reference;

      template<typename _Tp1>
	struct rebind
	{ typedef allocator<_Tp1> other; };
#endif

    //�����ڴ�
    _Tp* allocate(size_t n){
        n = sizeof(value_type) * n;
        void* ret = nullptr;
        if(n > (size_t)MAX_BYTES){
            ret = malloc(n);
            if(ret == 0){
                exit(222);
            }
        }else{
             Obj* volatile* myFreeList = freeList + freelistIndex(n) ;  //�õ���Ӧ�����ɶ���
#ifndef _NOTHREADS
            std::lock_guard<std::recursive_mutex> lockGuard(mtx);
#endif
            Obj*  result = *myFreeList;
            if(result == 0){
                ret =  refill(roundUp(n));
            }else{
                *myFreeList = static_cast<Obj*>(result)->next;
                ret = result;
            }
        }
        return static_cast<_Tp*>(ret);
    }

    //�ͷ��ڴ�
    void deallocate(void* p, size_t n){
        if(n > static_cast<size_t>(MAX_BYTES)){
            free(p);
        }else{
            Obj* volatile* myFreeList = freelistIndex(n) + freeList; //�õ���Ӧ�����ɶ���
#ifndef _NOTHREADS
            std::lock_guard<std::recursive_mutex> lockGuard(mtx);
#endif
            Obj* result =  *myFreeList;
            static_cast<Obj*>(p)->next = result;
            *myFreeList = static_cast<Obj*>(p);
        }
    }
    //�ڴ�����&����
    void* reallocate(void* p, size_t oldSz, size_t newSz);
    //������
    void construct(_Tp* p, const _Tp& val){
        new (p) _Tp(val);
    }
    //��������
    void destory(_Tp* p){
        p->~T();
    }

    static void* refill(size_t __n);
    static char* chunkAlloc(size_t size, int& objs);

private:
    //���뵽8�ֽڵı���
    static size_t roundUp(size_t bytes){
        return (((bytes) + (size_t) ALIGN - 1) & ~((size_t) ALIGN - 1));
    }

    //��ȡ��ǰ��С���������������
    static size_t freelistIndex(size_t bytes) {
        return (((bytes) + (size_t)ALIGN - 1) / (size_t)ALIGN - 1);
    }

private:
    enum {ALIGN = 8};  //���������Ǵ�8�ֽڿ�ʼ�� ��8�ֽڶ��뷽ʽ��һֱ���䵽128
    enum {MAX_BYTES = 128};  //�ڴ������chunk��
    enum {NFREELISTS = 16};  //��������ĸ���
    union Obj{
        union Obj* next;
        char clientData[1];
    };
    //������������ ����ÿ��Ԫ�ض��Ǵ洢һ����������
     static Obj*  volatile freeList[NFREELISTS];
    static char* start_free;
    static char* end_free;
    static size_t heap_size;
    static std::recursive_mutex mtx;
};

template<typename T>
 typename myallocator<T>::Obj*  volatile myallocator<T>::freeList[NFREELISTS] = {
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
};

template<typename T>
char* myallocator<T>::start_free = nullptr;

template<typename T>
char* myallocator<T>::end_free = nullptr;

template<typename T>
size_t myallocator<T>::heap_size = 0;

template<typename T>
std::recursive_mutex myallocator<T>::mtx;

/**
 * ����objs��chunk�����ҷ���Ҫ�õ�chunk
 * @tparam T
 * @param n
 * @return
 */
template<typename T>
void *myallocator<T>::refill(size_t n) {
    int objs = 20;
    char *chunk = chunkAlloc(n, objs);
    Obj *volatile *myFreeList;
    Obj *result;
    Obj *currentObj;
    Obj *nextObj;
    if (1 == objs) {
        return chunk;
    }
    myFreeList = freeList + freelistIndex(n);
    //build free list in chunk
    result = reinterpret_cast<Obj *>(chunk);
    *myFreeList = nextObj = reinterpret_cast<Obj *>(chunk + n);
    for (int i = 1;; i++) {
        currentObj = nextObj;
        if (objs - 1 == i) {
            currentObj->next = nullptr;
            break;
        } else {
            nextObj = reinterpret_cast<Obj *>(reinterpret_cast<char *>(nextObj) + n);
            currentObj->next = nextObj;
        }
    }
    return result;
}

template<typename T>
char *myallocator<T>::chunkAlloc(size_t size, int &objs) {
    char *result;
    size_t totalBytes = size * objs;
    size_t bytesLeft = end_free - start_free;
    if (bytesLeft >= totalBytes) {
        //����ʣ��ռ����Ҫ����Ĵ�С ʹ�û���Ŀռ�
        result = start_free;
        start_free = start_free + totalBytes;
        return result;
    } else if (bytesLeft >= size) {
        //����ʣ��Ŀռ����size
        objs = static_cast<int>(bytesLeft /size);
        totalBytes = size * objs;
        result = start_free;
        start_free = start_free + totalBytes;
        return result;
    } else {
        size_t bytesToGet = 2 * totalBytes + roundUp(heap_size >> 4);
        if(bytesLeft > 0 ){
            Obj *volatile *myFreeList = freeList + freelistIndex(bytesLeft);
            reinterpret_cast<Obj *>(start_free)->next = *myFreeList;
            *myFreeList = reinterpret_cast<Obj *>(start_free);
        }
        start_free = static_cast<char*>(malloc(bytesToGet));
        if(0 == start_free){
            //�����ڴ�ʧ�� ֱ���˳����򣬿����и��õİ취
            exit(1);
        }
        heap_size += bytesToGet;
        end_free = start_free + bytesToGet;
        return (chunkAlloc(size,objs));
    }

}
#endif //MEMORYPOOL_POOLALLOCATOR_H
