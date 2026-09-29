# Compilation Windows sans installation locale

1. Cree un nouveau depot GitHub vide.
2. Envoie **tout le contenu de ce dossier** dans le depot (y compris `.github`).
3. Important : `extern/godot-cpp` est un submodule. Si tu importes le ZIP via le site GitHub, le submodule ne sera pas automatiquement initialise. Le plus simple est d'utiliser le depot Multi-Mouse d'origine puis d'y appliquer ce ZIP, ou d'ajouter le submodule `extern/godot-cpp` avec l'URL de `.gitmodules`.
4. Dans GitHub : **Actions** -> **Build Windows Lightgun DLLs** -> **Run workflow**.
5. A la fin, ouvre l'execution et telecharge l'artifact `multi-mouse-lightgun-windows-x86_64`.
6. Copie les deux DLL dans ton projet Godot : `addons/multi_mouse/bin/win64/` en remplacant les anciennes.

Aucune installation Visual Studio/CMake/SCons n'est necessaire sur ton PC : GitHub Actions compile sur Windows Server 2022.
