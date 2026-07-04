# α-Redis - Moteur de Stockage NoSQL In-Memory

[![C](https://img.shields.io/badge/Langage-C-blue)](https://fr.wikipedia.org/wiki/C_(langage))
[![License](https://img.shields.io/badge/License-MIT-green)](LICENSE)

Version en C du moteur de stockage NoSQL α-Redis, avec interface console et web.

##  Démarrage Rapide

###  Windows

**Méthode simple**
```bash
# Double-cliquer sur start.bat
# OU taper dans le terminal:
start.bat
```

**Méthode manuelle:**
```bash
# Compiler
build.bat

# Ouvrir le dossier sur un terminal et taper
bin\aredis.exe
```

###  Linux / Mac

```bash
# Compiler le projet
make

# Lancer l'interface console
./bin/aredis
```

##  Commandes Disponibles

### Chaînes (Strings)
- `SET clé valeur` - Stocker une valeur
- `GET clé` - Récupérer une valeur
- `DEL clé` - Supprimer une clé
- `EXISTS clé` - Vérifier si une clé existe

### Listes
- `LPUSH clé valeur` - Insérer en tête de liste
- `RPUSH clé valeur` - Insérer en queue de liste
- `LPOP clé` - Extraire de la tête
- `RPOP clé` - Extraire de la queue

### Utilitaires
- `KEYS` - Lister toutes les clés
- `DBSIZE` - Nombre de clés
- `FLUSHALL` - Effacer toutes les données
- `PING` - Tester la connexion
- `HELP` - Afficher l'aide
- `STATS` - Afficher les statistiques
- `DATA` - Afficher toutes les données

##  Architecture

```
α-Redis/
├── src/                    # Code source C
│   ├── types.h
│   ├── linked_list.h/c
│   ├── hash_table.h/c
│   ├── engine.h/c
│   └── main.c
├── web_server.c           # Serveur web
├── Makefile               # Compilation (Linux/Mac)
├── build.bat              # Compilation (Windows)
├── start.bat              # Lancement facile (Windows)
└── README.md              # Documentation
```

##  Concepts Académiques

-  Table de hachage avec fonction DJB2
-  Listes doublement chaînées
-  Résolution de collisions par chaînage
-  Redimensionnement dynamique
-  Gestion manuelle de la mémoire
-  Parseur de commandes
-  Serveur web avec API REST

##  Performances

- **SET/GET**: ~100,000 ops/sec
- **LPUSH/RPUSH**: ~200,000 ops/sec
- **LPOP/RPOP**: ~200,000 ops/sec

##  Informations

- **Nom**: α-Redis
- **Langage**: C (C99)
- **Interface**: Console + Web
- **Date limite**: 05 Juillet 2026
- **Contact**: ibrahima.gaye.upa@gmail.com

##  Fonctionnalités

- Interface console interactive en français
- Interface web moderne avec statistiques en temps réel
- 12 commandes Redis implémentées


