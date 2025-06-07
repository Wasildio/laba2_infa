

#pragma once
#include <stdexcept>
#include <string>
#include <utility>

template<typename T>
class LinkedList {
    struct Node { T val; Node* next; explicit Node(const T& v): val(v), next(nullptr){} };

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    size_t len_ = 0;

    void range_check(size_t i) const {
        if (i >= len_)
            throw std::out_of_range("IndexOutOfRange: index=" + std::to_string(i) +
                                    " length=" + std::to_string(len_));
    }

public:
    /* --- ctors/dtor --- */
    LinkedList() = default;
    LinkedList(const T* src,size_t n){ for(size_t i=0;i<n;++i) Append(src[i]); }

    LinkedList(const LinkedList& o){ for(const auto& v:o) Append(v); }
    LinkedList& operator=(LinkedList rhs){ swap(rhs); return *this; }

    LinkedList(LinkedList&&) noexcept = default;
    LinkedList& operator=(LinkedList&&) noexcept = default;

    ~LinkedList(){ clear(); }

    /* --- read --- */
    size_t   GetLength() const { return len_; }
    const T& GetFirst()  const {
        if(!head_) throw std::out_of_range("GetFirst: empty list");
        return head_->val;
    }
    const T& GetLast()   const {
        if(!tail_) throw std::out_of_range("GetLast: empty list");
        return tail_->val;
    }
    const T& Get(size_t idx) const {
        range_check(idx);
        Node* p = head_; while(idx--) p = p->next;
        return p->val;
    }

    /* --- modify --- */
    void Append(const T& v){
        Node* n = new Node(v);
        if(!head_) head_ = tail_ = n;
        else tail_ = tail_->next = n;
        ++len_;
    }
    void Prepend(const T& v){
        Node* n = new Node(v);
        n->next = head_; head_ = n;
        if(!tail_) tail_ = head_;
        ++len_;
    }
    void InsertAt(const T& v,size_t i){
        if(i>len_) throw std::out_of_range("InsertAt: idx="+std::to_string(i));
        if(i==0){ Prepend(v); return; }
        if(i==len_){ Append(v); return; }
        Node* prev=head_;
        for(size_t k=1;k<i;++k) prev=prev->next;
        Node* cur=new Node(v); cur->next=prev->next; prev->next=cur; ++len_;
    }

    LinkedList* GetSubList(size_t l,size_t r) const{
        if(l>r||r>=len_) throw std::out_of_range("GetSubList: bad range");
        auto* res = new LinkedList;
        for(size_t i=l;i<=r;++i) res->Append(Get(i));
        return res;
    }
    LinkedList* Concat(const LinkedList* o) const{
        auto* res = new LinkedList(*this);
        for(const auto& v:*o) res->Append(v);
        return res;
    }

    /* --- simple iterators --- */
    class it{
        Node* p;
    public:
        explicit it(Node* n):p(n){}
        it& operator++(){ p=p->next; return *this; }
        bool operator!=(const it& o)const{ return p!=o.p; }
        T& operator*() const { return p->val; }
    };
    class cit{
        const Node* p;
    public:
        explicit cit(const Node* n):p(n){}
        cit& operator++(){ p=p->next; return *this; }
        bool operator!=(const cit& o)const{ return p!=o.p; }
        const T& operator*() const { return p->val; }
    };

    it  begin(){ return it(head_); }  it  end(){ return it(nullptr); }
    cit begin() const { return cit(head_); }  cit end() const { return cit(nullptr); }

private:
    void clear(){ while(head_){ Node* n=head_; head_=head_->next; delete n; } tail_=nullptr; len_=0; }
    void swap(LinkedList& o){ std::swap(head_,o.head_); std::swap(tail_,o.tail_); std::swap(len_,o.len_); }
};
