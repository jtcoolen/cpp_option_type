#include <iostream>
#include "../include/optional_stack.hpp"
#include "../include/optional.hpp"


int *f(int *x) {
    if (x == nullptr) {
        return nullptr;
    }
    *x = 2 * (*x);
    return x;
}

class A {
public:
    int x;

    explicit A(int x) : x(x) {}
};

class B {
public:
    std::string y;

    explicit B(const std::string &y) : y(y) {}
};


bool pred(A a) {
    return a.x == 4;
}

B *AtoBMapFun(A a) {
    if (a.x > 0) {
        return new B{"hello"};
    }
    return nullptr;
}

int main() {
    /* On propose deux implémentations d'optional :
     *
     * - optional encapsule un pointeur pointant vers une copie de l'objet
     * passé en argument aux fabriques (par référence ou pointeur). C'est ce
     * que demande l'énoncé car l'on demande un pointeur comme attribut du
     * type optionnel.
     *
     * - (bonus) optional_stack stocke le contenu du pointeur dans la pile.
     * Corollaire : pas de gestion de pointeur.
     *
     * On aurait aussi pu implémenter un type optionnel qui gère la durée de vie
     * du pointeur passé en argument tout comme dans l'exercice 1 (comme std::unique_ptr).
     * Dans ce cas, on aurait supprimé le constructeur/l'affectation par copie
     * et employé un constructeur/affectation par déplacement (move constructor/assignment).
     */


    // Fonctions lambda proposées en alternative des références vers les fonctions pour filter et map
    std::function<auto(A) -> bool> predLambda =
            [](A a) -> bool { return a.x == 4; };

    std::function<auto(A) -> B *> AtoBMap = [](A a) -> B * {
        return a.x > 0 ? new B{"hey"} : nullptr;
    };


    std::cout << "Test de optional\n\n";


    std::cout << "\n\n2: Test des fabriques\n";
    int *x = new int{2};
    A *abis = new A{4};
    A a2bis{-2};
    //lib::optional<int> o = lib::optional<int>(f(x)); // ne fonctionne pas (constructeur privé)
    lib::optional<int>::ofNullable(f(x)); // OK, appel de la fabrique ofNullable
    lib::optional<A>::of(a2bis); // OK, appel de la fabrique of

    std::cout << "\n\n2: Test isEmpty/isPresent\n";
    lib::optional<A> oabis = lib::optional<A>::ofNullable(abis); // copy assignment
    std::cout << oabis.isPresent() << "\n";
    std::cout << oabis.isEmpty() << "\n";
    lib::optional<A> onull{lib::optional<A>::ofNullable(nullptr)}; // copy constructor
    std::cout << onull.isPresent() << "\n";
    std::cout << onull.isEmpty() << "\n";


    std::cout << "\n\n3/4: Test orElse/orElseThrow\n";
    std::cout << oabis.orElse(a2bis).x << "\n";
    std::cout << onull.orElse(a2bis).x << "\n";
    //std::cout << onull.orElseThrow().x << "\n"; // Exception levée: Cannot get value of None type

    std::cout << "\n\n5: Copie et affectation par copie\n";
    /* Contrairement à l'exercice 1, j'ai décidé de ne pas implémenter un unique_ptr :
     * le type optionnel encapsule un pointeur pointant vers une copie de l'objet
     * passé en argument aux méthodes fabriques.
     *
     * On ne se charge donc pas de la durée de vie d'un pointeur passé en argument à ofNullable
     * et l'on ne désactive pas la construction/affectation par copie pour implémenter la
     * construction/affectation dite "move".
     *
     * On n'a donc pas nécéssairement besoin du move constructor/assignment,
     * sauf si l'on souhaite éventuellement économiser des copies inutiles (rule of 5).
     *
     * Ainsi on se contente de la construction et de l'affectation par copie, car
     * dans la plupart des cas d'utilisation on ne manipule qu'une instance de optional,
     * celle retourné par une fonction de type retour optional, qui retourne un objet
     * optional construit via la fabrique of ou bien ofNullable, et se charge bien entendu
     * de supprimer le pointeur dont on a copié le contenu dans le cas de ofNullable.
     */
    B btest = B("some test");
    lib::optional<B> botest{lib::optional<B>::of(btest)}; // Construction par copie
    lib::optional<B> bo2test = botest; // Affectation par copie
    std::cout << bo2test.orElseThrow().y << "\n";
    std::cout << bo2test.orElseThrow().y << "\n";
    lib::optional<B> bo3test{botest}; // Construction par copie
    std::cout << bo3test.orElseThrow().y << "\n";
    std::cout << bo3test.isEmpty() << "\n";

    std::cout << "\n\n6: Pointeurs sur optional\n";
    /* On décide qu'un pointeur vers un type optional<A> se comporte comme
     * un un pointeur sur A.
     *
     * En effet, un pointeur vers un optionnel n'a pas vraiment de sens
     * donc on préfère qu'il se comporte comme un pointeur basique.
     *
     * Ainsi * déréférence le pointeur encapsulé (quitte a faire quelque
     * chose on peut retourner une exception pour traiter le cas où le
     * pointeur encapsulé est nul, -> permet d'accéder au pointeur encapsulé
     * et retourne une exception si jamais on obtient nullptr.
     *
     * Ces exceptions forcent l'utilisateur à traiter les cas des nullptr,
     * car les erreurs de manipulation de pointeur sont levés à la compilation
     * et non pas à l'exécution avec des pointeurs classiques.
     *
     * En revanche le & n'est pas surchargé car l'on souhaite comparer des instances
     * de optional, on ne veut pas que l'adresse de deux optionals soit égale ssi leurs
     * pointeurs encapsulés pointent vers la même adresse.
     */
    auto *a3bis = new lib::optional<A>{lib::optional<A>::of(a2bis)};
    std::cout << a3bis->isPresent() << "\n";
    std::cout << a3bis->orElseThrow().x << "\n";
    std::cout << (*a3bis)->x << "\n";
    std::cout << (&a3bis) << "\n";


    std::cout << "\n\n7: Test de conversion vers booléen\n";
    if (oabis) {
        std::cout << "ok" << "\n";
    } else {
        std::cout << "not ok" << "\n";
    }


    std::cout << "\n\n8: Test de map\n";
    std::cout << oabis.map(AtoBMap).orElseThrow().y << "\n";
    std::cout << oabis.map(AtoBMap).isPresent() << "\n";
    std::cout << oabis.map(AtoBMapFun).orElseThrow().y << "\n";
    std::cout << oabis.map(AtoBMapFun).isEmpty() << "\n";

    std::cout << "\n\n9: Test de filter\n";

    std::cout << oabis.filter(&pred).isPresent() << "\n";

    std::cout << oabis.filter(predLambda).isPresent() << "\n";
    std::cout << lib::optional_stack<A>::of(a2bis).filter(pred).isPresent() << "\n";
    std::cout << oabis.filter(predLambda).isPresent() << "\n";

    std::cout << lib::optional_stack<A>::of(a2bis).filter(pred).isPresent() << "\n";

    delete abis; // la classe optional ne se charge pas de la durée de vie des pointeurs
    // passés à la méthode fabrique ofNullable.
    delete a3bis;

    // FIN DU TP



    // =========================================================================================
    // BONUS
    std::cout << "\n\n\nTest de optional_stack (BONUS, fait par curiosité)\n\n";

    lib::optional_stack<int>::ofNullable(f(x));
    //lib::optional_stack<int> o = lib::optional_stack<int>(f(x)); // ne fonctionne pas (constructeur privé)
    A *a = new A{4};
    A a2 = A{-2};
    A a3 = A{5};


    std::cout << "\n\n2: Test isEmpty/isPresent\n";
    std::cout << lib::optional_stack<A>::ofNullable(a).isPresent() << "\n";
    std::cout << lib::optional_stack<A>::ofNullable(a).isEmpty() << "\n";
    std::cout << lib::optional_stack<A>::ofNullable(nullptr).isEmpty() << "\n";
    std::cout << lib::optional_stack<A>::ofNullable(nullptr).isPresent() << "\n";


    std::cout << "\n\n3/4: Test orElse/orElseThrow\n";
    std::cout << lib::optional_stack<A>::ofNullable(a).orElse(a3).x << "\n";
    std::cout << lib::optional_stack<A>::ofNullable(nullptr).orElse(a3).x << "\n";

    std::cout << "\n\n5: Copie et affectation par copie\n";
    /* 5: Copie et affectation
     * On ne propose que la construction par copie et l'affectation (par copie)
     * L'affectation dite "move assignment" n'a pas vraiment d'intérêt étant
     * donné que notre implémentation d'optional_stack manipule une donnée et non
     * pas un pointeur dont il faut changer l'appartenance ("ownership") comme
     * dans l'exercice 1.
     */
    B b2 = B("some test");
    lib::optional_stack<B> bo = lib::optional_stack<B>::of(b2);
    lib::optional_stack<B> bo2 = bo; // affectation par copie
    std::cout << bo.orElseThrow().y << "\n";
    std::cout << bo2.orElseThrow().y << "\n";
    lib::optional_stack<B> bo3{bo}; // copie
    std::cout << bo3.orElseThrow().y << "\n";


    std::cout << "\n\n6: Pointeurs sur optional_stack\n";
    lib::optional_stack<A> a6 = lib::optional_stack<A>::of(*a);
    lib::optional_stack<A> *a6p = &a6;
    std::cout << a6p->isPresent() << "\n";
    std::cout << a6p->orElseThrow().x << "\n";
    std::cout << (*a6p)->x << "\n";
    std::cout << *(&a6p) << "\n";
    std::cout << (&a6) << "\n"; // Adresse d'un optional_stack


    std::cout << "\n\n7: Test de conversion vers booléen\n";
    if (lib::optional_stack<A>::ofNullable(a)) {
        std::cout << "ok" << "\n";
    } else {
        std::cout << "not ok" << "\n";
    }


    std::cout << "\n\n8: Test de map\n";
    std::cout << lib::optional_stack<A>::ofNullable(a).map(AtoBMap).orElseThrow().y << "\n";
    std::cout << lib::optional_stack<A>::of(a2).map(AtoBMap).isEmpty() << "\n";
    std::cout << lib::optional_stack<A>::ofNullable(a).map(AtoBMapFun).orElseThrow().y << "\n";
    std::cout << lib::optional_stack<A>::of(a2).map(AtoBMapFun).isEmpty() << "\n";


    std::cout << "\n\n9: Test de filter\n";
    A a4 = lib::optional_stack<A>::ofNullable(a).orElseThrow();
    std::cout << a4.x << "\n";
    lib::optional_stack<A> a5{lib::optional_stack<A>::ofNullable(a).filter(predLambda)};
    std::cout << a5.isEmpty() << "\n";
    std::cout << lib::optional_stack<A>::ofNullable(a).filter(&pred).isPresent() << "\n";

    std::cout << lib::optional_stack<A>::of(a2).filter(&pred).isPresent() << "\n";

    // optional_stack ne supprime pas le pointeur passé en argument des fonctions fabriques
    delete x;
    delete a;


    return 0;
}

