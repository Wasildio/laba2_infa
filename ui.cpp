

#include "DynamicArray.hpp"
#include "LinkedList.hpp"
#include "MutableArraySequence.hpp"
#include "MutableListSequence.hpp"
#include "ImmutableArraySequence.hpp"
#include "ImmutableListSequence.hpp"
#include "algorithms.hpp"

#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>

/* ---------- helpers ---------- */
template<class Seq>
void printSeq(const Seq& s){
    for (size_t i=0;i<s.GetLength();++i) std::cout << s.Get(i) << " ";
}

/* ---------- DynamicArray UI ---------- */
void dynamicArrayUI(){
    DynamicArray<int> arr;
    while(true){
        std::cout<<"\n-- DynamicArray<int> --\n"
                 <<"1) PushBack  2) Resize  3) Set  4) Get  5) Print  0) Back\n>";
        int c; std::cin>>c; if(!std::cin||!c) break;
        if(c==1){int x;std::cin>>x;arr.PushBack(x);}
        else if(c==2){int n;std::cin>>n;arr.Resize(n);}
        else if(c==3){int i,x;std::cin>>i>>x;arr[i]=x;}
        else if(c==4){int i;std::cin>>i;std::cout<<arr[i]<<"\n";}
        else if(c==5){for(auto& v:arr)std::cout<<v<<" ";std::cout<<"\n";}
    }
}

/* ---------- LinkedList UI ---------- */
void linkedListUI(){
    LinkedList<int> lst;
    while(true){
        std::cout<<"\n-- LinkedList<int> --\n"
                 <<"1) Append 2) Prepend 3) InsertAt 4) Print 0) Back\n>";
        int c;std::cin>>c;if(!std::cin||!c)break;
        if(c==1){int x;std::cin>>x;lst.Append(x);}
        else if(c==2){int x;std::cin>>x;lst.Prepend(x);}
        else if(c==3){int x,i;std::cin>>x>>i;lst.InsertAt(x,i);}
        else if(c==4){for(auto v:lst)std::cout<<v<<" ";std::cout<<"\n";}
    }
}

/* ---------- Sequence generic helpers ---------- */
template<class Seq>
void seqAppendUI(Seq& s){
    int x;std::cin>>x; s.Append(x);
}
template<class Seq>
void seqPrintUI(const Seq& s){
    printSeq(s); std::cout<<"\n";
}

/* ---------- MutableArraySequence UI ---------- */
void mutArrSeqUI(){
    MutableArraySequence<int> seq;
    while(true){
        std::cout<<"\n-- MutableArraySequence<int> --\n"
                 <<"1) Append 2) Prepend 3) InsertAt 4) Print 0) Back\n>";
        int c;std::cin>>c;if(!std::cin||!c)break;
        if(c==1) seqAppendUI(seq);
        else if(c==2){int x;std::cin>>x;seq.Prepend(x);}
        else if(c==3){int x,i;std::cin>>x>>i;seq.InsertAt(x,i);}
        else if(c==4) seqPrintUI(seq);
    }
}

/* ---------- MutableListSequence UI ---------- */
void mutListSeqUI(){
    MutableListSequence<int> seq;
    while(true){
        std::cout<<"\n-- MutableListSequence<int> --\n"
                 <<"1) Append 2) Prepend 3) InsertAt 4) Print 0) Back\n>";
        int c;std::cin>>c;if(!std::cin||!c)break;
        if(c==1) seqAppendUI(seq);
        else if(c==2){int x;std::cin>>x;seq.Prepend(x);}
        else if(c==3){int x,i;std::cin>>x>>i;seq.InsertAt(x,i);}
        else if(c==4) seqPrintUI(seq);
    }
}

