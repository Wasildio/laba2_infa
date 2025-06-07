
#pragma once
#include "Sequence.hpp"
#include "LinkedList.hpp"
#include <stdexcept>
#include <string>

template<typename T>
class ImmutableListSequence : public Sequence<T> {
    LinkedList<T> list_;
public:
    ImmutableListSequence() = default;
    ImmutableListSequence(const T* p,size_t n): list_(p,n) {}
    ImmutableListSequence(const LinkedList<T>& lst): list_(lst) {}
    /* read */
    size_t GetLength()               const override { return list_.GetLength(); }
    const T& Get(size_t i)           const override { return list_.Get(i); }
    T GetFirst()                     const override { return list_.GetFirst(); }
    T GetLast()                      const override { return list_.GetLast();  }

    /* immutable ops */
    SeqUPtr<T> Append (const T& v) const override {
        auto cp = std::make_unique<ImmutableListSequence>(*this);
        cp->list_.Append(v); return cp;
    }
    SeqUPtr<T> Prepend(const T& v) const override {
        auto cp = std::make_unique<ImmutableListSequence>(*this);
        cp->list_.Prepend(v); return cp;
    }
    SeqUPtr<T> InsertAt(const T& v,size_t i) const override {
        auto cp = std::make_unique<ImmutableListSequence>(*this);
        cp->list_.InsertAt(v,i); return cp;
    }
    SeqUPtr<T> Concat(const Sequence<T>* o) const override {
        auto cp = std::make_unique<ImmutableListSequence>(*this);
        for (size_t i=0;i<o->GetLength();++i) cp->list_.Append(o->Get(i));
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
        return SeqUPtr<T>( new ImmutableListSequence(*list_.GetSubList(l,r)) );
    }
    SeqUPtr<T> Clone() const override { return std::make_unique<ImmutableListSequence>(*this); }
    Sequence<T>* Instance() override { return Clone().release(); }

    auto begin() const { return list_.begin(); }
    auto end()   const { return list_.end();   }
};
