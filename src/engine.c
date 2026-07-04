#include "engine.h"
#include "linked_list.h"  // Pour les opérations sur les listes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

/* ============================================
   CRÉATION DU MOTEUR
   ============================================
   
   Crée un nouveau moteur avec une base de données vide.
   C'est comme créer un nouveau restaurant vide:
   - On crée la structure (le bâtiment)
   - On initialise la base de données (les tables vides)
   - On met le compteur de commandes à 0
*/
ARedisEngine* engine_create() {
    // 1. Allouer le moteur
    ARedisEngine* engine = (ARedisEngine*)malloc(sizeof(ARedisEngine));
    if (!engine) return NULL;
    
    // 2. Créer la base de données (table de hachage)
    engine->storage = hash_table_create(INITIAL_SIZE);
    if (!engine->storage) {
        free(engine);
        return NULL;
    }
    
    // 3. Initialiser le compteur
    engine->commands_executed = 0;
    
    return engine;
}

/* ============================================
   LIBÉRATION DU MOTEUR
   ============================================
   Libère toute la mémoire:
   1. La table de hachage (avec toutes les données)
   2. Le moteur lui-même
*/
void engine_free(ARedisEngine* engine) {
    if (!engine) return;
    
    // Libérer la base de données
    hash_table_free(engine->storage);
    // Libérer le moteur
    free(engine);
}

/* ============================================
   FONCTION HELPER: CRÉER UN RÉSULTAT
   ============================================
   Crée une chaîne de caractères formatée.
   
   Exemple: create_result("(integer) %d", 1)
   → Retourne "(integer) 1"
   
   IMPORTANT: L'appelant doit libérer le résultat avec free()
*/
char* create_result(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    
    va_end(args);
    return strdup(buffer);  // Dupliquer pour retourner
}

/* ============================================
   COMMANDE SET
   ============================================
   
   Syntaxe: SET clé valeur
   
   Exemple: SET nom "Alice"
   
   Étapes:
   1. Vérifier les arguments (minimum 3: SET, clé, valeur)
   2. Appeler hash_table_set() pour stocker
   3. Retourner 1 si nouvelle insertion, 0 si mise à jour
   
   Complexité: O(1) en moyenne
*/
char* cmd_set(ARedisEngine* engine, char** parts, int argc) {
    // Vérifier le nombre d'arguments
    if (argc < 3) {
        return strdup("(error) ERR wrong number of arguments for 'set' command");
    }
    
    char* key = parts[1];   // "nom"
    char* value = parts[2]; // "Alice"
    
    // Insérer dans la table de hachage
    int is_new = hash_table_set(engine->storage, key, value, STRING_TYPE);
    
    // Incrémenter le compteur
    engine->commands_executed++;
    
    // Retourner le résultat
    return create_result("(integer) %d", is_new ? 1 : 0);
}

/* ============================================
   COMMANDE GET
   ============================================
   
   Syntaxe: GET clé
   
   Exemple: GET nom
   
   Étapes:
   1. Vérifier les arguments (minimum 2: GET, clé)
   2. Appeler hash_table_get() pour récupérer
   3. Retourner la valeur, ou (nil) si non trouvé
   
   Complexité: O(1) en moyenne
*/
char* cmd_get(ARedisEngine* engine, char** parts, int argc) {
    if (argc < 2) {
        return strdup("(error) ERR wrong number of arguments for 'get' command");
    }
    
    char* key = parts[1];
    Entry* entry = hash_table_get(engine->storage, key);
    engine->commands_executed++;
    
    if (!entry) {
        return strdup("(nil)");  // Clé non trouvée
    }
    
    // Vérifier le type (doit être STRING_TYPE)
    if (entry->data_type != STRING_TYPE) {
        return strdup("(error) ERR Operation against a key holding the wrong kind of value");
    }
    
    // Retourner la valeur entre guillemets
    return create_result("\"%s\"", entry->value.string.value);
}