/* ---------- Algorithms demo ---------- */
void algorithmsUI(){
    std::cout<<"n? ";int n;std::cin>>n;
    std::vector<int> v(n); for(int& x:v) std::cin>>x;
    MutableArraySequence<int> s(v.data(),v.size());

    auto doubled = s.Map<int>([](int x){return x*2;});
    std::cout<<"Map x*2: "; printSeq(*doubled); std::cout<<"\n";

    auto flats = s.FlatMap<int>([](int x){
        auto p = SeqUPtr<int>(new MutableArraySequence<int>);
        p->Append(x); p->Append(x+1); return p;
    });
    std::cout<<"FlatMap {x,x+1}: "; printSeq(*flats); std::cout<<"\n";

    int sum = s.Reduce(0,[](int a,int b){return a+b;});
    std::cout<<"Reduce(sum)="<<sum<<"\n";
}

/* ---------- Self‑tests (quick assertions) ---------- */
void selfTestsUI(){
    std::cout << "\n-- Running built‑in self‑tests --\n";

    // DynamicArray basic
    {
        DynamicArray<int> a;
        a.PushBack(1); a.PushBack(2); a.Resize(3);
        assert(a.GetSize() == 3 && a[0] == 1 && a[1] == 2);
    }

    // MutableArraySequence append / prepend / insert
    {
        MutableArraySequence<int> s;
        s.Append(2); s.Prepend(1); s.InsertAt(3,2);            // 1 2 3
        assert(s.GetLength()==3 && s.Get(1)==2 && s.GetLast()==3);
    }

    // ImmutableArraySequence immutability
    {
        int raw[] = {5,6,7};
        const ImmutableArraySequence<int> im(raw,3);   // make object const to select const‑qualified Append
        auto im2 = im.Append(8);
        assert(im.GetLength()==3 && im2->GetLength()==4);
        assert(im2->GetLast()==8 && im.GetLast()==7);
    }

    // Algorithms zip/unzip
    {
        auto a = From<int>({1,2,3});
        auto b = From<int>({4,5,6});
        auto z = Zip(*a,*b);
        auto [x,y] = Unzip<int,int>(*z);
        assert(z->GetLength()==3 && x->Get(1)==2 && y->Get(1)==5);
    }

    std::cout << "All embedded tests passed!\n";
}

/* ---------- Benchmark ---------- */
void benchmarkUI(){
    int N; std::cout<<"N ops? "; std::cin>>N;
    std::cout<<"Benchmark target:\n"
             <<"1) DynamicArray PushBack\n"
             <<"2) MutableArraySequence Append\n"
             <<"3) MutableListSequence  Append\n"
             <<"> ";
    int opt; std::cin>>opt;

    switch(opt){
        case 1:{
            DynamicArray<int> da;
            auto t0 = std::chrono::high_resolution_clock::now();
            for(int i=0;i<N;++i) da.PushBack(i);
            auto t1 = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
            std::cout<<"DynamicArray.PushBack x"<<N<<" : "<<ms<<" ms\n";
            break;
        }
        case 2:{
            MutableArraySequence<int> seq;
            auto t0 = std::chrono::high_resolution_clock::now();
            for(int i=0;i<N;++i) seq.Append(i);
            auto t1 = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
            std::cout<<"MutableArraySequence.Append x"<<N<<" : "<<ms<<" ms\n";
            break;
        }
        case 3:{
            MutableListSequence<int> seq;
            auto t0 = std::chrono::high_resolution_clock::now();
            for(int i=0;i<N;++i) seq.Append(i);
            auto t1 = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
            std::cout<<"MutableListSequence.Append  x"<<N<<" : "<<ms<<" ms\n";
            break;
        }
        default:
            std::cout<<"Unknown option\n";
    }
}

/* ---------- Main ---------- */
int main(){
    while(true){
        std::cout<<"\n== Menu ==\n"
                 <<"1) DynamicArray  2) LinkedList\n"
                 <<"3) MutArraySeq   4) MutListSeq\n"
                 <<"5) Algorithms    6) Benchmark   7) Self‑tests   0) Exit\n>";
        int c;std::cin>>c;if(!std::cin||!c)break;
        switch(c){
            case 1: dynamicArrayUI(); break;
            case 2: linkedListUI();  break;
            case 3: mutArrSeqUI();   break;
            case 4: mutListSeqUI();  break;
            case 5: algorithmsUI();  break;
            case 6: benchmarkUI();   break;
            case 7: selfTestsUI();    break;
            default: std::cout<<"?\n";
        }
    }
    return 0;
}
