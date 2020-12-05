#include <ostream>
#include <functional>
#include <vector>
#include <algorithm>

namespace lib {

    template<class T>
    class optional {
    private:
        T *t;

        static std::vector<T *> owned_ptrs;

        static bool isOwned(T *t);

        static void addToOwned(T *t);

        static void removeFromOwned(T *t);

        T *release();

        explicit optional(T *t);

        explicit optional();

        const static optional<T> &none;

    public:
        ~optional();

        optional(optional<T> &other); // copy constructor = delete;

        optional(optional<T> &&other) noexcept; // move constructor

        optional<T> &operator=(optional<T> &other) = delete; // delete copy assignment

        optional<T> &operator=(optional<T> &&other) noexcept; // move assignment

        explicit operator bool() const;

        static optional<T> of(T &t);

        static optional<T> ofNullable(T *t);

        static const optional<T> &empty();

        bool isEmpty() const;

        bool isPresent() const;

        T orElseThrow() const;

        T orElse(T &other) const;

        const T &operator*();

        const T *operator->();

        template<class U>
        optional<U> map(std::function<U *(T)> f);

        template<class U>
        optional<U> map(U *(*f)(T));

        optional<T> filter(std::function<bool(T)> predicate);

        optional<T> filter(bool (*predicate)(T));
    };

    template<class T>
    template<class U>
    optional<U> optional<T>::map(std::function<U *(T)> f) {
        return optional<U>::ofNullable(f(*t));
    }

    template<class T>
    template<class U>
    optional<U> optional<T>::map(U *(*f)(T)) {
        return optional<U>::ofNullable((*f)(*t));
    }

    template<class T>
    optional<T>
    optional<T>::filter(std::function<bool(T)> predicate) {
        if (isEmpty()) {
            return optional<T>();
        }
        if (predicate(*t)) {
            return *this;
        }
        return optional<T>();
    }

    template<class T>
    optional<T>
    optional<T>::filter(bool(*predicate)(T)) {
        if (isEmpty()) {
            return optional<T>();
        }
        if ((*predicate)(*t)) {
            return *this;
        }
        return optional<T>();
    }

    template<class T>
    optional<T>::operator bool() const {
        return isPresent();
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
            throw std::runtime_error("Cannot get value of None type");
        }
        return *t;
    }

    template<class T>
    bool optional<T>::isEmpty() const {
        return t == nullptr;
    }

    template<class T>
    optional<T>::~optional() {
        if (t != nullptr) {
            removeFromOwned(t);
            delete t;
        }
    }

    template<class T>
    T *optional<T>::release() {
        T *tmp = t;
        t = nullptr;
        if (tmp != nullptr) {
            removeFromOwned(tmp);
        }
        return tmp;
    }

    template<class T>
    optional<T> optional<T>::of(T &t) {
        return optional<T>(new T(t));
    }

    template<class T>
    const optional<T> &optional<T>::empty() {
        return none;
    }

    template<class T>
    optional<T> optional<T>::ofNullable(T *t) {
        if (t == nullptr) {
            return optional<T>();
        }
        return optional<T>(t);
    }

    template<class T>
    bool optional<T>::isPresent() const {
        return !isEmpty();
    }

    template<class T>
    const T &optional<T>::operator*() {
        return *t;
    }

    template<class T>
    const T *optional<T>::operator->() {
        return t;
    }

    template<class T>
    optional<T>::optional(T *t) : t{t} {
        if (isOwned(t)) {
            throw std::runtime_error("Pointer is already owned");
        }
        addToOwned(t);
    }

    template<class T>
    optional<T>::optional() : t{nullptr} {}

    template<class T>
    optional<T>::optional(optional<T> &other) {
        if (t == other.t) return;
        if (t != nullptr) {
            removeFromOwned(t);
            delete t;
        }
        t = other.release();
        addToOwned(t);
        other.t = nullptr;
    }

    template<class T>
    optional<T>::optional(optional<T> &&other) noexcept {
        if (t == other.t) return;
        if (t != nullptr) {
            removeFromOwned(t);
            delete t;
        }
        t = other.release();
        addToOwned(t);
        other.t = nullptr;
    }

    template<class T>
    optional<T> &optional<T>::operator=(optional<T> &&other) noexcept {
        if (t == other.t) return *this;
        if (t != nullptr) {
            removeFromOwned(t);
            delete t;
        }
        t = other.release();
        addToOwned(t);
        other.t = nullptr;
        return *this;
    }

    template<class T>
    bool optional<T>::isOwned(T *t) {
        for (unsigned i = 0; i < owned_ptrs.size(); i++) {
            if (owned_ptrs.at(i) == t) return true;
        }
        return false;
    }

    template<class T>
    void optional<T>::addToOwned(T *t) {
        owned_ptrs.push_back(t);
    }

    template<class T>
    void optional<T>::removeFromOwned(T *t) {
        auto f = std::find(owned_ptrs.begin(), owned_ptrs.end(), t);
        if (f != owned_ptrs.end()) {
            owned_ptrs.erase(f);
        }
    }


    template<class T>
    const optional_ptr<T> &optional<T>::none = optional_ptr<T>();

    template<class T> std::vector<T *> optional<T>::owned_ptrs;

}