//
// Created by jco on 13/11/2020.
//

#ifndef TP9_MON_PTR_U_TCC
#define TP9_MON_PTR_U_TCC


template<class T>
Mon_ptr_u<T>::~Mon_ptr_u() {
    delete ptr;
}

template<class T>
T *Mon_ptr_u<T>::release() {
    T *t = ptr;
    ptr = nullptr;
    return t;
}


template<class T>
bool Mon_ptr_u<T>::operator==(const Mon_ptr_u &rhs) const {
    return ptr == rhs.ptr;
}

template<class T>
bool Mon_ptr_u<T>::operator!=(const Mon_ptr_u &rhs) const {
    return !(rhs == *this);
}

template<class T>
void Mon_ptr_u<T>::echange(Mon_ptr_u<T> &p) {
    const Mon_ptr_u<T> &t = p;
    p.ptr = this->ptr;
    this->ptr = t.ptr;
}

template<class T>
Mon_ptr_u<T>::operator bool() const {
    return ptr != nullptr;
}

template<class T>
T Mon_ptr_u<T>::operator*() const {
    return *ptr;
}

template<class T>
T *Mon_ptr_u<T>::operator->() const {
    return ptr;
}

template<class T>
Mon_ptr_u<T>::Mon_ptr_u(T *ptr):ptr(ptr) {}

template<class T>
Mon_ptr_u<T>& Mon_ptr_u<T>::operator=(Mon_ptr_u<T> &p) {
    if (p == *this) {
        return *this;
    }
    this->ptr = p.release();
    return *this;
}


#endif //TP9_MON_PTR_U_TCC