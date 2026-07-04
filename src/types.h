#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

/* ============================================
   DÉFINITIONS DES TYPES DE DONNÉES
   ============================================ */

// Type pour les chaînes de caractères (valeur simple)
#define STRING_TYPE 1

// Type pour les listes (collection ordonnée)
#define LIST_TYPE 2

/* ============================================
   STRUCTURE POUR LES VALEURS STRING
   ============================================
   Exemple: SET nom "Alice"
   - key = "nom"
   - value = "Alice"
*/
typedef struct {
    char* value;      // La chaîne de caractères (ex: "Alice")
    int type;         // Le type de données (STRING_TYPE)
} StringType;

/* ============================================
   STRUCTURE D'UN NŒUD DE LISTE
   ============================================
   Une liste doublement chaînée permet d'aller
   dans les deux directions (avant/arrière).
   
   C'est comme une chaîne de personnes où chacun
   connaît la personne devant et derrière lui.
*/
typedef struct ListNode {
    char* value;              // La valeur stockée dans ce nœud
    struct ListNode* prev;    // Pointeur vers le nœud précédent
    struct ListNode* next;    // Pointeur vers le nœud suivant
} ListNode;

/* ============================================
   STRUCTURE D'UNE LISTE
   ============================================
   Contient tous les nœuds et permet de:
   - Ajouter en tête (LPUSH) - O(1)
   - Ajouter en queue (RPUSH) - O(1)
   - Retirer de la tête (LPOP) - O(1)
   - Retirer de la queue (RPOP) - O(1)
*/
typedef struct {
    ListNode* head;    // Premier élément (tête de liste)
    ListNode* tail;    // Dernier élément (queue de liste)
    size_t size;       // Nombre d'éléments dans la liste
    int type;          // Type de données (LIST_TYPE)
} ListType;

/* ============================================
   UNION POUR STOCKER DIFFÉRENTS TYPES
   ============================================
   Une union permet de stocker UN SEUL type à la fois:
   - Soit un StringType
   - Soit un ListType
   
   C'est comme une boîte qui peut contenir soit
   un gâteau, soit une pizza, mais pas les deux.
*/
typedef union {
    StringType string;  // Pour stocker une chaîne
    ListType list;      // Pour stocker une liste
} DataValue;

/* ============================================
   STRUCTURE D'UNE ENTRÉE (CLÉ-VALEUR)
   ============================================
   Chaque entrée dans la table de hachage contient:
   - Une clé (ex: "nom")
   - Une valeur (soit string, soit list)
   - Un pointeur 'next' pour gérer les collisions
   
   Les collisions: quand deux clés différentes
   donnent le même hash, on les stocke dans une
   liste chaînée (comme une file d'attente).
*/
typedef struct Entry {
    char* key;              // La clé (ex: "user:1:nom")
    DataValue value;        // La valeur (string ou list)
    int data_type;          // Type: STRING_TYPE ou LIST_TYPE
    struct Entry* next;     // Prochain élément en cas de collision
} Entry;

#endif