/* ============================================
   COMMANDE DEL
   ============================================
   
   Syntaxe: DEL clé
   
   Exemple: DEL nom
   
   Étapes:
   1. Vérifier les arguments
   2. Appeler hash_table_delete()
   3. Retourner 1 si supprimé, 0 si non trouvé
*/
char* cmd_del(ARedisEngine* engine, char** parts, int argc) {
    if (argc < 2) {
        return strdup("(error) ERR wrong number of arguments for 'del' command");
    }
    
    char* key = parts[1];
    int deleted = hash_table_delete(engine->storage, key);
    engine->commands_executed++;
    
    return create_result("(integer) %d", deleted ? 1 : 0);
}

/* ============================================
   COMMANDE LPUSH
   ============================================
   
   Syntaxe: LPUSH clé valeur
   
   Exemple: LPUSH taches "faire_courses"
   
   Étapes:
   1. Vérifier les arguments
   2. Chercher la clé dans la table
   3. Si n'existe pas: créer une nouvelle liste
   4. Si existe mais pas une liste: erreur
   5. Ajouter en tête de liste
   6. Retourner la nouvelle taille
   
   Complexité: O(1)
*/
char* cmd_lpush(ARedisEngine* engine, char** parts, int argc) {
    if (argc < 3) {
        return strdup("(error) ERR wrong number of arguments for 'lpush' command");
    }
    
    char* key = parts[1];
    char* value = parts[2];
    
    // Chercher la clé
    Entry* entry = hash_table_get(engine->storage, key);
    
    if (!entry) {
        // Clé n'existe pas: créer une nouvelle liste
        hash_table_set(engine->storage, key, value, LIST_TYPE);
        entry = hash_table_get(engine->storage, key);
    } else if (entry->data_type != LIST_TYPE) {
        // Clé existe mais ce n'est pas une liste
        return strdup("(error) ERR Operation against a key holding the wrong kind of value");
    }
    
    // Ajouter en tête
    list_push_left(&entry->value.list, value);
    engine->commands_executed++;
    
    // Retourner la taille
    return create_result("(integer) %zu", list_size(&entry->value.list));
}

/* ============================================
   COMMANDE RPUSH
   ============================================
   
   Syntaxe: RPUSH clé valeur
   
   Exemple: RPUSH taches "envoyer_rapport"
   
   Même logique que LPUSH mais ajoute en queue.
   Complexité: O(1)
*/
char* cmd_rpush(ARedisEngine* engine, char** parts, int argc) {
    if (argc < 3) {
        return strdup("(error) ERR wrong number of arguments for 'rpush' command");
    }
    
    char* key = parts[1];
    char* value = parts[2];
    
    Entry* entry = hash_table_get(engine->storage, key);
    
    if (!entry) {
        hash_table_set(engine->storage, key, value, LIST_TYPE);
        entry = hash_table_get(engine->storage, key);
    } else if (entry->data_type != LIST_TYPE) {
        return strdup("(error) ERR Operation against a key holding the wrong kind of value");
    }
    
    list_push_right(&entry->value.list, value);
    engine->commands_executed++;
    
    return create_result("(integer) %zu", list_size(&entry->value.list));
}

/* ============================================
   COMMANDE LPOP
   ============================================
   
   Syntaxe: LPOP clé
   
   Exemple: LPOP taches
   
   Étapes:
   1. Chercher la clé
   2. Vérifier que c'est une liste
   3. Retirer le premier élément
   4. Si liste vide: supprimer la clé
   5. Retourner la valeur
   
   Complexité: O(1)
*/
char* cmd_lpop(ARedisEngine* engine, char** parts, int argc) {
    if (argc < 2) {
        return strdup("(error) ERR wrong number of arguments for 'lpop' command");
    }
    
    char* key = parts[1];
    Entry* entry = hash_table_get(engine->storage, key);
    engine->commands_executed++;
    
    if (!entry) {
        return strdup("(nil)");
    }
    
    if (entry->data_type != LIST_TYPE) {
        return strdup("(error) ERR Operation against a key holding the wrong kind of value");
    }
    
    // Retirer de la tête
    char* value = list_pop_left(&entry->value.list);
    
    if (!value) {
        // Liste vide: supprimer la clé
        hash_table_delete(engine->storage, key);
        return strdup("(nil)");
    }
    
    // Créer le résultat et libérer la valeur
    char* result = create_result("\"%s\"", value);
    free(value);
    return result;
}

