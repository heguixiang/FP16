#ifndef _RT_MEM_PTR_H_
#define _RT_MEM_PTR_H_
#include<algorithm>        // for std::swap
#include<stdint.h>
#include "common/debug/log.h"

namespace lxzh
{
    template<typename T>
    class RtMemPtr
    {
    private:
        T *ptr_;
    public:
        RtMemPtr(T* ptr) :ptr_(ptr)
        {
        }
        RtMemPtr(uint32_t size) : ptr_(nullptr)
        {
            rtError_t ret = mallocHost((void**)&ptr_, size);
            if (ret != RT_ERROR_NONE)
            {
                DOMI_LOGE("rtMallocHost failed. ret = %d", ret);
                ptr_ = nullptr;
            }
        }
        RtMemPtr(const RtMemPtr<T>& mp)
        {
            ptr_ = mp.ptr_;
        }

        RtMemPtr<T>& operator=(RtMemPtr<T> const &mp)
        {
            if (this != &mp)
            {
                RtMemPtr<T>(mp).swap(*this);
            }
            return *this;
        }

        T& operator*() const
        {
            return *ptr_;
        }

        T* operator->() const
        {
            return ptr_;
        }

        ~RtMemPtr()
        {
            if (ptr_)
            {
                rtError_t ret = freeHost(ptr_);
                if (ret != RT_ERROR_NONE)
                {
                    DOMI_LOGE("rtFreeHost failed. ret = %d", ret);
                }
                ptr_ = nullptr;
            }
        }

        void set(T *p)
        {
            ptr_ = p;
        }

        T* get()
        {
            return ptr_;
        }

        bool hasData()
        {
            return (ptr_ != nullptr);
        }

    private:
        void swap(RtMemPtr<T> & other)  // never throws
        {
            std::swap(ptr_, other.ptr_);
        }
    };
}

#endif _RT_MEM_PTR_H_
