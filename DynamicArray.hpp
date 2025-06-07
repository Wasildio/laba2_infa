
#pragma once
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <string>

template<typename T>
class DynamicArray {
    size_t size_     = 0;
    size_t capacity_ = 0;
    std::unique_ptr<T[]> data_;

    void check(size_t i) const {
        if (i >= size_)
            throw std::out_of_range("IndexOutOfRange: index=" + std::to_string(i) +
                                    " size="  + std::to_string(size_));
    }

public:
    /* --- ctors --- */
    DynamicArray() = default;
    explicit DynamicArray(size_t n)
        : size_(n), capacity_(n), data_(std::make_unique<T[]>(n)) {}

    DynamicArray(const T* src,size_t n) : DynamicArray(n) {
        std::copy(src, src+n, data_.get());
    }

    DynamicArray(const DynamicArray& o) : DynamicArray(o.size_) {
        std::copy(o.data_.get(), o.data_.get()+size_, data_.get());
    }

    DynamicArray& operator=(DynamicArray rhs){ swap(rhs); return *this; }

    DynamicArray(DynamicArray&&) noexcept = default;
    DynamicArray& operator=(DynamicArray&&) noexcept = default;

    /* --- access --- */
    size_t GetSize() const { return size_; }

    T&       operator[](size_t i){ check(i); return data_[i]; }
    const T& operator[](size_t i) const { check(i); return data_[i]; }

    /* --- iterators --- */
    T*       begin()       { return data_.get(); }
    T*       end()         { return data_.get()+size_; }
    const T* begin() const { return data_.get(); }
    const T* end()   const { return data_.get()+size_; }

    /* --- capacity helpers --- */
    void Reserve(size_t newCap){
        if(newCap<=capacity_) return;
        auto tmp = std::make_unique<T[]>(newCap);
        std::move(data_.get(), data_.get()+size_, tmp.get());
        data_.swap(tmp); capacity_ = newCap;
    }

    /* --- resize / push --- */
    void Resize(size_t n){
        Reserve(n);
        if(n > size_) std::fill(data_.get()+size_, data_.get()+n, T{});
        size_ = n;
    }
    void PushBack(const T& v){
        if(size_==capacity_) Reserve(capacity_?capacity_*2:1);
        data_[size_++] = v;
    }

    /* --- util --- */
    void swap(DynamicArray& o) noexcept {
        std::swap(size_, o.size_);
        std::swap(capacity_, o.capacity_);
        data_.swap(o.data_);
    }
};
