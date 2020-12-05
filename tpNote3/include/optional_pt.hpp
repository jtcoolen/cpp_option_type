#ifndef OPTIONAL_PTR_HPP
#define OPTIONAL_PTR_HPP

#include <ostream>
#include <functional>
#include <vector>
#include <algorithm>


namespace lib {

    template<class T>
    class optional {
    private:
        T *t;

        static std::vector<T *> owned_ptrs; // pointeurs manipulés par les instances de optional

        static bool isOwned(T *t);

        // Ajoute le pointeur à la liste des pointeurs owned
        static void addToOwned(T *t);

        // Supprime le pointeur de la liste des pointeurs owned
        static void removeFromOwned(T *t);

        T *release();

        // Constructeur pour l'optional non vide
        // Privé car l'on ne souhaite pas construire d'optionnel directement,
        // Si ce n'était pas pour l'exercice ce serait naturel de pouvoir passer par un constructeur.
        explicit optional(T *t);

        // Constructeur pour l'optional vide, privé donc
        explicit optional();

        // Référence statique vers l'optional vide
        const static optional<T> &none;

        /* Retourne le tableau de char sous forme
         * d'un pointeur vers un objet de type T.
         */
        optional(optional<T> &other); // copy constructor = delete;
    public:
        /* Comme pour l'exo 1, on n'implémente pas le constructeur par copie
         * pour qu'une seule instance d'unique_ptr ait l'ownership d'un pointeur donné.
         * On implémente le move assignment pour transférer l'appartenance d'un pointeur
         * d'une instance à une autre.
         */
        ~optional();



        optional(optional<T> &&other) noexcept; // move constructor

        optional<T> &operator=(optional<T> &other) = delete; // delete copy assignment

        optional<T> &operator=(optional<T> &&other) noexcept; // move assignment

        /* Surcharge de l'opérateur bool() pour convertir l'optional_stack
         * en une valeur de vérité.
         * Retourne la valeur retournée par isPresent(),
         * ie. true ssi il est différent de empty(). */
        explicit operator bool() const;

        /* Fabriques de valeurs optionnelles */
        static optional<T> of(T &t);

        static optional<T> ofNullable(T *t);

        /* Retourne une référence vers l'optional empty */
        static const optional<T> &empty();

        bool isEmpty() const;

        bool isPresent() const;

        T orElseThrow() const;

        T orElse(T &other) const;

        /*
         * Implémentation des opérateurs sur pointeurs
         */
        const T &operator*();

        const T *operator->();

        /* On propose deux versions de map et filter :
         * - l'une prend en argument un pointeur vers une fonction (comme en C)
         * - l'autre, plus idiomatique en C++ moderne (>= C++11), prend en argument
         * une lambda.
         *
         * Note : Et non pas un foncteur de type UnaryFunction, obsolète en C++11
         * et supprimé dans les versions suivantes.
         * En effet, il n'est pas possible de passer en argument un foncteur
         * UnaryFunction f et d'appeler l'opérateur (), ce dernier ne pouvant
         * être défini que pour les classes dérivées de UnaryFunction.
         */

        // Attention: les deux versions de map suppriment le pointeur retourné par f !
        template<class U>
        optional<U> map(std::function<U *(T)> f);

        template<class U>
        optional<U> map(U *(&f)(T));

        optional<T> filter(std::function<bool(T)> predicate);

        optional<T> filter(bool (*predicate)(T));
    };

    template<class T>
    template<class U>
    optional<U> optional<T>::map(std::function<U *(T)> f) {
        U *u = f(*t);
        optional<U> o = optional<U>::ofNullable(u);
        return o;
    }

    template<class T>
    template<class U>
    optional<U> optional<T>::map(U *(&f)(T)) {
        U *u = f(*t);
        optional<U> o = optional<U>::ofNullable(u);
        return o;
    }

    template<class T>
    optional<T>
    optional<T>::filter(std::function<bool(T)> predicate) {
        if (isEmpty()) {
            return optional<T>(nullptr);
        }
        if (predicate(*t)) {
            return *this;
        }
        return optional<T>(nullptr);
    }

    template<class T>
    optional<T>
    optional<T>::filter(bool(*predicate)(T)) {
        if (isEmpty()) {
            return optional<T>(nullptr);
        }
        if ((*predicate)(*t)) {
            return *this;
        }
        return optional<T>(nullptr);
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


#endif
