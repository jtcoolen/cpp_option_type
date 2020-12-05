#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <ostream>
#include <functional>

// =====================================================
// ATTENTION, ceci n'est pas l'implémentation demandée dans le tp car l'on ne manipule pas
// un pointeur. Voir le fichier optional.hpp pour l'implémentation demandée.
// je l'ai laissé en bonus

namespace lib {

    template<class T>
    class optional_stack {
    private:
        /* On stocke le contenu du pointeur dans la pile (on ne
         * stocke pas le pointeur lui-même).
         *
         * Cela nous exempt de la gestion des pointeurs.
         */

        alignas(alignof(T)) char t[sizeof(T)];
        /* alignof() retourne l'alignement minimum requis pour le type T
         * alignas(n) permet de stocker la variable dans la mémoire
         * à une adresse multiple de n.
         * alignas employée en conjonction avec alignof permet donc
         * de stocker une variable avec le bon alignement
         */

        bool is_empty; // true ssi l'instance est empty

        // Constructeur pour l'optional_stack non vide
        // Privé car l'on ne souhaite pas construire d'optionnel directement,
        explicit optional_stack(const T &t);

        // Constructeur pour l'optional_stack vide, privé donc
        explicit optional_stack();

        // Référence statique vers l'optional_stack vide
        const static optional_stack<T> &none;

        /* Retourne le tableau de char sous forme
         * d'un pointeur vers un objet de type T.
         */
        const T *pointer_to_t() const;

    public:
        /* On souhaite l'un des opérateurs suivants donc tous ("Rule of three") :
         * - un destructeur
         * - un constructeur par copie
         * - un opérateur d'affectation
         */
        ~optional_stack();

        optional_stack(const optional_stack<T> &other);

        optional_stack<T> &operator=(const optional_stack<T> &other);

        /* Surcharge de l'opérateur bool() pour convertir l'optional_stack
         * en une valeur de vérité.
         * Retourne la valeur retournée par isPresent(),
         * ie. true ssi il est différent de empty(). */
        explicit operator bool() const;

        /* Fabriques de valeurs optionnelles */
        static optional_stack<T> of(T &t);

        static optional_stack<T> ofNullable(T *t);

        /* Retourne une référence vers l'optional_stack empty */
        static const optional_stack<T> &empty();

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
        optional_stack<U> map(std::function<U *(T)> f);

        template<class U>
        optional_stack<U> map(U *(&f)(T));

        optional_stack<T> filter(std::function<bool(T)> predicate);

        optional_stack<T> filter(bool (&predicate)(T));
    };

    template<class T>
    template<class U>
    optional_stack<U> optional_stack<T>::map(std::function<U *(T)> f) {
        if (isEmpty()) {
            return optional_stack<U>::empty();
        }
        U *u = f(*pointer_to_t());
        optional_stack<U> o = optional_stack<U>::ofNullable(u);
        if (u != nullptr) {
            delete u;
        }
        return o;
    }

    template<class T>
    template<class U>
    optional_stack<U> optional_stack<T>::map(U *(&f)(T)) {
        if (isEmpty()) {
            return optional_stack<U>::empty();
        }
        U *u = f(*pointer_to_t());
        optional_stack<U> o = optional_stack<U>::ofNullable(u);
        if (u != nullptr) {
            delete u;
        }
        return o;
    }

    template<class T>
    optional_stack<T>
    optional_stack<T>::filter(std::function<bool(T)> predicate) {
        if (isEmpty()) {
            return optional_stack<T>::empty();
        }
        if (predicate(*pointer_to_t())) {
            return *this;
        }
        return optional_stack<T>::empty();
    }

    template<class T>
    optional_stack<T>
    optional_stack<T>::filter(bool(&predicate)(T)) {
        if (isEmpty()) {
            return optional_stack<T>::empty();
        }
        if (predicate(*pointer_to_t())) {
            return *this;
        }
        return optional_stack<T>::empty();
    }

    template<class T>
    optional_stack<T>::operator bool() const {
        return isPresent();
    }

    template<class T>
    T optional_stack<T>::orElse(T &other) const {
        if (isEmpty()) {
            return other;
        }
        return *pointer_to_t();
    }

    template<class T>
    T optional_stack<T>::orElseThrow() const {
        if (isEmpty()) {
            throw std::runtime_error("Cannot get value of None type");
        }
        return *pointer_to_t();
    }

    template<class T>
    bool optional_stack<T>::isEmpty() const {
        return is_empty;
    }

    template<class T>
    optional_stack<T>::~optional_stack() {
        // Appel du destructeur de T,
        // on supprime l'objet pointé par pointer_to_t.
        pointer_to_t()->~T();
    }

    template<class T>
    optional_stack<T> optional_stack<T>::of(T &t) {
        return optional_stack<T>(t);
    }

    template<class T>
    const optional_stack<T> &optional_stack<T>::empty() {
        return none;
    }

    template<class T>
    optional_stack<T> optional_stack<T>::ofNullable(T *t) {
        if (t == nullptr) {
            return optional_stack<T>::empty();
        }
        return optional_stack<T>(*t);
    }

    template<class T>
    bool optional_stack<T>::isPresent() const {
        return !isEmpty();
    }

    template<class T>
    const T &optional_stack<T>::operator*() {
        return pointer_to_t() == nullptr ? throw std::runtime_error("Cannot dereference nullptr") : *pointer_to_t();
    }

    template<class T>
    const T *optional_stack<T>::operator->() {
        return pointer_to_t();
    }

    template<class T>
    const T *optional_stack<T>::pointer_to_t() const {
        /* C++ style cast, vérifie le cast à la compilation
         * contrairement au C-style cast : (T*) t.
         */
        return reinterpret_cast<const T *>(t);
    }

    template<class T>
    optional_stack<T>::optional_stack(const T &t) : is_empty{false} {
        /* Syntaxe "placement new" :
         * copie le pointeur t de type T
         * dans le tableau this->t.
         * En C on emploie plutôt memcpy mais c'est une 
         * très mauvaise idée de faire de même en C++.
         */
        new(this->t) T(t);
    }

    template<class T>
    optional_stack<T>::optional_stack() : is_empty{true} {}

    template<class T>
    optional_stack<T>::optional_stack(const optional_stack<T> &other)
            : is_empty{other.is_empty} {
        // on copie le contenu du pointeur de other dans this->t
        new(this->t) T(*other.pointer_to_t());
    }

    template<class T>
    optional_stack<T> &optional_stack<T>::operator=(const optional_stack<T> &other) {
        // on copie le contenu de other dans l'instance courante
        if (&other != this) {
            is_empty = other.is_empty;
            new(this->t) T(*other.t);
        }
        return *this;
    }


    template<class T>
    const optional_stack<T> &optional_stack<T>::none = optional_stack<T>();

}


#endif