/* ============================================
   COMMANDE RPOP
   ============================================
   
   Même logique que LPOP mais retire de la queue.
   Complexité: O(1)
*/
char* cmd_rpop(ARedisEngine* engine, char** parts, int argc) {
    if (argc < 2) {
        return strdup("(error) ERR wrong number of arguments for 'rpop' command");
    }
    
    char* key = parts[1];
    Entry* entry = hash_table_get(engine->storage, key);
    engine->commands_executed++;
    
    if (!entry) {
        return strdup("(nil)");
    }
    
    if (entry->data_type != LIST_TYPE) {
        return strdup("(error) ERR Operation against a key holding the wrong kind of value");
    }
    
    char* value = list_pop_right(&entry->value.list);
    
    if (!value) {
        hash_table_delete(engine->storage, key);
        return strdup("(nil)");
    }
    
    char* result = create_result("\"%s\"", value);
    free(value);
    return result;
}

/* ============================================
   COMMANDE EXISTS
   ============================================
   
   Syntaxe: EXISTS clé
   
   Exemple: EXISTS nom
   
   Retourne 1 si la clé existe, 0 sinon.
*/
char* cmd_exists(ARedisEngine* engine, char** parts, int argc) {
    if (argc < 2) {
        return strdup("(error) ERR wrong number of arguments for 'exists' command");
    }
    
    char* key = parts[1];
    int exists = hash_table_exists(engine->storage, key);
    engine->commands_executed++;
    
    return create_result("(integer) %d", exists ? 1 : 0);
}

/* ============================================
   COMMANDE KEYS
   ============================================
   
   Syntaxe: KEYS
   
   Retourne toutes les clés de la base de données.
   
   Étapes:
   1. Appeler hash_table_keys() pour obtenir toutes les clés
   2. Les formater avec des guillemets et retours à la ligne
   3. Retourner la chaîne
*/
char* cmd_keys(ARedisEngine* engine, char** parts, int argc) {
    int count = 0;
    char** keys = hash_table_keys(engine->storage, &count);
    engine->commands_executed++;
    
    if (!keys || count == 0) {
        free(keys);
        return strdup("(empty list)");
    }
    
    // Allouer de la mémoire pour le résultat
    char* result = (char*)malloc(1024 * count);
    if (!result) {
        for (int i = 0; i < count; i++) free(keys[i]);
        free(keys);
        return strdup("(error) ERR memory allocation failed");
    }
    
    // Construire la chaîne de résultat
    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        strcat(result, "\"");
        strcat(result, keys[i]);
        strcat(result, "\"");
        if (i < count - 1) strcat(result, "\n");
        free(keys[i]);
    }
    free(keys);
    
    return result;
}

/* ============================================
   COMMANDE DBSIZE
   ============================================
   
   Syntaxe: DBSIZE
   
   Retourne le nombre de clés dans la base de données.
*/
char* cmd_dbsize(ARedisEngine* engine, char** parts, int argc) {
    int size = hash_table_size(engine->storage);
    engine->commands_executed++;
    
    return create_result("(integer) %d", size);
}

/* ============================================
   COMMANDE FLUSHALL
   ============================================
   
   Syntaxe: FLUSHALL
   
   Efface toutes les données de la base de données.
   (La table de hachage reste, mais elle est vide)
*/
char* cmd_flushall(ARedisEngine* engine, char** parts, int argc) {
    hash_table_clear(engine->storage);
    engine->commands_executed++;
    
    return strdup("OK");
}

/* ============================================
   COMMANDE PING
   ============================================
   
   Syntaxe: PING
   
   Retourne "PONG" pour tester la connexion.
*/
char* cmd_ping(ARedisEngine* engine, char** parts, int argc) {
    engine->commands_executed++;
    return strdup("PONG");
}

/* ============================================
   PARSER DE COMMANDE
   ============================================
   
   Transforme une chaîne de commande en tableau de tokens.
   
   Exemple: "SET nom Alice" → ["SET", "nom", "Alice"]
   
   Utilise strtok() pour découper sur les espaces/tabulations.
*/
int parse_command(char* line, char** parts, int max_parts) {
    int count = 0;
    char* token = strtok(line, " \t\n\r");
    
    while (token && count < max_parts) {
        parts[count++] = token;
        token = strtok(NULL, " \t\n\r");
    }
    
    return count;
}

