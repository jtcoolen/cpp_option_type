#ifndef OPTIONAL_PTR_HPP
#define OPTIONAL_PTR_HPP

#include <ostream>
#include <functional>
#include <vector>
#include <algorithm>

// Ceci est l'implémentation demandée dans le TP

namespace lib {

    template<class T>
    class optional {
    private:
        T *t;

        // Constructeur pour l'optional non vide
        // Privé car l'on ne souhaite pas construire d'optionnel directement,
        explicit optional(T *t);

        // Référence statique vers l'optional vide
        const static optional<T> &none;

    public:
        /* On souhaite l'un des opérateurs suivants donc tous ("Rule of three") :
         * - un destructeur
         * - un constructeur par copie
         * - un opérateur d'affectation
         * cf. fonction main pour plus d'explications sur ce choix
         */
        ~optional();

        optional(const optional<T> &other);

        optional<T> &operator=(const optional<T> &other);

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

        optional<T> filter(bool (&predicate)(T));
    };

    template<class T>
    template<class U>
    optional<U> optional<T>::map(std::function<U *(T)> f) {
        U *u = f(*t);
        optional<U> o = optional<U>::ofNullable(u);
        if (u != nullptr) {
            delete u;
        }
        return o;
    }

    template<class T>
    template<class U>
    optional<U> optional<T>::map(U *(&f)(T)) {
        U *u = f(*t);
        optional<U> o = optional<U>::ofNullable(u);
        if (u != nullptr) {
            delete u;
        }
        return o;
    }

    template<class T>
    optional<T>
    optional<T>::filter(std::function<bool(T)> predicate) {
        if (isEmpty()) {
            return optional<T>::empty();
        }
        if (predicate(*t)) {
            return *this;
        }
        return optional<T>::empty();
    }

    template<class T>
    optional<T>
    optional<T>::filter(bool(&predicate)(T)) {
        if (isEmpty()) {
            return optional<T>::empty();
        }
        if (predicate(*t)) {
            return *this;
        }
        return optional<T>::empty();
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
            delete t;
        }
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
            return optional<T>::empty();
        }
        return optional<T>(new T(*t));
    }

    template<class T>
    bool optional<T>::isPresent() const {
        return !isEmpty();
    }

    template<class T>
    const T &optional<T>::operator*() {
        return t == nullptr ? throw std::runtime_error("Cannot dereference nullptr") : *t;
    }

    template<class T>
    const T *optional<T>::operator->() {
        return t;
    }

    template<class T>
    optional<T>::optional(T *t) : t{t} {}

    template<class T>
    optional<T>::optional(const optional<T> &other)
    /* On ne déréférence pas un pointeur nul! Et l'on copie l'objet pointé par other.t
     * afin de ne pas se retrouver avec deux pointeurs pointant vers le même objet
     * et obtenir un double free lorsque le destructeur de this et other est appelé.
     */
            : t{other.t == nullptr ? nullptr : new T(*other.t)} {}

    template<class T>
    optional<T> &optional<T>::operator=(const optional<T> &other) {
        if (&other != this) {
            /* On supprime l'objet uniquement pointé par t avant de le faire pointer
             * vers autre chose pour éviter une fuite de mémoire.
             */
            if (t != nullptr) {
                delete t;
            }
            // Copie ou non de l'objet pointé par le pointeur de l'autre instance other.
            if (other.t != nullptr) {
                t = new T(*other.t);
            } else {
                t = nullptr;
            }
        }
        return *this;
    }

    template<class T>
    const optional<T> &optional<T>::none = optional<T>(nullptr);

}


#endif
