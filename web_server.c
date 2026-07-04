#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "src/engine.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 4096

/* ============================================
   SERVEUR WEB POUR α-Redis
   ============================================
   
   Ce serveur web permet d'accéder à α-Redis
   via un navigateur web.
   
   Fonctionnalités:
   - Interface web moderne en HTML/CSS/JS
   - API REST pour exécuter des commandes
   - Affichage en temps réel des résultats
*/

// Variable globale pour le moteur
ARedisEngine* global_engine = NULL;

/* ============================================
   EXÉCUTER UNE COMMANDE VIA L'API
   ============================================
   Reçoit une commande JSON et retourne le résultat.
*/
char* execute_command_api(const char* command) {
    if (!global_engine || !command) {
        return strdup("{\"error\": \"Invalid command\"}");
    }
    
    char* result = engine_execute(global_engine, command);
    
    // Créer une réponse JSON
    char* json_response = (char*)malloc(BUFFER_SIZE);
    if (!json_response) {
        free(result);
        return strdup("{\"error\": \"Memory allocation failed\"}");
    }
    
    // Échapper les guillemets dans le résultat
    char escaped[BUFFER_SIZE * 2];
    int j = 0;
    for (int i = 0; result[i] && j < BUFFER_SIZE * 2 - 2; i++) {
        if (result[i] == '"') {
            escaped[j++] = '\\';
            escaped[j++] = '"';
        } else if (result[i] == '\n') {
            escaped[j++] = '\\';
            escaped[j++] = 'n';
        } else {
            escaped[j++] = result[i];
        }
    }
    escaped[j] = '\0';
    
    // Créer le JSON
    snprintf(json_response, BUFFER_SIZE, 
             "{\"result\": \"%s\"}", escaped);
    
    free(result);
    return json_response;
}

