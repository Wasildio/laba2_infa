#pragma once
#include "MutableArraySequence.hpp"
#include <algorithm>
#include <initializer_list>
#include <utility>
#include <string>

/* ---------- zip / unzip ---------- */
template<typename A, typename B>
SeqUPtr< std::pair<A,B> >
Zip(const Sequence<A>& left, const Sequence<B>& right)
{
    using P = std::pair<A,B>;
    auto res = SeqUPtr<P>(new MutableArraySequence<P>);
    size_t n = std::min(left.GetLength(), right.GetLength());
    for (size_t i=0;i<n;++i) res->Append({ left.Get(i), right.Get(i) });
    return res;
}

template<typename A, typename B>
std::pair< SeqUPtr<A>, SeqUPtr<B> >
Unzip(const Sequence< std::pair<A,B> >& src)
{
    auto l = SeqUPtr<A>(new MutableArraySequence<A>);
    auto r = SeqUPtr<B>(new MutableArraySequence<B>);
    for (size_t i=0;i<src.GetLength();++i) {
        l->Append(src.Get(i).first);
        r->Append(src.Get(i).second);
    }
    return { std::move(l), std::move(r) };
}

/* ---------- split ---------- */
template<typename T, typename Pred>
SeqUPtr< SeqUPtr<T> >
Split(const Sequence<T>& src, Pred delim)
{
    using Sub = SeqUPtr<T>;
    auto res = SeqUPtr<Sub>(new MutableArraySequence<Sub>);
    auto cur = SeqUPtr<T>(new MutableArraySequence<T>);
    for (size_t i=0;i<src.GetLength();++i) {
        const T& v = src.Get(i);
        if (delim(v)) {
            if (cur->GetLength()) res->Append(std::move(cur));
            cur = SeqUPtr<T>(new MutableArraySequence<T>);
        } else cur->Append(v);
    }
    if (cur->GetLength()) res->Append(std::move(cur));
    return res;
}

/* ---------- slice ---------- */
template<typename T>
SeqUPtr<T> Slice(const Sequence<T>& src,int start,size_t cnt,
                 const Sequence<T>* repl=nullptr)
{
    int n = static_cast<int>(src.GetLength());
    if (start < 0) start += n;
    if (start < 0 || start > n)
        throw std::out_of_range("Slice: start out of range");
    if (start + static_cast<int>(cnt) > n)
        throw std::out_of_range("Slice: start+cnt overflow");

    auto res = SeqUPtr<T>(new MutableArraySequence<T>);
    for (int i=0;i<start;++i) res->Append(src.Get(i));
    if (repl) for (size_t i=0;i<repl->GetLength();++i) res->Append(repl->Get(i));
    for (int i=start+static_cast<int>(cnt); i<n; ++i) res->Append(src.Get(i));
    return res;
}

/* ---------- from / fold / free-where / free-find ---------- */
template<typename T>
SeqUPtr<T> From(std::initializer_list<T> ilist)
{
    return SeqUPtr<T>(new MutableArraySequence<T>(ilist.begin(), ilist.size()));
}

template<typename T, typename U, typename F>
U Fold(const Sequence<T>& seq, U init, F reduce)
{
    return seq.Reduce(init, reduce);
}

template<typename T, typename P>
SeqUPtr<T> Where(const Sequence<T>& seq, P p) { return seq.Where(p); }

template<typename T, typename P>
T Find(const Sequence<T>& seq, P p) { return seq.Find(p); }
