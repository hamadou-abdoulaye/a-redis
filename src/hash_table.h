#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "types.h"

/* ============================================
   TABLE DE HACHAGE
   ============================================
   
   Une table de hachage permet de stocker des paires
   clé-valeur et de retrouver une valeur en O(1) 
   (temps constant) grâce à une fonction de hachage.
   
   Analogie: Comme un dictionnaire où vous cherchez
   un mot directement à la bonne page, sans parcourir
   toutes les pages.
*/

// Taille initiale de la table (puissance de 2)
#define INITIAL_SIZE 1024

// Seuil de redimensionnement (75%)
#define LOAD_FACTOR_THRESHOLD 0.75

/* ============================================
   STRUCTURE DE LA TABLE DE HACHAGE
   ============================================
   
   - table: tableau de pointeurs vers des Entry
   - size: taille du tableau (nombre de cases)
   - count: nombre d'éléments stockés
   - load_factor_threshold: seuil de redimensionnement
   
   Exemple avec size=4:
   table[0] → NULL
   table[1] → Entry("nom") → Entry("age") → NULL  (collision)
   table[2] → NULL
   table[3] → Entry("taches") → NULL
*/
typedef struct {
    Entry** table;      // Tableau de pointeurs vers Entry
    int size;           // Taille du tableau
    int count;          // Nombre d'éléments
    double load_factor_threshold;
} HashTable;

/* ============================================
   FONCTIONS DE LA TABLE DE HACHAGE
   ============================================ */

// Créer une nouvelle table de hachage vide
// Retourne: pointeur vers la table, ou NULL si erreur
HashTable* hash_table_create(int initial_size);

// Libérer toute la mémoire de la table
void hash_table_free(HashTable* ht);

/* ============================================
   FONCTION DE HACHAGE DJB2
   ============================================
   
   C'est l'algorithme de hachage choisi pour le projet.
   Il transforme une chaîne de caractères en un nombre.
   
   Exemple:
   - "nom" → hash = 12345 → index = 12345 % 1024 = 345
   - "age" → hash = 67890 → index = 67890 % 1024 = 234
   
   Si deux clés donnent le même index → COLLISION
   → On utilise une liste chaînée pour stocker les deux.
*/
unsigned int hash_djb2(const char* key);

// Insérer ou mettre à jour une clé-valeur
// Retourne: 1 si nouvelle insertion, 0 si mise à jour
int hash_table_set(HashTable* ht, const char* key, 
                   const char* value, int data_type);

// Récupérer une valeur par sa clé
// Retourne: pointeur vers Entry, ou NULL si non trouvé
Entry* hash_table_get(HashTable* ht, const char* key);

// Supprimer une clé
// Retourne: 1 si supprimé, 0 si non trouvé
int hash_table_delete(HashTable* ht, const char* key);

// Vérifier si une clé existe
// Retourne: 1 si existe, 0 sinon
int hash_table_exists(HashTable* ht, const char* key);

// Obtenir toutes les clés (pour la commande KEYS)
// Retourne: tableau de chaînes, met à jour count
char** hash_table_keys(HashTable* ht, int* count);

// Obtenir le nombre de clés
int hash_table_size(HashTable* ht);

// Effacer toutes les données
void hash_table_clear(HashTable* ht);

// Redimensionner la table (interne, quand load factor > 0.75)
void hash_table_resize(HashTable* ht, int new_size);

#endif