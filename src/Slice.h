#ifndef SLICE_H
#define SLICE_H

//#include <atomic>
//#include <stddef.h>
//#include <stddef.h>
#include "Option.h"
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

    Option<std::size_t> find( T ch)
    {
        for( std::size_t index = 0; index < this->len; index++)
        {
            if( *( this->addr + index) == ch)
                return Some(index);
        }
        return None();
    }

    std::size_t find_count( T ch)
    {
        std::size_t count = 0;
        for( std::size_t index = 0; index < this->len; index++)
        {
            if( *( this->addr + index) == ch)
                count++;
        }
        return count;
    }

    Option<std::size_t> find( Slice<T> ch)
    {
        std::size_t j = 0;
        for( std::size_t i = 0; i < this->len - ch.len; i++)
        {
            for(  j = 0; i < ch.len; j++)
            {
                if( *( this->addr + i + j) != *( ch.addr + j))
                    break;
            }

            if( j == ch.len)
                return Some(i);


        }
        return None();
    }

    Option<Slice> slice(size_t start, size_t end)
    {
        if ( start >= this->len)
            return None();

        if ( end >= this->len)
            return None();

        if ( end <= start)
            return None();

        return Some(Slice(this->addr + start, end - start));
    }

    Option<Slice> slice(size_t pos )
    {
        if ( pos >= this->len)
            return None();

        return Some(Slice(this->addr + pos, this->len - pos));
    }

    std::string to_string()
    {
        return std::string((char*)this->addr, this->len);
    }
};



#endif