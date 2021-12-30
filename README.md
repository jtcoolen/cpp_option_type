Various implementation of an optional type (either store a pointer to the data or copy the data to the stack) in C++ in folder tpNote3.
On propose deux implémentations d'optional :

- optional encapsule un pointeur pointant vers une copie de l'objet
 passé en argument aux fabriques (par référence ou pointeur). C'est ce
 que demande l'énoncé car l'on demande un pointeur comme attribut du
 type optionnel.

 - (bonus) optional_stack stocke le contenu du pointeur dans la pile.
 Corollaire : pas de gestion de pointeur.

On aurait aussi pu implémenter un type optionnel qui gère la durée de vie
du pointeur passé en argument tout comme dans l'exercice 1 (comme std::unique_ptr).
Dans ce cas, on aurait supprimé le constructeur/l'affectation par copie
et employé un constructeur/affectation par déplacement (move constructor/assignment).

