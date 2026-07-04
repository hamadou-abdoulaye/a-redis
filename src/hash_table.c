#include "hash_table.h"
#include "linked_list.h"  // Pour list_create()
#include <stdlib.h>
#include <string.h>

/* ============================================
   FONCTION DE HACHAGE DJB2
   ============================================
   
   C'est le cœur du système: transforme une clé (chaîne)
   en un nombre (hash), puis en un index dans le tableau.
   
   Algorithme:
   1. Commencer avec hash = 5381
   2. Pour chaque caractère de la clé:
      hash = (hash * 33) + caractère
   3. Retourner hash % taille_tableau
   
   Exemple avec "nom":
   - hash = 5381
   - 'n' (ASCII 110): hash = (5381 * 33) + 110 = 177583
   - 'o' (ASCII 111): hash = (177583 * 33) + 111 = 5860230
   - 'm' (ASCII 109): hash = (5860230 * 33) + 109 = 193188079
   - Index = 193188079 % 1024 = 127
*/
unsigned int hash_djb2(const char* key) {
    unsigned int hash = 5381;  // Valeur de départ magique
    int c;
    
    // Parcourir chaque caractère de la clé
    while ((c = *key++)) {
        // Formule: hash * 33 + caractère
        // (hash << 5) = hash * 32
        // (hash << 5) + hash = hash * 33
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/* ============================================
   CRÉATION DE LA TABLE DE HACHAGE
   ============================================
   
   Alloue et initialise une nouvelle table vide.
   
   - table: tableau de pointeurs (tous NULL au début)
   - size: 1024 cases par défaut
   - count: 0 élément au début
*/
HashTable* hash_table_create(int initial_size) {
    // 1. Allouer la structure HashTable
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (!ht) return NULL;
    
    // 2. Allouer le tableau de pointeurs (tous NULL)
    // calloc met automatiquement tous les pointeurs à NULL
    ht->table = (Entry**)calloc(initial_size, sizeof(Entry*));
    if (!ht->table) {
        free(ht);
        return NULL;
    }
    
    // 3. Initialiser les champs
    ht->size = initial_size;
    ht->count = 0;
    ht->load_factor_threshold = LOAD_FACTOR_THRESHOLD;
    
    return ht;
}

/* ============================================
   LIBÉRATION D'UNE ENTRÉE
   ============================================
   Libère la mémoire d'une seule entrée (clé + valeur).
*/
void entry_free(Entry* entry) {
    if (!entry) return;
    
    // 1. Libérer la clé
    free(entry->key);
    
    // 2. Libérer la valeur selon son type
    if (entry->data_type == STRING_TYPE) {
        // Pour une chaîne: libérer la valeur
        free(entry->value.string.value);
    } else if (entry->data_type == LIST_TYPE) {
        // Pour une liste: libérer toute la liste
        list_free(&entry->value.list);
    }
    
    // 3. Libérer l'entrée elle-même
    free(entry);
}

/* ============================================
   LIBÉRATION DE LA TABLE COMPLÈTE
   ============================================
   Parcourt toutes les cases du tableau et libère
   toutes les entrées (y compris les collisions).
*/
void hash_table_free(HashTable* ht) {
    if (!ht) return;
    
    // Parcourir toutes les cases du tableau
    for (int i = 0; i < ht->size; i++) {
        // Pour chaque case, parcourir la liste chaînée
        Entry* current = ht->table[i];
        while (current) {
            Entry* next = current->next;
            entry_free(current);
            current = next;
        }
    }
    
    // Libérer le tableau et la structure
    free(ht->table);
    free(ht);
}

/* ============================================
   REDIMENSIONNEMENT DE LA TABLE
   ============================================
   
   Quand la table est trop pleine (load factor > 0.75),
   on double la taille et on réinsère tous les éléments.
   
   Exemple:
   - Ancienne taille: 1024
   - Nouvelle taille: 2048
   - Tous les éléments sont réhachés avec la nouvelle taille
   
   Cela maintient les performances O(1).
*/
void hash_table_resize(HashTable* ht, int new_size) {
    // 1. Créer un nouveau tableau vide
    Entry** new_table = (Entry**)calloc(new_size, sizeof(Entry*));
    if (!new_table) return;
    
    // 2. Parcourir l'ancien tableau
    for (int i = 0; i < ht->size; i++) {
        Entry* current = ht->table[i];
        while (current) {
            // Sauvegarder le prochain élément
            Entry* next = current->next;
            
            // Calculer le nouvel index avec la nouvelle taille
            unsigned int new_index = hash_djb2(current->key) % new_size;
            
            // Insérer dans le nouveau tableau
            current->next = new_table[new_index];
            new_table[new_index] = current;
            
            current = next;
        }
    }
    
    // 3. Remplacer l'ancien tableau par le nouveau
    free(ht->table);
    ht->table = new_table;
    ht->size = new_size;
}

/* ============================================
   INSERTION/MISE À JOUR (SET)
   ============================================
   
   Exemple: SET nom "Alice"
   1. Calculer hash("nom") % 1024 = index
   2. Chercher dans table[index] si "nom" existe
   3. Si oui: mettre à jour la valeur
   4. Si non: créer nouvelle entrée en tête de liste
   
   Retourne 1 si nouvelle insertion, 0 si mise à jour
*/
int hash_table_set(HashTable* ht, const char* key, 
                   const char* value, int data_type) {
    if (!ht || !key || !value) return 0;
    
    // Vérifier si on doit redimensionner
    double load_factor = (double)ht->count / ht->size;
    if (load_factor > ht->load_factor_threshold) {
        // Table trop pleine: doubler la taille
        hash_table_resize(ht, ht->size * 2);
    }
    
    // Calculer l'index
    unsigned int index = hash_djb2(key) % ht->size;
    
    // Chercher si la clé existe déjà
    Entry* current = ht->table[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // CLÉ EXISTE: mettre à jour la valeur
            if (current->data_type == STRING_TYPE) {
                free(current->value.string.value);
                current->value.string.value = strdup(value);
            }
            return 0;  // Mise à jour
        }
        current = current->next;
    }
    
    // CLÉ N'EXISTE PAS: créer nouvelle entrée
    Entry* new_entry = (Entry*)malloc(sizeof(Entry));
    if (!new_entry) return 0;
    
    // Copier la clé
    new_entry->key = strdup(key);
    new_entry->data_type = data_type;
    
    // Stocker la valeur selon le type
    if (data_type == STRING_TYPE) {
        new_entry->value.string.value = strdup(value);
        new_entry->value.string.type = STRING_TYPE;
    } else if (data_type == LIST_TYPE) {
        new_entry->value.list = *list_create();
    }
    
    // Insérer en TÊTE de la liste chaînée (pour gérer les collisions)
    new_entry->next = ht->table[index];
    ht->table[index] = new_entry;
    ht->count++;
    
    return 1;  // Nouvelle insertion
}

/* ============================================
   RECHERCHE (GET)
   ============================================
   
   Exemple: GET nom
   1. Calculer hash("nom") % 1024 = index
   2. Chercher dans table[index] la clé "nom"
   3. Retourner l'entrée trouvée, ou NULL
   
   Complexité: O(1) en moyenne
*/
Entry* hash_table_get(HashTable* ht, const char* key) {
    if (!ht || !key) return NULL;
    
    unsigned int index = hash_djb2(key) % ht->size;
    
    // Parcourir la liste chaînée à cet index
    Entry* current = ht->table[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current;  // Trouvé!
        }
        current = current->next;
    }
    
    return NULL;  // Non trouvé
}

