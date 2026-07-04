# Guide pour publier sur GitHub

## 📋 Étapes pour mettre le projet sur GitHub

### 1. Créer un repository sur GitHub

1. Aller sur https://github.com/new
2. Nom du repository: `α-Redis` (ou `a-Redis`)
3. Description: `Moteur de stockage NoSQL en C avec interface console et web`
4. Choisir "Public"
5. **NE PAS** cocher "Initialize this repository with a README"
6. Cliquer sur "Create repository"

### 2. Lier le repository local à GitHub

```bash
# Ajouter le remote (remplacer USERNAME par votre nom d'utilisateur GitHub)
git remote add origin https://github.com/ibrahimagaye/α-Redis.git

# Vérifier le remote
git remote -v
```

### 3. Pousser le code sur GitHub

```bash
# Renommer la branche en main (optionnel, GitHub utilise "main" par défaut)
git branch -M main

# Pousser le code
git push -u origin main
```

### 4. Vérifier sur GitHub

Aller sur https://github.com/ibrahimagaye/α-Redis pour voir votre projet!

## 🔑 Authentification

### Option 1: HTTPS avec Personal Access Token (recommandé)

1. Aller sur GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Générer un nouveau token avec les permissions `repo`
3. Copier le token
4. Quand Git demande le mot de passe, utiliser le token

### Option 2: SSH

1. Générer une clé SSH: `ssh-keygen -t ed25519 -C "ibrahima.gaye.upa@gmail.com"`
2. Ajouter la clé publique à GitHub
3. Utiliser l'URL SSH: `git@github.com:ibrahimagaye/α-Redis.git`

## 📝 Commandes complètes (copier-coller)

```bash
# 1. Ajouter le remote
git remote add origin https://github.com/ibrahimagaye/α-Redis.git

# 2. Renommer la branche
git branch -M main

# 3. Pousser
git push -u origin main
```

## ✅ Vérification

Après le push, vérifier:
- Le repository est visible sur GitHub
- Tous les fichiers sont présents
- Le README s'affiche correctement
- Le badge de license apparaît

## 🎉 Félicitations!

Votre projet est maintenant sur GitHub et peut être partagé avec le professeur!