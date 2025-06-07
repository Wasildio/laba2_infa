//  Базовый абстрактный интерфейс для всех «последовательностей».
#pragma once
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

template<typename T> class Sequence;
template<typename T> class MutableArraySequence;

template<typename T>
using SeqUPtr = std::unique_ptr< Sequence<T> >;

template<typename T>
class Sequence {
public:
    virtual ~Sequence() = default;

    /* --- обязательный API --- */
    virtual size_t    GetLength() const                       = 0;
    virtual const T&  Get(size_t) const                       = 0;
    virtual T         GetFirst() const                        = 0;
    virtual T         GetLast()  const                        = 0;

    virtual SeqUPtr<T> GetSubsequence(size_t, size_t) const   = 0;
    virtual SeqUPtr<T> Clone() const                          = 0;

    /* immutable-операции */
    virtual SeqUPtr<T> Append (const T&)                const = 0;
    virtual SeqUPtr<T> Prepend(const T&)                const = 0;
    virtual SeqUPtr<T> InsertAt(const T&, size_t)       const = 0;
    virtual SeqUPtr<T> Concat(const Sequence<T>*)       const = 0;

    /* mutable-операции */
    virtual void Append (const T&)                            = 0;
    virtual void Prepend(const T&)                            = 0;
    virtual void InsertAt(const T&, size_t)                   = 0;
    virtual Sequence<T>* Concat(Sequence<T>*)                 = 0;

    /* фабрика (нужна flatMap и т.п.) */
    virtual Sequence<T>* Instance()                           = 0;

    /* --- util-алгоритмы --- */
    template<typename U, typename F>
    SeqUPtr<U> Map(F f) const {
        auto out = SeqUPtr<U>(new MutableArraySequence<U>());
        for (size_t i=0;i<GetLength();++i) out->Append(f(Get(i)));
        return out;
    }
    template<typename U, typename R>
    U Reduce(U init, R r) const {
        for (size_t i=0;i<GetLength();++i) init = r(init, Get(i));
        return init;
    }
    template<typename P>
    SeqUPtr<T> Where(P p) const {
        auto out = SeqUPtr<T>(new MutableArraySequence<T>());
        for (size_t i=0;i<GetLength();++i) if (p(Get(i))) out->Append(Get(i));
        return out;
    }

    /* FlatMap */
    template<typename U, typename F>
    SeqUPtr<U> FlatMap(F f) const {
        auto out = SeqUPtr<U>(new MutableArraySequence<U>());
        for (size_t i=0;i<GetLength();++i) {
            auto sub = f(Get(i));
            for (size_t j=0;j<sub->GetLength();++j) out->Append(sub->Get(j));
        }
        return out;
    }

    /* Try-семантика */
    template<typename P>
    std::optional<T> TryFirst(P p) const {
        for (size_t i=0;i<GetLength();++i) if (p(Get(i))) return Get(i);
        return std::nullopt;
    }
    template<typename P>
    T Find(P p) const {
        if (auto opt = TryFirst(p)) return *opt;
        throw std::out_of_range("Find: no match");
    }

    /* итератор-обёртка */
    class Iterator {
        const Sequence<T>* seq_;
        size_t idx_;
    public:
        using iterator_category = std::forward_iterator_tag;
        Iterator(const Sequence<T>* s,size_t i): seq_(s), idx_(i) {}
        Iterator& operator++(){ ++idx_; return *this; }
        bool operator!=(const Iterator& o) const { return idx_ != o.idx_; }
        const T& operator*() const { return seq_->Get(idx_); }
    };
    Iterator begin() const { return Iterator(this,0); }
    Iterator end()   const { return Iterator(this,GetLength()); }

    /* удобный [] */
    const T& operator[](size_t i) const { return Get(i); }
};
