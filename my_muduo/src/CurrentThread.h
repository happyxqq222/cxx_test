//
// Created by Administrator on 2024-03-27.
//

#ifndef MY_MUDUO_CURRENTTHREAD_H
#define MY_MUDUO_CURRENTTHREAD_H

namespace CurrentThread
{
    extern __thread int t_cachedTid;

    extern thread_local int t_cachedTid2;

    void cacheTid();

    void cacheTid2();

    inline int tid(){
        if(__builtin_expect(t_cachedTid ==0,0)){
            cacheTid();
        }
        return t_cachedTid;
    }

    inline int tid2(){
        if(t_cachedTid2 == 0){
            cacheTid2();
        }
        return t_cachedTid2;
    }

}


#endif //MY_MUDUO_CURRENTTHREAD_H
