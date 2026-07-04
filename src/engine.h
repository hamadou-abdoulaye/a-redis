#ifndef ENGINE_H
#define ENGINE_H

#include "types.h"
#include "hash_table.h"

/* ============================================
   MOTEUR α-Redis
   ============================================
   
   Le moteur est le cerveau du système. Il:
   - Contient la table de hachage (storage)
   - Interprète les commandes (SET, GET, etc.)
   - Exécute les opérations
   - Retourne les résultats
   
   Analogie: C'est comme un serveur dans un restaurant:
   - Il reçoit les commandes (clients)
   - Il les exécute (cuisine)
   - Il retourne les résultats (plats)
*/

// Structure du moteur
typedef struct {
    HashTable* storage;           // La base de données (table de hachage)
    int commands_executed;        // Compteur de commandes exécutées
} ARedisEngine;

/* ============================================
   FONCTIONS DU MOTEUR
   ============================================ */

// Créer un nouveau moteur (initialiser la base de données)
// Retourne: pointeur vers le moteur, ou NULL si erreur
ARedisEngine* engine_create();

// Libérer toute la mémoire du moteur
void engine_free(ARedisEngine* engine);

// Exécuter une commande (ex: "SET nom Alice")
// Retourne: chaîne de résultat (à libérer par l'appelant)
// Exemple: "(integer) 1" ou "\"Alice\"" ou "(nil)"
char* engine_execute(ARedisEngine* engine, const char* command);

// Obtenir les statistiques du moteur
void engine_get_stats(ARedisEngine* engine, 
                      int* total_keys,        // Nombre de clés
                      int* commands_executed, // Commandes exécutées
                      int* hash_size,         // Taille table hash
                      double* load_factor);   // Facteur de charge

// Obtenir toutes les clés (pour affichage)
void engine_get_all_data(ARedisEngine* engine, 
                         char*** keys,  // Tableau de clés
                         int* count);   // Nombre de clés

// Effacer toutes les données (FLUSHALL)
void engine_flushall(ARedisEngine* engine);

// ============================================
// FONCTIONS DU SERVEUR WEB
// ============================================
// Lance le serveur web (bloquant)
int run_web_server(ARedisEngine* engine);

#endif
