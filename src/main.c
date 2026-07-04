#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine.h"

#define MAX_COMMAND_LENGTH 1024
#define MAX_HISTORY 100

/* ============================================
   INTERFACE CONSOLE INTERACTIVE
   ============================================
   
   C'est l'interface utilisateur du programme.
   Elle permet de:
   - Afficher le prompt (α-redis>)
   - Lire les commandes de l'utilisateur
   - Afficher les résultats avec des couleurs
   - Gérer les commandes spéciales (HELP, STATS, etc.)
*/

/* ============================================
   AFFICHER LE PROMPT
   ============================================
   Affiche: α-redis> 
   Les codes ANSI \033[1;36m et \033[0m sont pour
   la couleur cyan (gras).
*/
void print_prompt() {
    printf("\033[1;36mα-redis>\033[0m ");
    fflush(stdout);  // Forcer l'affichage immédiat
}

/* ============================================
   AFFICHER L'AIDE
   ============================================
   Affiche toutes les commandes disponibles
   avec leur description.
*/
void print_help() {
    printf("\n\033[1;33m=== Commandes Disponibles ===\033[0m\n\n");
    
    printf("  \033[1;32mChaînes:\033[0m\n");
    printf("    SET clé valeur          - Stocker une valeur\n");
    printf("    GET clé                 - Récupérer une valeur\n");
    printf("    DEL clé                 - Supprimer une clé\n");
    printf("    EXISTS clé              - Vérifier si une clé existe\n\n");
    
    printf("  \033[1;32mListes:\033[0m\n");
    printf("    LPUSH clé valeur        - Insérer en tête de liste\n");
    printf("    RPUSH clé valeur        - Insérer en queue de liste\n");
    printf("    LPOP clé                - Extraire de la tête\n");
    printf("    RPOP clé                - Extraire de la queue\n\n");
    
    printf("  \033[1;32mUtilitaires:\033[0m\n");
    printf("    KEYS                    - Lister toutes les clés\n");
    printf("    DBSIZE                  - Nombre de clés\n");
    printf("    FLUSHALL                - Effacer toutes les données\n");
    printf("    PING                    - Tester la connexion\n");
    printf("    HELP                    - Afficher cette aide\n");
    printf("    EXIT / QUIT             - Quitter\n\n");
}

/* ============================================
   AFFICHER LES STATISTIQUES
   ============================================
   Affiche les statistiques du moteur:
   - Nombre de clés
   - Commandes exécutées
   - Taille de la table de hachage
   - Facteur de charge
*/
void print_stats(ARedisEngine* engine) {
    int total_keys, commands, hash_size;
    double load_factor;
    
    // Récupérer les statistiques
    engine_get_stats(engine, &total_keys, &commands, &hash_size, &load_factor);
    
    printf("\n\033[1;33m=== Statistiques du Moteur ===\033[0m\n");
    printf("  Clés totales:       %d\n", total_keys);
    printf("  Commandes exécutées: %d\n", commands);
    printf("  Taille table hash:  %d\n", hash_size);
    printf("  Facteur de charge:  %.2f%%\n", load_factor * 100);
    printf("\n");
}

/* ============================================
   AFFICHER TOUTES LES DONNÉES
   ============================================
   Affiche toutes les clés et leurs valeurs.
   
   Exemple d'affichage:
   nom (string): "Alice"
   taches (list): ["faire_courses", "envoyer_rapport"]
*/
void print_all_data(ARedisEngine* engine) {
    char** keys = NULL;
    int count = 0;
    
    // Récupérer toutes les clés
    engine_get_all_data(engine, &keys, &count);
    
    if (!keys || count == 0) {
        printf("\033[1;31m(empty database)\033[0m\n");
        free(keys);
        return;
    }
    
    printf("\n\033[1;33m=== Données Stockées ===\033[0m\n\n");
    
    // Parcourir et afficher chaque clé
    for (int i = 0; i < count; i++) {
        Entry* entry = hash_table_get(engine->storage, keys[i]);
        if (entry) {
            printf("  \033[1;36m%s\033[0m (\033[1;35m%s\033[0m): ", 
                   keys[i], 
                   entry->data_type == STRING_TYPE ? "string" : "list");
            
            if (entry->data_type == STRING_TYPE) {
                // Afficher une chaîne
                printf("\"%s\"\n", entry->value.string.value);
            } else if (entry->data_type == LIST_TYPE) {
                // Afficher une liste
                printf("[");
                ListNode* current = entry->value.list.head;
                int first = 1;
                while (current) {
                    if (!first) printf(", ");
                    printf("\"%s\"", current->value);
                    current = current->next;
                    first = 0;
                }
                printf("]\n");
            }
        }
        free(keys[i]);
    }
    free(keys);
    printf("\n");
}

/* ============================================
   EXÉCUTER ET AFFICHER UNE COMMANDE
   ============================================
   Exécute une commande et affiche le résultat
   avec la couleur appropriée:
   - Rouge pour les erreurs
   - Vert pour les succès
   - Blanc pour le reste
*/
void execute_and_print(ARedisEngine* engine, const char* command) {
    // Exécuter la commande
    char* result = engine_execute(engine, command);
    
    // Choisir la couleur selon le résultat
    if (strncmp(result, "(error)", 7) == 0) {
        // Erreur: rouge
        printf("\033[1;31m%s\033[0m\n", result);
    } else if (strncmp(result, "(integer)", 9) == 0 || 
               strncmp(result, "\"", 1) == 0 ||
               strcmp(result, "PONG") == 0 ||
               strcmp(result, "OK") == 0) {
        // Succès: vert
        printf("\033[1;32m%s\033[0m\n", result);
    } else {
        // Autre: blanc
        printf("%s\n", result);
    }
    
    free(result);  // Libérer le résultat
}