/* ============================================
   GÉNÉRER LA PAGE HTML
   ============================================
   Retourne le code HTML de l'interface web.
*/
char* generate_html_page() {
    char* html = (char*)malloc(BUFFER_SIZE * 10);
    if (!html) return NULL;
    
    strcpy(html, 
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=utf-8\r\n"
"Connection: close\r\n"
"\r\n"
"<!DOCTYPE html>\n"
"<html lang=\"fr\">\n"
"<head>\n"
"    <meta charset=\"UTF-8\">\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"    <title>α-Redis - Interface Web</title>\n"
"    <style>\n"
"        * {\n"
"            margin: 0;\n"
"            padding: 0;\n"
"            box-sizing: border-box;\n"
"        }\n"
"        \n"
"        body {\n"
"            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n"
"            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
"            min-height: 100vh;\n"
"            display: flex;\n"
"            justify-content: center;\n"
"            align-items: center;\n"
"            padding: 20px;\n"
"        }\n"
"        \n"
"        .container {\n"
"            background: white;\n"
"            border-radius: 20px;\n"
"            box-shadow: 0 20px 60px rgba(0,0,0,0.3);\n"
"            max-width: 1000px;\n"
"            width: 100%;\n"
"            padding: 40px;\n"
"            animation: fadeIn 0.5s ease-in;\n"
"        }\n"
"        \n"
"        @keyframes fadeIn {\n"
"            from { opacity: 0; transform: translateY(-20px); }\n"
"            to { opacity: 1; transform: translateY(0); }\n"
"        }\n"
"        \n"
"        h1 {\n"
"            color: #667eea;\n"
"            text-align: center;\n"
"            margin-bottom: 10px;\n"
"            font-size: 3em;\n"
"            font-weight: bold;\n"
"            text-shadow: 2px 2px 4px rgba(0,0,0,0.1);\n"
"        }\n"
"        \n"
"        .subtitle {\n"
"            text-align: center;\n"
"            color: #666;\n"
"            margin-bottom: 40px;\n"
"            font-size: 1.1em;\n"
"        }\n"
"        \n"
"        .stats {\n"
"            display: grid;\n"
"            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));\n"
"            gap: 20px;\n"
"            margin-bottom: 30px;\n"
"        }\n"
"        \n"
"        .stat-card {\n"
"            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
"            color: white;\n"
"            padding: 25px;\n"
"            border-radius: 15px;\n"
"            text-align: center;\n"
"            box-shadow: 0 10px 25px rgba(102, 126, 234, 0.3);\n"
"            transition: transform 0.3s ease;\n"
"        }\n"
"        \n"
"        .stat-card:hover {\n"
"            transform: translateY(-5px);\n"
"        }\n"
"        \n"
"        .stat-value {\n"
"            font-size: 2.5em;\n"
"            font-weight: bold;\n"
"            margin-bottom: 5px;\n"
"        }\n"
"        \n"
"        .stat-label {\n"
"            font-size: 1em;\n"
"            opacity: 0.95;\n"
"            text-transform: uppercase;\n"
"            letter-spacing: 1px;\n"
"        }\n"
"        \n"
"        .command-section {\n"
"            background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);\n"
"            padding: 30px;\n"
"            border-radius: 15px;\n"
"            margin-bottom: 25px;\n"
"            box-shadow: 0 5px 15px rgba(0,0,0,0.1);\n"
"        }\n"
"        \n"
"        .command-section h2 {\n"
"            color: #667eea;\n"
"            margin-bottom: 20px;\n"
"            font-size: 1.8em;\n"
"        }\n"
"        \n"
"        .input-group {\n"
"            display: flex;\n"
"            gap: 10px;\n"
"            margin-bottom: 15px;\n"
"        }\n"
"        \n"
"        input[type=\"text\"] {\n"
"            flex: 1;\n"
"            padding: 15px;\n"
"            font-size: 16px;\n"
"            border: 2px solid #667eea;\n"
"            border-radius: 10px;\n"
"            font-family: 'Courier New', monospace;\n"
"            transition: all 0.3s ease;\n"
"        }\n"
"        \n"
"        input[type=\"text\"]:focus {\n"
"            outline: none;\n"
"            border-color: #764ba2;\n"
"            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);\n"
"        }\n"
"        \n"
"        button {\n"
"            padding: 15px 30px;\n"
"            font-size: 16px;\n"
"            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
"            color: white;\n"
"            border: none;\n"
"            border-radius: 10px;\n"
"            cursor: pointer;\n"
"            font-weight: bold;\n"
"            transition: all 0.3s ease;\n"
"            box-shadow: 0 4px 15px rgba(102, 126, 234, 0.3);\n"
"        }\n"
"        \n"
"        button:hover {\n"
"            transform: translateY(-2px);\n"
"            box-shadow: 0 6px 20px rgba(102, 126, 234, 0.4);\n"
"        }\n"
"        \n"
"        button:active {\n"
"            transform: translateY(0);\n"
"        }\n"
"        \n"
"        .result {\n"
"            background: #1e1e1e;\n"
"            color: #00ff00;\n"
"            padding: 20px;\n"
"            border-radius: 10px;\n"
"            margin-top: 15px;\n"
"            font-family: 'Courier New', monospace;\n"
"            min-height: 60px;\n"
"            white-space: pre-wrap;\n"
"            word-wrap: break-word;\n"
"            box-shadow: inset 0 2px 10px rgba(0,0,0,0.3);\n"
"            border: 1px solid #333;\n"
"        }\n"
"        \n"
"        .error {\n"
"            color: #ff6b6b;\n"
"            font-weight: bold;\n"
"        }\n"
"        \n"
"        .commands-help {\n"
"            background: linear-gradient(135deg, #fff9e6 0%, #ffe6cc 100%);\n"
"            padding: 25px;\n"
"            border-radius: 15px;\n"
"            margin-top: 25px;\n"
"            box-shadow: 0 5px 15px rgba(0,0,0,0.1);\n"
"        }\n"
"        \n"
"        .commands-help h3 {\n"
"            color: #667eea;\n"
"            margin-bottom: 15px;\n"
"            font-size: 1.5em;\n"
"        }\n"
"        \n"
"        .commands-help ul {\n"
"            list-style: none;\n"
"            padding-left: 0;\n"
"            display: grid;\n"
"            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));\n"
"            gap: 10px;\n"
"        }\n"
"        \n"
"        .commands-help li {\n"
"            padding: 8px;\n"
"            color: #333;\n"
"        }\n"
"        \n"
"        .commands-help code {\n"
"            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
"            color: white;\n"
"            padding: 4px 10px;\n"
"            border-radius: 5px;\n"
"            font-weight: bold;\n"
"            font-family: 'Courier New', monospace;\n"
"        }\n"
"        \n"
"        .footer {\n"
"            text-align: center;\n"
"            margin-top: 30px;\n"
"            color: #666;\n"
"            font-size: 0.9em;\n"
"        }\n"
"    </style>\n"
"</head>\n"
"<body>\n"
"    <div class=\"container\">\n"
"        <h1>α-Redis</h1>\n"
"        <p class=\"subtitle\">Moteur de Stockage NoSQL In-Memory - Interface Web</p>\n"
"        \n"
"        <div class=\"stats\">\n"
"            <div class=\"stat-card\">\n"
"                <div class=\"stat-value\" id=\"totalKeys\">0</div>\n"
"                <div class=\"stat-label\">Clés</div>\n"
"            </div>\n"
"            <div class=\"stat-card\">\n"
"                <div class=\"stat-value\" id=\"commands\">0</div>\n"
"                <div class=\"stat-label\">Commandes</div>\n"
"            </div>\n"
"            <div class=\"stat-card\">\n"
"                <div class=\"stat-value\" id=\"loadFactor\">0%</div>\n"
"                <div class=\"stat-label\">Charge</div>\n"
"            </div>\n"
"        </div>\n"
"        \n"
"        <div class=\"command-section\">\n"
"            <h2 style=\"color: #667eea; margin-bottom: 15px;\">Exécuter une commande</h2>\n"
"            <div class=\"input-group\">\n"
"                <input type=\"text\" id=\"commandInput\" placeholder=\"SET nom Alice\" onkeypress=\"if(event.key==='Enter') executeCommand()\">\n"
"                <button onclick=\"executeCommand()\">Exécuter</button>\n"
"            </div>\n"
"            <div class=\"result\" id=\"result\">Entrez une commande et cliquez sur Exécuter...</div>\n"
"        </div>\n"
"        \n"
"        <div class=\"commands-help\">\n"
"            <h3>📚 Commandes disponibles</h3>\n"
"            <ul>\n"
"                <li><code>SET clé valeur</code> - Stocker une valeur</li>\n"
"                <li><code>GET clé</code> - Récupérer une valeur</li>\n"
"                <li><code>DEL clé</code> - Supprimer une clé</li>\n"
"                <li><code>EXISTS clé</code> - Vérifier si une clé existe</li>\n"
"                <li><code>LPUSH clé valeur</code> - Ajouter en tête de liste</li>\n"
"                <li><code>RPUSH clé valeur</code> - Ajouter en queue de liste</li>\n"
"                <li><code>LPOP clé</code> - Extraire de la tête</li>\n"
"                <li><code>RPOP clé</code> - Extraire de la queue</li>\n"
"                <li><code>KEYS</code> - Lister toutes les clés</li>\n"
"                <li><code>DBSIZE</code> - Nombre de clés</li>\n"
"                <li><code>FLUSHALL</code> - Effacer toutes les données</li>\n"
"                <li><code>PING</code> - Tester la connexion</li>\n"
"            </ul>\n"
"        </div>\n"
"    </div>\n"
"    \n"
"    <script>\n"
"        async function executeCommand() {\n"
"            const input = document.getElementById('commandInput');\n"
"            const resultDiv = document.getElementById('result');\n"
"            const command = input.value.trim();\n"
"            \n"
"            if (!command) {\n"
"                resultDiv.innerHTML = '<span class=\"error\">Veuillez entrer une commande</span>';\n"
"                return;\n"
"            }\n"
"            \n"
"            resultDiv.textContent = 'Exécution...';\n"
"            \n"
"            try {\n"
"                const response = await fetch('/api', {\n"
"                    method: 'POST',\n"
"                    headers: {\n"
"                        'Content-Type': 'application/json',\n"
"                    },\n"
"                    body: JSON.stringify({command: command})\n"
"                });\n"
"                \n"
"                const data = await response.json();\n"
"                \n"
"                if (data.error) {\n"
"                    resultDiv.innerHTML = '<span class=\"error\">' + data.error + '</span>';\n"
"                } else {\n"
"                    // Formater le résultat\n"
"                    let formatted = data.result;\n"
"                    if (formatted.startsWith('(error)')) {\n"
"                        resultDiv.innerHTML = '<span class=\"error\">' + formatted + '</span>';\n"
"                    } else {\n"
"                        resultDiv.textContent = formatted;\n"
"                    }\n"
"                }\n"
"                \n"
"                // Mettre à jour les statistiques\n"
"                updateStats();\n"
"                \n"
"                // Vider le champ\n"
"                input.value = '';\n"
"            } catch (error) {\n"
"                resultDiv.innerHTML = '<span class=\"error\">Erreur: ' + error.message + '</span>';\n"
"            }\n"
"        }\n"
"        \n"
"        async function updateStats() {\n"
"            try {\n"
"                const response = await fetch('/stats');\n"
"                const data = await response.json();\n"
"                \n"
"                document.getElementById('totalKeys').textContent = data.total_keys;\n"
"                document.getElementById('commands').textContent = data.commands_executed;\n"
"                document.getElementById('loadFactor').textContent = data.load_factor.toFixed(2) + '%';\n"
"            } catch (error) {\n"
"                console.error('Erreur stats:', error);\n"
"            }\n"
"        }\n"
"        \n"
"        // Mettre à jour les stats toutes les 2 secondes\n"
"        setInterval(updateStats, 2000);\n"
"        \n"
"        // Charger les stats au démarrage\n"
"        window.onload = updateStats;\n"
"    </script>\n"
"</body>\n"
"</html>\n");
    
    return html;
}

