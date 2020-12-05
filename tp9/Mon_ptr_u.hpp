//
// Created by jco on 13/11/2020.
//

#ifndef TP9_MON_PTR_U_HPP
#define TP9_MON_PTR_U_HPP

template<class T>
class Mon_ptr_u {
private:
    T *ptr;
public:
    explicit Mon_ptr_u(T *ptr);

    explicit Mon_ptr_u(Mon_ptr_u<T> const &p) = delete;
    explicit Mon_ptr_u(Mon_ptr_u<T> &p) = delete;

    bool operator==(const Mon_ptr_u &rhs) const;

    bool operator!=(const Mon_ptr_u &rhs) const;

    virtual ~Mon_ptr_u();

    //Mon_ptr_u<T>& operator=(Mon_ptr_u<T> p);
    Mon_ptr_u<T>& operator=(Mon_ptr_u<T> &p);

    T *release();

    void echange(Mon_ptr_u<T> &p);

    explicit operator bool() const;

    T operator*() const;

    T *operator->() const;
};

#include "Mon_ptr_u.tcc"


#endif //TP9_MON_PTR_U_HPP
