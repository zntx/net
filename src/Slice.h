#ifndef SLICE_H
#define SLICE_H

//#include <atomic>
//#include <stddef.h>
//#include <stddef.h>

template <typename T>
class Slice {
public:
    T* addr;
    std::size_t len;

    Slice(T* address, std::size_t size)
    {
        addr = address;
        len = size;
    }

    Slice(const Slice<T> &slice)
    {
        //printf("Slice1 拷贝构造\n");
        this->addr = slice.addr;
        this->len  = slice.len;
    }
    // 移动构造
    Slice(Slice<T> &&slice)
    {
        //printf("Slice1 移动构造\n");
        this->addr = slice.addr;
        this->len  = slice.len;
    }

    // 移动赋值运算符
    // 和复制赋值运算符的区别在于，其参数是右值引用
    Slice& operator= (Slice<T> &&slice)
    {
        //printf("Slice1 移动赋值运算符\n");
        this->addr = slice.addr;
        this->len  = slice.len;

        return *this;
    }

    T operator[](std::size_t i)
    {
        if( i > this->len)
            throw "out of array";
        return this->addr[i];
    }

    /*   */
    T at(size_t index)
    {
        return *(this->addr + index);
    }

    bool set(size_t index, T ch)
    {
        if( index <= this->len )
        {
            *( this->addr + index) = ch;
            return true;
        }

        return false;
    }
    
    Slice slice(size_t pso, size_t size)
    {
        return Slice(this->address + pso, size);
    }

    Slice slice(size_t pso )
    {
        return Slice(this->address + pso, this->len - pso);
    }
};



#endif