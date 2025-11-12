# 🎮 Casse-Briques (Breakout Game)

Un jeu de casse-briques classique développé en C++ avec SFML 3.0.2.

## 🎯 Description

Ce projet est un jeu de casse-briques (Breakout) complet avec :
- Raquette contrôlable
- Balle avec physique réaliste
- Grille de 50 briques colorées
- Système de score
- Détection de collision avancée
- Condition de victoire

## 🎮 Contrôles

- **A** ou **Flèche Gauche** : Déplacer la raquette à gauche
- **D** ou **Flèche Droite** : Déplacer la raquette à droite

## 🛠️ Technologies utilisées

- **C++17**
- **SFML 3.0.2** (Simple and Fast Multimedia Library)
- **Clang++** (Compilateur)

## 🚀 Installation et compilation

### Prérequis
- SFML 3.0.2 installé sur votre système
- Compilateur C++17 compatible

### Sur macOS avec Homebrew
```bash
# Installer SFML
brew install sfml

# Cloner le repository
git clone 
cd CasseBriques

# Compiler le projet
clang++ -std=c++17 main.cpp -o main -I/opt/homebrew/include -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system

# Lancer le jeu
./main
```

## 📝 Fonctionnalités implémentées

### ✅ Gameplay complet
- [x] Raquette contrôlable avec les touches A/D ou flèches
- [x] Balle avec rebonds réalistes
- [x] 50 briques colorées (5 rangées × 10 colonnes)
- [x] Système de score (10 points par brique)
- [x] Collision detection précise
- [x] Condition de victoire (toutes briques détruites)
- [x] Reset automatique si balle sort de l'écran

### 🎨 Graphismes
- Raquette blanche en bas de l'écran
- Balle jaune avec mouvement fluide
- Briques colorées par rangée :
  - Rangée 1 : Rouge
  - Rangée 2 : Vert
  - Rangée 3 : Bleu  
  - Rangée 4 : Jaune
  - Rangée 5 : Magenta

## 🏗️ Architecture du code

Le jeu est structuré autour d'une boucle principale avec :
- **Gestion d'événements** : Capture des touches et fermeture
- **Logique de jeu** : Mouvement, collisions, score
- **Rendu** : Affichage des éléments graphiques

### Concepts techniques utilisés
- Vecteurs 2D pour positions et vitesses
- Gestion du temps avec `deltaTime`
- Détection de collision AABB (Axis-Aligned Bounding Box)
- Conteneurs STL (`std::vector`) pour les briques
- Gestion d'événements SFML 3.0

## 👥 Contributeur

- **Aminata Constance Sané** - Développeuse principale

## 📄 Licence

Ce projet est développé pour un portfolio.

---

**Score maximum possible : 500 points** (50 briques × 10 points)