/* ============================================
   EXÉCUTION DE COMMANDE (FONCTION PRINCIPALE)
   ============================================
   
   C'est la fonction principale qui:
   1. Reçoit une commande (ex: "SET nom Alice")
   2. La parse en tokens
   3. Appelle la fonction appropriée
   4. Retourne le résultat
   
   C'est le "chef d'orchestre" du système.
*/
char* engine_execute(ARedisEngine* engine, const char* command) {
    if (!engine || !command) {
        return strdup("(error) ERR invalid command");
    }
    
    // Copier la commande (car strtok modifie la chaîne)
    char* cmd_copy = strdup(command);
    if (!cmd_copy) {
        return strdup("(error) ERR memory allocation failed");
    }
    
    // Parser la commande
    char* parts[64];
    int argc = parse_command(cmd_copy, parts, 64);
    
    if (argc == 0) {
        free(cmd_copy);
        return strdup("(error) ERR empty command");
    }
    
    char* result = NULL;
    char* cmd_upper = parts[0];
    
    // Convertir en majuscules pour comparaison
    for (int i = 0; cmd_upper[i]; i++) {
        cmd_upper[i] = toupper(cmd_upper[i]);
    }
    
    // Appeler la fonction appropriée
    if (strcmp(cmd_upper, "SET") == 0) {
        result = cmd_set(engine, parts, argc);
    } else if (strcmp(cmd_upper, "GET") == 0) {
        result = cmd_get(engine, parts, argc);
    } else if (strcmp(cmd_upper, "DEL") == 0) {
        result = cmd_del(engine, parts, argc);
    } else if (strcmp(cmd_upper, "LPUSH") == 0) {
        result = cmd_lpush(engine, parts, argc);
    } else if (strcmp(cmd_upper, "RPUSH") == 0) {
        result = cmd_rpush(engine, parts, argc);
    } else if (strcmp(cmd_upper, "LPOP") == 0) {
        result = cmd_lpop(engine, parts, argc);
    } else if (strcmp(cmd_upper, "RPOP") == 0) {
        result = cmd_rpop(engine, parts, argc);
    } else if (strcmp(cmd_upper, "EXISTS") == 0) {
        result = cmd_exists(engine, parts, argc);
    } else if (strcmp(cmd_upper, "KEYS") == 0) {
        result = cmd_keys(engine, parts, argc);
    } else if (strcmp(cmd_upper, "DBSIZE") == 0) {
        result = cmd_dbsize(engine, parts, argc);
    } else if (strcmp(cmd_upper, "FLUSHALL") == 0) {
        result = cmd_flushall(engine, parts, argc);
    } else if (strcmp(cmd_upper, "PING") == 0) {
        result = cmd_ping(engine, parts, argc);
    } else {
        result = create_result("(error) ERR unknown command '%s'", cmd_upper);
    }
    
    free(cmd_copy);
    return result;
}

/* ============================================
   STATISTIQUES
   ============================================
   Retourne les statistiques du moteur.
*/
void engine_get_stats(ARedisEngine* engine, int* total_keys, int* commands_executed, 
                      int* hash_size, double* load_factor) {
    if (!engine) return;
    
    if (total_keys) *total_keys = hash_table_size(engine->storage);
    if (commands_executed) *commands_executed = engine->commands_executed;
    if (hash_size) *hash_size = engine->storage->size;
    if (load_factor) {
        *load_factor = engine->storage->count / (double)engine->storage->size;
    }
}

/* ============================================
   OBTENIR TOUTES LES DONNÉES
   ============================================
   Retourne toutes les clés pour l'affichage.
*/
void engine_get_all_data(ARedisEngine* engine, char*** keys, int* count) {
    if (!engine || !keys || !count) return;
    
    *keys = hash_table_keys(engine->storage, count);
}

/* ============================================
   FLUSHALL
   ============================================
   Efface toutes les données.
*/
void engine_flushall(ARedisEngine* engine) {
    if (!engine) return;
    hash_table_clear(engine->storage);
}