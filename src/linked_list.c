#include "linked_list.h"
#include <stdlib.h>
#include <string.h>

/* ============================================
   CRÉATION D'UN NŒUD
   ============================================
   Un nœud est comme une boîte qui contient:
   - Une valeur (chaîne de caractères)
   - Un pointeur vers le nœud précédent
   - Un pointeur vers le nœud suivant
   
   Exemple visuel:
   [Valeur: "Alice"] ←→ [Valeur: "Bob"] ←→ [Valeur: "Charlie"]
*/
ListNode* list_node_create(const char* value) {
    // 1. Allouer de la mémoire pour le nœud
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    if (!node) return NULL;  // Si allocation échoue, retourner NULL
    
    // 2. Copier la valeur (strdup = string duplicate)
    node->value = strdup(value);
    
    // 3. Initialiser les pointeurs à NULL
    node->prev = NULL;
    node->next = NULL;
    
    return node;
}

/* ============================================
   CRÉATION D'UNE LISTE VIDE
   ============================================
   Crée une liste vide prête à recevoir des éléments.
*/
ListType* list_create() {
    // 1. Allouer la mémoire pour la structure de liste
    ListType* list = (ListType*)malloc(sizeof(ListType));
    if (!list) return NULL;
    
    // 2. Initialiser la liste comme vide
    list->head = NULL;  // Pas de premier élément
    list->tail = NULL;  // Pas de dernier élément
    list->size = 0;     // Taille = 0
    list->type = LIST_TYPE;  // Type = liste
    
    return list;
}

/* ============================================
   LIBÉRATION DE LA MÉMOIRE D'UNE LISTE
   ============================================
   IMPORTANT: Cette fonction libère seulement les nœuds
   et les valeurs, mais PAS la structure ListType elle-même.
   
   Pourquoi? Parce que ListType fait partie d'une union
   dans Entry, et c'est Entry qui est alloué et libéré.
   
   Cette fonction est appelée depuis entry_free() qui
   libère l'Entry complet.
*/
void list_free(ListType* list) {
    if (!list) return;  // Si liste est NULL, ne rien faire
    
    // Parcourir tous les nœuds
    ListNode* current = list->head;
    while (current) {
        // Sauvegarder le prochain nœud AVANT de libérer
        ListNode* next = current->next;
        
        // Libérer la valeur
        free(current->value);
        // Libérer le nœud
        free(current);
        
        // Passer au suivant
        current = next;
    }
    
    // NE PAS libérer la structure list ici!
    // Elle fait partie de l'union dans Entry
}

/* ============================================
   LPUSH: AJOUTER EN TÊTE (O(1))
   ============================================
   Exemple: Liste = [B, C]
   LPUSH(list, "A")
   Résultat: [A, B, C]
   
   Complexité: O(1) - on ajoute directement en tête
*/
void list_push_left(ListType* list, const char* value) {
    if (!list) return;  // Sécurité: vérifier que liste existe
    
    // 1. Créer un nouveau nœud
    ListNode* node = list_node_create(value);
    if (!node) return;
    
    if (list->head == NULL) {
        // CAS 1: Liste vide
        // Le nouveau nœud est à la fois tête ET queue
        list->head = node;
        list->tail = node;
    } else {
        // CAS 2: Liste non vide
        // Le nouveau nœud pointe vers l'ancienne tête
        node->next = list->head;
        // L'ancienne tête pointe vers le nouveau nœud
        list->head->prev = node;
        // Le nouveau nœud devient la tête
        list->head = node;
    }
    
    list->size++;  // Augmenter la taille
}

/* ============================================
   RPUSH: AJOUTER EN QUEUE (O(1))
   ============================================
   Exemple: Liste = [A, B]
   RPUSH(list, "C")
   Résultat: [A, B, C]
   
   Complexité: O(1) - on ajoute directement en queue
*/
void list_push_right(ListType* list, const char* value) {
    if (!list) return;
    
    // 1. Créer un nouveau nœud
    ListNode* node = list_node_create(value);
    if (!node) return;
    
    if (list->tail == NULL) {
        // CAS 1: Liste vide
        list->head = node;
        list->tail = node;
    } else {
        // CAS 2: Liste non vide
        // Le nouveau nœud pointe vers l'ancienne queue
        node->prev = list->tail;
        // L'ancienne queue pointe vers le nouveau nœud
        list->tail->next = node;
        // Le nouveau nœud devient la queue
        list->tail = node;
    }
    
    list->size++;
}

/* ============================================
   LPOP: RETIRER DE LA TÊTE (O(1))
   ============================================
   Exemple: Liste = [A, B, C]
   LPOP(list) → retourne "A"
   Résultat: [B, C]
   
   Complexité: O(1) - on retire directement en tête
*/
char* list_pop_left(ListType* list) {
    if (!list || !list->head) return NULL;  // Liste vide
    
    // 1. Sauvegarder le nœud à supprimer
    ListNode* node = list->head;
    
    // 2. Copier la valeur (on va libérer le nœud)
    char* value = strdup(node->value);
    
    // 3. Déplacer la tête vers le nœud suivant
    list->head = node->next;
    
    if (list->head == NULL) {
        // CAS 1: Liste devenue vide
        list->tail = NULL;
    } else {
        // CAS 2: Liste pas vide
        // Le nouveau premier nœud n'a plus de précédent
        list->head->prev = NULL;
    }
    
    // 4. Libérer l'ancien nœud
    free(node->value);
    free(node);
    list->size--;
    
    return value;
}

/* ============================================
   RPOP: RETIRER DE LA QUEUE (O(1))
   ============================================
   Exemple: Liste = [A, B, C]
   RPOP(list) → retourne "C"
   Résultat: [A, B]
   
   Complexité: O(1) - on retire directement en queue
*/
char* list_pop_right(ListType* list) {
    if (!list || !list->tail) return NULL;
    
    // 1. Sauvegarder le nœud à supprimer
    ListNode* node = list->tail;
    
    // 2. Copier la valeur
    char* value = strdup(node->value);
    
    // 3. Déplacer la queue vers le nœud précédent
    list->tail = node->prev;
    
    if (list->tail == NULL) {
        // CAS 1: Liste devenue vide
        list->head = NULL;
    } else {
        // CAS 2: Liste pas vide
        // Le nouveau dernier nœud n'a plus de suivant
        list->tail->next = NULL;
    }
    
    // 4. Libérer l'ancien nœud
    free(node->value);
    free(node);
    list->size--;
    
    return value;
}

/* ============================================
   TAILLE DE LA LISTE
   ============================================
   Retourne simplement le compteur size.
   Complexité: O(1)
*/
size_t list_size(const ListType* list) {
    if (!list) return 0;
    return list->size;
}

/* ============================================
   CONVERSION EN TABLEAU (pour affichage)
   ============================================
   Convertit la liste en tableau de chaînes pour
   pouvoir l'afficher facilement.
   
   Exemple: [A, B, C] → ["A", "B", "C"]
*/
char** list_to_array(const ListType* list, size_t* count) {
    if (!list || !count) return NULL;
    
    *count = list->size;
    if (*count == 0) return NULL;
    
    // 1. Allouer le tableau de pointeurs
    char** array = (char**)malloc(sizeof(char*) * (*count));
    if (!array) return NULL;
    
    // 2. Parcourir la liste et copier chaque valeur
    ListNode* current = list->head;
    size_t i = 0;
    
    while (current && i < *count) {
        array[i] = strdup(current->value);  // Copier la valeur
        current = current->next;
        i++;
    }
    
    return array;
}