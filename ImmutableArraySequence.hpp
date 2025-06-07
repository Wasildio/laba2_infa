
#pragma once
#include "Sequence.hpp"
#include "DynamicArray.hpp"
#include <stdexcept>
#include <string>

template<typename T>
class ImmutableArraySequence : public Sequence<T> {
    DynamicArray<T> data_;
public:
    ImmutableArraySequence() = default;
    ImmutableArraySequence(const T* p,size_t n): data_(p,n) {}

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

    /* immutable ops (реально меняют копию) */
    SeqUPtr<T> Append (const T& v) const override {
        auto cp = std::make_unique<ImmutableArraySequence>(*this);
        cp->data_.PushBack(v);
        return cp;
    }
    SeqUPtr<T> Prepend(const T& v) const override {
        auto cp = std::make_unique<ImmutableArraySequence>(*this);
        cp->data_.Resize(GetLength()+1);
        for (size_t i=GetLength(); i>0; --i) cp->data_[i]=std::move(cp->data_[i-1]);
        cp->data_[0]=v;
        return cp;
    }
    SeqUPtr<T> InsertAt(const T& v,size_t i) const override {
        if (i>GetLength()) throw std::out_of_range("InsertAt: bad idx");
        auto cp = std::make_unique<ImmutableArraySequence>(*this);
        cp->data_.Resize(GetLength()+1);
        for (size_t k=GetLength(); k>i; --k) cp->data_[k]=std::move(cp->data_[k-1]);
        cp->data_[i]=v;
        return cp;
    }
    SeqUPtr<T> Concat(const Sequence<T>* o) const override {
        auto cp = std::make_unique<ImmutableArraySequence>(*this);
        for (size_t i=0;i<o->GetLength();++i) cp->data_.PushBack(o->Get(i));
        return cp;
    }

    /* mutable ops — запрещены */
    void Append (const T&) override { throw std::logic_error("immutable"); }
    void Prepend(const T&) override { throw std::logic_error("immutable"); }
    void InsertAt(const T&,size_t) override { throw std::logic_error("immutable"); }
    Sequence<T>* Concat(Sequence<T>*) override { throw std::logic_error("immutable"); }

    /* service */
    SeqUPtr<T> GetSubsequence(size_t l,size_t r) const override {
        if (l>r || r>=GetLength()) throw std::out_of_range("subseq: bad range");
        auto res = std::make_unique<ImmutableArraySequence>();
        for (size_t i=l;i<=r;++i) res->data_.PushBack(data_[i]);
        return res;
    }
    SeqUPtr<T> Clone() const override { return std::make_unique<ImmutableArraySequence>(*this); }
    Sequence<T>* Instance() override  { return Clone().release(); }

    auto begin() const { return data_.begin(); }
    auto end()   const { return data_.end();   }
};