/* ============================================
   GÉNÉRER LA RÉPONSE STATS (JSON)
   ============================================
*/
char* generate_stats_json() {
    if (!global_engine) {
        return strdup("{\"error\": \"Engine not initialized\"}");
    }
    
    int total_keys, commands, hash_size;
    double load_factor;
    
    engine_get_stats(global_engine, &total_keys, &commands, &hash_size, &load_factor);
    
    char* json = (char*)malloc(BUFFER_SIZE);
    if (!json) return strdup("{\"error\": \"Memory allocation failed\"}");
    
    snprintf(json, BUFFER_SIZE,
             "{\"total_keys\": %d, \"commands_executed\": %d, \"hash_size\": %d, \"load_factor\": %.4f}",
             total_keys, commands, hash_size, load_factor);
    
    return json;
}

/* ============================================
   TRAITER UNE REQUÊTE HTTP
   ============================================
*/
void handle_request(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytes_received <= 0) {
        closesocket(client_socket);
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    // Parser la requête HTTP
    char method[16], path[256];
    sscanf(buffer, "%s %s", method, path);
    
    // Route: /api - Exécuter une commande
    if (strcmp(path, "/api") == 0 && strcmp(method, "POST") == 0) {
        // Extraire le body de la requête
        char* body = strstr(buffer, "\r\n\r\n");
        if (body) {
            body += 4;  // Skip les headers
            
            // Chercher "command":"..."
            char* cmd_start = strstr(body, "\"command\":\"");
            if (cmd_start) {
                cmd_start += 11;  // Skip "command":"
                char* cmd_end = strchr(cmd_start, '"');
                if (cmd_end) {
                    *cmd_end = '\0';
                    
                    // Exécuter la commande
                    char* result = execute_command_api(cmd_start);
                    
                    // Envoyer la réponse
                    char response[BUFFER_SIZE * 2];
                    sprintf(response,
                            "HTTP/1.1 200 OK\r\n"
                            "Content-Type: application/json\r\n"
                            "Access-Control-Allow-Origin: *\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                            "%s", result);
                    
                    send(client_socket, response, strlen(response), 0);
                    free(result);
                }
            }
        }
    }
    // Route: /stats - Statistiques
    else if (strcmp(path, "/stats") == 0) {
        char* stats = generate_stats_json();
        
        char response[BUFFER_SIZE];
        sprintf(response,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Connection: close\r\n"
                "\r\n"
                "%s", stats);
        
        send(client_socket, response, strlen(response), 0);
        free(stats);
    }
    // Route: / - Page principale
    else {
        char* html = generate_html_page();
        if (html) {
            send(client_socket, html, strlen(html), 0);
            free(html);
        }
    }
    
    closesocket(client_socket);
}

/* ============================================
   FONCTION PRINCIPALE DU SERVEUR WEB
   ============================================
*/
int run_web_server(ARedisEngine* engine) {
    global_engine = engine;
    
    // Initialiser Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Erreur: Impossible d'initialiser Winsock\n");
        return 1;
    }
    
    // Créer le socket
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Erreur: Impossible de créer le socket\n");
        WSACleanup();
        return 1;
    }
    
    // Configurer l'adresse
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Erreur: Bind failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    
    // Listen
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Erreur: Listen failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                    ║\n");
    printf("║          \033[1;36mα-Redis\033[0m - Serveur Web Démarré!                      ║\n");
    printf("║                                                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("  🌐 Interface web: http://localhost:%d\n", PORT);
    printf("  📊 API: http://localhost:%d/api\n", PORT);
    printf("  📈 Stats: http://localhost:%d/stats\n", PORT);
    printf("\n");
    printf("  Appuyez sur Ctrl+C pour arrêter le serveur\n");
    printf("\n");
    
    // Boucle principale
    while (1) {
        SOCKET client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            continue;
        }
        
        handle_request(client_socket);
    }
    
    // Nettoyage (jamais atteint dans cette version)
    closesocket(server_socket);
    WSACleanup();
    
    return 0;
}