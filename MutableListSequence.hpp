

#pragma once
#include "Sequence.hpp"
#include "LinkedList.hpp"
#include <stdexcept>
#include <string>

template<typename T>
class MutableListSequence : public Sequence<T> {
    LinkedList<T> list_;
public:
    /* ctors */
    MutableListSequence() = default;
    MutableListSequence(const T* p,size_t n): list_(p,n) {}
    MutableListSequence(const LinkedList<T>& lst): list_(lst) {}
    MutableListSequence(const MutableListSequence&)            = default;
    MutableListSequence& operator=(const MutableListSequence&) = default;
    MutableListSequence(MutableListSequence&&) noexcept        = default;
    MutableListSequence& operator=(MutableListSequence&&) noexcept = default;

    /* read */
    size_t GetLength()               const override { return list_.GetLength(); }
    const T& Get(size_t i)           const override { return list_.Get(i); }
    T GetFirst()                     const override { return list_.GetFirst(); }
    T GetLast()                      const override { return list_.GetLast();  }

    /* mutable */
    void Append (const T& v) override { list_.Append(v);   }
    void Prepend(const T& v) override { list_.Prepend(v);  }
    void InsertAt(const T& v,size_t i) override { list_.InsertAt(v,i); }
    Sequence<T>* Concat(Sequence<T>* o) override {
        for (size_t i=0;i<o->GetLength();++i) list_.Append(o->Get(i));
        return this;
    }

    /* immutable */
    SeqUPtr<T> Append (const T& v) const override {
        auto cp = Clone(); cp->Append(v); return cp;
    }
    SeqUPtr<T> Prepend(const T& v) const override {
        auto cp = Clone(); cp->Prepend(v); return cp;
    }
    SeqUPtr<T> InsertAt(const T& v,size_t i) const override {
        auto cp = Clone(); cp->InsertAt(v,i); return cp;
    }
    SeqUPtr<T> Concat(const Sequence<T>* o) const override {
        auto cp = Clone(); cp->Concat(const_cast<Sequence<T>*>(o)); return cp;
    }

    /* service */
    SeqUPtr<T> GetSubsequence(size_t l,size_t r) const override {
        if (l>r || r>=GetLength())
            throw std::out_of_range("subseq: bad range");
        return SeqUPtr<T>( new MutableListSequence(*list_.GetSubList(l,r)) );
    }
    SeqUPtr<T> Clone()   const override { return SeqUPtr<T>(new MutableListSequence(*this)); }
    Sequence<T>* Instance() override    { return this; }

    auto begin()       { return list_.begin(); }
    auto end()         { return list_.end();   }
    auto begin() const { return list_.begin(); }
    auto end()   const { return list_.end();   }
};