/* ============================================
   SUPPRESSION (DEL)
   ============================================
   
   Exemple: DEL nom
   1. Calculer hash("nom") % 1024 = index
   2. Chercher dans table[index]
   3. Supprimer l'entrée de la liste chaînée
   4. Libérer la mémoire
   
   Retourne 1 si supprimé, 0 si non trouvé
*/
int hash_table_delete(HashTable* ht, const char* key) {
    if (!ht || !key) return 0;
    
    unsigned int index = hash_djb2(key) % ht->size;
    
    Entry* current = ht->table[index];
    Entry* prev = NULL;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // TROUVÉ: supprimer de la liste
            if (prev == NULL) {
                // C'est la tête de liste
                ht->table[index] = current->next;
            } else {
                // C'est au milieu ou à la fin
                prev->next = current->next;
            }
            
            // Libérer la mémoire
            entry_free(current);
            ht->count--;
            return 1;  // Succès
        }
        
        prev = current;
        current = current->next;
    }
    
    return 0;  // Non trouvé
}

/* ============================================
   VÉRIFICATION D'EXISTENCE (EXISTS)
   ============================================
   Retourne 1 si la clé existe, 0 sinon.
*/
int hash_table_exists(HashTable* ht, const char* key) {
    return hash_table_get(ht, key) != NULL;
}

/* ============================================
   OBTENIR TOUTES LES CLÉS (KEYS)
   ============================================
   Parcourt toute la table et retourne un tableau
   de toutes les clés.
*/
char** hash_table_keys(HashTable* ht, int* count) {
    if (!ht || !count) return NULL;
    
    *count = ht->count;
    if (*count == 0) return NULL;
    
    // Allouer le tableau de clés
    char** keys = (char**)malloc(sizeof(char*) * (*count));
    if (!keys) return NULL;
    
    // Parcourir toute la table
    int idx = 0;
    for (int i = 0; i < ht->size && idx < *count; i++) {
        Entry* current = ht->table[i];
        while (current && idx < *count) {
            // Copier chaque clé
            keys[idx++] = strdup(current->key);
            current = current->next;
        }
    }
    
    return keys;
}

/* ============================================
   NOMBRE D'ÉLÉMENTS (DBSIZE)
   ============================================
   Retourne simplement le compteur count.
*/
int hash_table_size(HashTable* ht) {
    if (!ht) return 0;
    return ht->count;
}

/* ============================================
   EFFACER TOUTES LES DONNÉES (FLUSHALL)
   ============================================
   Supprime toutes les entrées mais garde la table.
*/
void hash_table_clear(HashTable* ht) {
    if (!ht) return;
    
    // Parcourir et libérer toutes les entrées
    for (int i = 0; i < ht->size; i++) {
        Entry* current = ht->table[i];
        while (current) {
            Entry* next = current->next;
            entry_free(current);
            current = next;
        }
        ht->table[i] = NULL;
    }
    
    ht->count = 0;
}