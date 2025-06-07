
#pragma once
#include "Sequence.hpp"
#include "DynamicArray.hpp"
#include <stdexcept>
#include <string>

template<typename T>
class MutableArraySequence : public Sequence<T> {
    DynamicArray<T> data_;
public:
    /* ctors */
    MutableArraySequence() = default;
    MutableArraySequence(const T* p,size_t n): data_(p,n) {}
    MutableArraySequence(const MutableArraySequence&)            = default;
    MutableArraySequence& operator=(const MutableArraySequence&) = default;
    MutableArraySequence(MutableArraySequence&&) noexcept        = default;
    MutableArraySequence& operator=(MutableArraySequence&&) noexcept = default;

    /* read */
    size_t GetLength()               const override { return data_.GetSize(); }
    const T& Get(size_t i)           const override { return data_[i]; }
    T GetFirst()                     const override {
        if (!GetLength()) throw std::out_of_range("empty");
        return Get(0);
    }
    T GetLast()                      const override {
        if (!GetLength()) throw std::out_of_range("empty");
        return Get(GetLength()-1);
    }

    /* mutable */
    void Append (const T& v) override { data_.PushBack(v); }
    void Prepend(const T& v) override {
        data_.Reserve(GetLength()+1);
        data_.Resize(GetLength()+1);
        for (size_t i=GetLength()-1;i>0;--i) data_[i]=std::move(data_[i-1]);
        data_[0]=v;
    }
    void InsertAt(const T& v,size_t idx) override {
        if (idx>GetLength()) throw std::out_of_range("InsertAt: bad idx");
        data_.Reserve(GetLength()+1);
        data_.Resize(GetLength()+1);
        for (size_t i=GetLength()-1;i>idx;--i) data_[i]=std::move(data_[i-1]);
        data_[idx]=v;
    }
    Sequence<T>* Concat(Sequence<T>* other) override {
        for (size_t i=0;i<other->GetLength();++i) Append(other->Get(i));
        return this;
    }

    // immutable versions — create a copy then apply change
    SeqUPtr<T> Append(const T& v) const override {
        throw std::logic_error("MutableArraySequence: immutable operation not supported");
    }
    SeqUPtr<T> Prepend(const T& v) const override {
        throw std::logic_error("MutableArraySequence: immutable operation not supported");
    }
    SeqUPtr<T> InsertAt(const T& v, size_t idx) const override {
        throw std::logic_error("MutableArraySequence: immutable operation not supported");
    }
    SeqUPtr<T> Concat(const Sequence<T>* other) const override {
        throw std::logic_error("MutableArraySequence: immutable operation not supported");
    }

    /* service */
    SeqUPtr<T> GetSubsequence(size_t l,size_t r) const override {
        if (l>r || r>=GetLength()) throw std::out_of_range("subseq: bad range");
        auto res = SeqUPtr<T>(new MutableArraySequence);
        for (size_t i=l;i<=r;++i) res->Append(data_[i]);
        return res;
    }
    SeqUPtr<T> Clone() const override {
        return SeqUPtr<T>(new MutableArraySequence(*this));
    }
    Sequence<T>* Instance() override { return this; }

    auto begin()       { return data_.begin(); }
    auto end()         { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end()   const { return data_.end(); }
};
