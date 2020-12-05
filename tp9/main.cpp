#include<iostream>
#include "Mon_ptr_u.hpp"
#include "optional.hpp"

using namespace std;

class A {
public:
    int v;

    explicit A(int x) : v(x) {}
};

//template<class T>
//void f(Mon_ptr_u<T> x) {}

//template<class T>
//void g(Mon_ptr_u<T> &x) {}

int *f(int *x) {
    if (x == nullptr) {
        return nullptr;
    }
    *x = 2 * (*x);
    return x;
}

class C {
public:
    int x;
};

class D {
public:
    std::string s;
};

class H :
        public std::unary_function<C, D *> {
    D *operator()(C c) {
        if (c.x > 0) {
            return new D{"hello"};
        }
        return nullptr;
    }
};
std::function<auto(C) -> D* >goodLambda = [](C c) -> D * {
    if (c.x > 0) {
        return new D{"hello"};
    } else {
        return nullptr;
    }
};

static bool pred(C c) {
    return c.x == 1;
}

int main() {
    cout << "Tests de l'exo 1" << endl;
//   Tests 1
    Mon_ptr_u<A> p1{new A{1}};

//   Tests 2
//Mon_ptr_u<A> p1bis {p1}; //   Ne   d o i t   p a s   marcher
//f ( p1 ) ; //   Ne   d o i t   p a s   marcher
    //g(p1); //   E s t  OK
    Mon_ptr_u<A> p1bis{nullptr};
    //p1bis  =  p1 ; //   Ne   d o i t   p a s   marcher

//   Tests 3
    cout << "−−−−−−−−−" << endl;
    cout << (p1.release())->v << endl;
// c o u t<<( p1 . r e l e a s e ( ) )−>v<<e n d l ; //   ne   marche   p l u s
    cout << p1.release() << endl; //   a f f i c h e    l e   0   c o r r e s p o n d a n t   `an u l l p t r

    //   Tests   4
    cout << "−−−−−−−−−" << endl;
    Mon_ptr_u<A> p2{new A{2}};
    p1 = p2;
    // c o u t<<( p2 . r e l e a s e ( ) )−>v<<e n d l ;//   ne   d o i t   p a s   marcher
    cout << p2.release() << endl;//   a f f i c h e    l e   0
    // c o r r e s p o n d a n t   `a   n u l l p t r
    cout << (p1.release())->v << endl;//   a f f i c h e   2
    cout << p1.release() << endl;//   a f f i c h e    l e   0
    // c o r r e s p o n d a n t   `a   n u l l p t r
    Mon_ptr_u<A> p3{new A{3}};
    p3 = p3;
    p3 = p3;
    cout << p3.release()->v << endl;//   a f f i c h e   3
    cout << p3.release() << endl;//   a f f i c h e    l e   0
    // c o r r e s p o n d a n t   `a   n u l l p t r
    p3 = p3;
    cout << p3.release() << endl;//   a f f i c h e    l e   0
    // c o r r e s p o n d a n t   `a   n u l l p t r
    p3 = p3;

    //   t e s t   5
    cout << "−−−−−−−−−" << endl;
    Mon_ptr_u<A> p4{new A{4}}, p5{new A{5}};
    p4.echange(p5);
    cout << p4.release()->v << endl;//   a f f i c h e   5
    cout << p5.release()->v << endl;//   a f f i c h e   4

    //   Tests   6
    cout << "−−−−−−−−−" << endl;
    Mon_ptr_u<A> p6{new A{6}};
    if (p6) cout << "p6 pointe vers la valeur" << p6.release()->v << endl;
    else cout << "p6 pointe vers nullptr " << endl;
    if (p6) cout << "p6 pointe vers la valeur" << p6.release()->v << endl;
    else cout << "p6 pointe vers nullptr " << endl;

    //   Tests   7
    cout << "−−−−−−−−−" << endl;
    Mon_ptr_u<A> p7{new A{7}};
    cout << "p7contient" << p7->v << endl;
    cout << "p7contient" << (*p7).v << endl;

    //   Tests   8
    cout << "−−−−−−−−−" << endl;
    A *a = new A{7};
    Mon_ptr_u<A> p8{a};
    // Mon_ptr_u<A>p9{a};   //   D o i t   l e v e r   une   e x c e p t i o n
    Mon_ptr_u<A> p9{new A{4}};
    p9 = p8;
    // Mon_ptr_u<A> p10{a};   //   D o i t   l e v e r   une   e x c e p t i o n
    p9.release();
    Mon_ptr_u<A> p10{a};


    cout << "Tests exo 2" << endl;

    int *x = new int{2};
    lib::optional<int>::ofNullable(f(x));
    int *y = nullptr;
    lib::optional<int>::ofNullable(f(y));
    C *c = new C{4};

    std::function<auto(C) -> bool> goodLambda2 =
            [](C c) -> bool {return c.x == 1;};

    lib::optional<C>::ofNullable(c).map(goodLambda);
    cout << lib::optional<C>::ofNullable(c).filter(goodLambda2).isEmpty() << endl;
    cout << lib::optional<C>::ofNullable(c).filter(pred).isEmpty() << endl;
    return 0;
}