/* ============================================
   LIRE UNE COMMANDE
   ============================================
   Lit une ligne de commande depuis le clavier.
   
   Retourne: pointeur vers la chaîne lue, ou NULL si EOF
*/
char* read_command() {
    static char buffer[MAX_COMMAND_LENGTH];
    
    // Lire depuis le clavier
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return NULL;  // Fin de fichier (Ctrl+D)
    }
    
    // Supprimer le saut de ligne
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return buffer;
}

/* ============================================
   NETTOYER L'ÉCRAN
   ============================================
   Utilise les codes ANSI pour nettoyer l'écran.
   \033[2J = effacer tout
   \033[H  = replacer le curseur en haut
*/
void clear_screen() {
    printf("\033[2J\033[H");
}

/* ============================================
   FONCTION PRINCIPALE (MAIN)
   ============================================
   
   C'est le point d'entrée du programme.
   
   Étapes:
   1. Afficher le message de bienvenue
   2. Créer le moteur
   3. Boucle infinie:
      - Afficher le prompt
      - Lire la commande
      - Traiter la commande
      - Afficher le résultat
   4. Libérer les ressources et quitter
*/
/* ============================================
   FONCTION: INTERFACE CONSOLE
   ============================================
   Lance l'interface console interactive.
*/
void run_console_interface(ARedisEngine* engine) {
    char* command;
    
    printf("\nTapez \033[1;32mHELP\033[0m pour voir les commandes disponibles.\n");
    printf("Tapez \033[1;31mEXIT\033[0m ou \033[1;31mQUIT\033[0m pour quitter.\n\n");
    
    // ============================================
    // BOUCLE PRINCIPALE
    // ============================================
    while (1) {
        // 1. Afficher le prompt
        print_prompt();
        
        // 2. Lire la commande
        command = read_command();
        
        // Si Ctrl+D (fin de fichier), quitter
        if (!command) {
            printf("\n\n\033[1;33mAu revoir!\033[0m\n");
            break;
        }
        
        // Ignorer les lignes vides
        if (strlen(command) == 0) {
            continue;
        }
        
        // ============================================
        // COMMANDES SPÉCIALES
        // ============================================
        
        // EXIT / QUIT
        if (strcasecmp(command, "EXIT") == 0 || strcasecmp(command, "QUIT") == 0) {
            printf("\n\033[1;33mAu revoir!\033[0m\n");
            break;
        }
        
        // HELP
        if (strcasecmp(command, "HELP") == 0) {
            print_help();
            continue;
        }
        
        // CLEAR / CLS (nettoyer l'écran)
        if (strcasecmp(command, "CLEAR") == 0 || strcasecmp(command, "CLS") == 0) {
            clear_screen();
            continue;
        }
        
        // STATS (afficher les statistiques)
        if (strcasecmp(command, "STATS") == 0) {
            print_stats(engine);
            continue;
        }
        
        // DATA / SHOW (afficher toutes les données)
        if (strcasecmp(command, "DATA") == 0 || strcasecmp(command, "SHOW") == 0) {
            print_all_data(engine);
            continue;
        }
        
        // ============================================
        // EXÉCUTER LA COMMANDE REDIS
        // ============================================
        execute_and_print(engine, command);
    }
}

/* ============================================
   FONCTION PRINCIPALE (MAIN)
   ============================================
   
   C'est le point d'entrée du programme.
   
   Affiche un menu pour choisir entre:
   - Interface console
   - Interface web
*/
int main() {
    // Afficher le message de bienvenue
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                    ║\n");
    printf("║          \033[1;36mα-Redis\033[0m - Moteur de Stockage NoSQL In-Memory          ║\n");
    printf("║                                                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Créer le moteur (initialiser la base de données)
    ARedisEngine* engine = engine_create();
    if (!engine) {
        printf("\033[1;31mErreur: Impossible de créer le moteur\033[0m\n");
        return 1;  // Code d'erreur
    }
    
    // Menu de choix
    printf("Choisissez une interface:\n\n");
    printf("  1. Interface Console\n");
    printf("  2. Interface Web (http://localhost:8080)\n\n");
    printf("Votre choix (1 ou 2): ");
    
    char choice[10];
    if (fgets(choice, sizeof(choice), stdin) == NULL) {
        printf("\n\033[1;31mErreur de lecture\033[0m\n");
        engine_free(engine);
        return 1;
    }
    
    int mode = atoi(choice);
    
    if (mode == 2) {
        // Lancer le serveur web
        printf("\n\033[1;32mLancement du serveur web...\033[0m\n");
        int result = run_web_server(engine);
        engine_free(engine);
        return result;
    } else {
        // Lancer l'interface console (par défaut)
        printf("\n\033[1;32mLancement de l'interface console...\033[0m\n\n");
        run_console_interface(engine);
        engine_free(engine);
        return 0;
    }
}
