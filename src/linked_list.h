#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "types.h"

/* ============================================
   LISTE DOUBLEMENT CHAÎNÉE
   ============================================
   
   Une liste doublement chaînée permet:
   - D'ajouter/retirer en tête (LPUSH/LPOP) en O(1)
   - D'ajouter/retirer en queue (RPUSH/RPOP) en O(1)
   
   Chaque nœud sait qui est son voisin AVANT et APRÈS.
*/

// Créer un nouveau nœud avec une valeur
// Retourne: pointeur vers le nœud créé, ou NULL si erreur
ListNode* list_node_create(const char* value);

// Créer une liste vide
// Retourne: pointeur vers la liste créée, ou NULL si erreur
ListType* list_create();

// Libérer toute la mémoire d'une liste
void list_free(ListType* list);

/* ============================================
   OPÉRATIONS PRINCIPALES (toutes en O(1))
   ============================================ */

// LPUSH: Ajouter une valeur en TÊTE de liste
// Exemple: [A, B, C] → LPUSH X → [X, A, B, C]
void list_push_left(ListType* list, const char* value);

// RPUSH: Ajouter une valeur en QUEUE de liste
// Exemple: [A, B, C] → RPUSH X → [A, B, C, X]
void list_push_right(ListType* list, const char* value);

// LPOP: Retirer et retourner la valeur en TÊTE
// Exemple: [A, B, C] → LPOP → retourne "A", liste devient [B, C]
// Retourne: la valeur, ou NULL si liste vide
char* list_pop_left(ListType* list);

// RPOP: Retirer et retourner la valeur en QUEUE
// Exemple: [A, B, C] → RPOP → retourne "C", liste devient [A, B]
// Retourne: la valeur, ou NULL si liste vide
char* list_pop_right(ListType* list);

// Obtenir le nombre d'éléments dans la liste
size_t list_size(const ListType* list);

// Convertir la liste en tableau (pour affichage)
// Retourne: tableau de chaînes, ou NULL si vide
char** list_to_array(const ListType* list, size_t* count);

#endif