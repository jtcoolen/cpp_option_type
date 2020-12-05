#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <ostream>
#include <regex>


namespace lib {

    template<class T>
    class optional {
    private:
        T *t;

        explicit optional(T *t) : t{t} {}

        const static optional<T> &none;
    public:
        virtual ~optional();

        optional(const optional<T> &o) : t{o.t} {}

        optional<T> &operator=(optional<T> &&o) noexcept;

        explicit operator bool() const;

        static optional<T> of(const T &t);

        static optional<T> ofNullable(T *t);

        static const optional<T> &empty();

        bool isEmpty() const;

        bool isPresent() const;

        T orElseThrow() const;

        T orElse(T &other) const;

        template<class U>
        optional<U> map(std::function<U* (T)> f);
        template<class U>
        optional<U> map(U* (*f)(T));


        optional<T> filter(std::function<bool (T)> predicate);
        optional<T> filter(bool (*predicate)(T));
    };

    template<class T>
    template<class U>
    optional<U> optional<T>::map(std::function<U* (T)> f) {
        if (isEmpty()) { return optional<U>::empty(); }
        U *u;
        try {
            u = f(this->orElseThrow());
        } catch (std::runtime_error &error) {
            return optional<U>::empty();
        }
        return optional<U>::ofNullable(u);
    }

    template<class T>
    template<class U>
    optional<U> optional<T>::map(U* (*f)(T)) {
        if (isEmpty()) { return optional<U>::empty(); }
        U *u;
        try {
            u = (*f)(this->orElseThrow());
        } catch (std::runtime_error &error) {
            return optional<U>::empty();
        }
        return optional<U>::ofNullable(u);
    }

    template<class T>
    optional<T>
    optional<T>::filter(std::function<bool (T)> predicate) {
        T tval;
        try {
            tval = this->orElseThrow();
        } catch (std::runtime_error &error) {
            return optional<T>::empty();
        }
        if (predicate(tval)) {
            return *this;
        }
        return none;
    }

    template<class T>
    optional<T>
    optional<T>::filter(bool(*predicate)(T)) {
        T tval;
        try {
            tval = this->orElseThrow();
        } catch (std::runtime_error &error) {
            return optional<T>::empty();
        }
        if ((*predicate)(tval)) {
            return *this;
        }
        return none;
    }

    template<class T>
    optional<T>::operator bool() const {
        return !empty();
    }

    template<class T>
    optional<T> &optional<T>::operator=(optional<T> &&o) noexcept {
        if (this != &o) {
            if (o.isEmpty()) {
                delete this->t;
                this->t = o.t;
                return *this;
            }
            delete this->t;
            this->t = o.t;
            o.t = nullptr;
        }
        return *this;
    }

    template<class T>
    T optional<T>::orElse(T &other) const {
        if (isEmpty()) {
            return other;
        }
        return *t;
    }

    template<class T>
    T optional<T>::orElseThrow() const {
        if (isEmpty()) {
            throw std::runtime_error("Tried getting value from None type");
        }
        return *t;
    }

    template<class T>
    bool optional<T>::isEmpty() const {
        return t == nullptr;
    }

    template<class T>
    optional<T>::~optional() {
        //if (t != nullptr) delete t;
    }

    template<class T>
    optional<T> optional<T>::of(const T &t) {
        return optional<T>(new T(t));
    }

    template<class T>
    const optional<T> &optional<T>::empty() {
        return none;
    }

    template<class T>
    optional<T> optional<T>::ofNullable(T *t) {
        if (t == nullptr) return none;
        return of(*t);
    }

    template<class T>
    bool optional<T>::isPresent() const {
        return !isEmpty();
    }

    template<class T>
    const optional<T> &optional<T>::none = optional<T>(nullptr);

}


#endif
