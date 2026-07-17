## init
tabkioum@lmecxl1194:~/mbedtls$ git checkout v4.2.0
tabkioum@lmecxl1194:~/mbedtls$ git branch 
* (HEAD detached at v4.2.0)
  development
tabkioum@lmecxl1194:~/mbedtls$ 
tu ne te mets pas sur une branche, tu te mets directement sur un commit précis correspondant au tag v4.2.0.
branche = pointeur qui avance avec les commits
tag = image figée d’une version
HEAD = ce sur quoi tu travailles maintenant. Ici, HEAD pointe sur le tag, pas sur une branche

##
tabkioum@lmecxl1194:~/mbedtls$ git switch -c maho-dev
Switched to a new branch 'maho-dev'
tabkioum@lmecxl1194:~/mbedtls$ git branch
  development
* maho-dev
tabkioum@lmecxl1194:~/mbedtls$ 
Git a créé une nouvelle branche à partir de l’endroit où tu étais déjà, donc depuis le tag v4.2.0
vérification :
tabkioum@lmecxl1194:~/mbedtls$ git log --oneline -1
ece41aa84d (HEAD -> maho-dev, tag: v4.2.0, tag: mbedtls-4.2.0) ChangeLog: Removed CVE-ID
tabkioum@lmecxl1194:~/mbedtls$ git tag --points-at HEAD
mbedtls-4.2.0
v4.2.0
tabkioum@lmecxl1194:~/mbedtls$ git log --oneline --decorate -n 5
ece41aa84d (HEAD -> maho-dev, tag: v4.2.0, tag: mbedtls-4.2.0) ChangeLog: Removed CVE-ID
b4ee2d4277 Update BRANCHES.md
c1cc0ed3ed ChangeLog: Removed todo placeholder for CVE
7f64d9609b Changelog: Added reference to tf-psa-crypto update
f3c2896076 Assemble ChangeLog
tabkioum@lmecxl1194:~/mbedtls$ 


##
Et si tu veux vraiment partir d’une autre version ?
Tu dois te placer sur ce tag, puis créer une branche :
Exemple pour v4.1.1
git switch v4.1.1
git switch -c maho-dev-411
Exemple pour 3.6.6
git switch v3.6.6
git switch -c maho-dev-366

Option 1 — recréer la branche à partir de v4.2.0
Si tu n’as rien d’important dessus, le plus simple est :
git switch v4.2.0
git switch -C maho-dev
Important :
-c crée une branche
-C force la recréation de la branche existante en la déplaçant sur le commit courant
Donc -C est pratique si tu veux “repartir” la branche à zéro.

Option 2 — déplacer la branche avec git branch -f
Si maho-dev existe déjà et que tu veux la forcer sur v4.2.0 :
git branch -f maho-dev v4.2.0
git switch maho-dev

##
Questions pour option 2:
si par exemple au début j'avait fait  
git switch v4.1.1
git switch -c maho-dev
puis j'ai travaillé sur ma branch maho-dev et je voudrai garder mes projets notamment le code aes-ctr que tu m'as écrit et mes modifications et switcher vers le tag v4.2.0 est ce que je doit faire que 
git branch -f maho-dev v4.2.0
git switch maho-dev
est ce que c'est comme ca  ou y'as un merge conflict ?

Réponse :
la commande déplace bien la branche vers v4.2.0, ce n’est pas une opération de fusion
ce n’est pas une conservation automatique de toutes tes modifications que t'as fait 
et avant que tu fasse git swicth maho-dev il faut deja que j'ai poussé mes commits pour sauvgarder mes modifications et puis les récupérer
en faite quand tu fais :
git branch -f maho-dev v4.2.0
tu dis à Git : “Réécris l’étiquette maho-dev pour qu’elle pointe ici.”
Cela ne “fusionne” pas automatiquement ton ancien travail avec v4.2.0.


## Fork 

1) Ce que tu as actuellement
Tu es sur :

un dépôt cloné de Mbed TLS
une branche locale maho-dev
basée sur le tag v4.2.0
Donc localement, tu peux :
git add .
git commit -m "Mon changement"

2) Pour pousser vers ton GitHub
il faut généralement :
soit forker le projet
soit ajouter ton dépôt GitHub comme remote
Cas le plus courant : fork
Tu fais un fork de Mbed TLS sur GitHub, puis tu ajoutes ton fork comme remote, par exemple :

git remote add myfork https://github.com/TON_COMPTE/mbedtls.git
git push -u myfork maho-dev
Ça pousse ta branche vers ton dépôt personnel.

### fork deja fait mais le push marche pas (DNS block)

### token http
tabkioum@lmecxl1194:~/mbedtls$ git remote -v
origin	https://github.com/Mbed-TLS/mbedtls.git (fetch)
origin	https://github.com/Mbed-TLS/mbedtls.git (push)
tabkioum@lmecxl1194:~/mbedtls$ git remote add myfork https://github.com/mxvzzzz/mbedtls.git
tabkioum@lmecxl1194:~/mbedtls$ git remote -v
myfork	https://github.com/mxvzzzz/mbedtls.git (fetch)
myfork	https://github.com/mxvzzzz/mbedtls.git (push)
origin	https://github.com/Mbed-TLS/mbedtls.git (fetch)
origin	https://github.com/Mbed-TLS/mbedtls.git (push)
tabkioum@lmecxl1194:~/mbedtls$ git push -u myfork maho-dev
Username for 'https://github.com': mxvxzzz
Password for 'https://mxvxzzz@github.com': 
remote: Invalid username or token. Password authentication is not supported for Git operations.
fatal: Authentication failed for 'https://github.com/mxvzzzz/mbedtls.git/'
tabkioum@lmecxl1194:~/mbedtls$ 

## j'ai corrigé j'ai oublié x dans mxvxzzzz
tabkioum@lmecxl1194:~/mbedtls$ git remote -v
myfork  git@github.com:mxvzzzz/mbedtls.git (fetch)
myfork  git@github.com:mxvzzzz/mbedtls.git (push)
origin  https://github.com/Mbed-TLS/mbedtls.git (fetch)
origin  https://github.com/Mbed-TLS/mbedtls.git (push)
tabkioum@lmecxl1194:~/mbedtls$ git remote set-url myfork git@github.com:mxvxzzz/mbedtls.git
tabkioum@lmecxl1194:~/mbedtls$ git remote -v
myfork  git@github.com:mxvxzzz/mbedtls.git (fetch)
myfork  git@github.com:mxvxzzz/mbedtls.git (push)
origin  https://github.com/Mbed-TLS/mbedtls.git (fetch)
origin  https://github.com/Mbed-TLS/mbedtls.git (push)
tabkioum@lmecxl1194:~/mbedtls$ 

## encoe correction git utilise SSH il faut changer https:// et aussi le / au lieu :
tabkioum@lmecxl1194:~/mbedtls$ git remote set-url myfork https://github.com/mxvxzzz/mbedtls.git
tabkioum@lmecxl1194:~/mbedtls$ git push -u myfork maho-dev
Enumerating objects: 11, done.
Counting objects: 100% (11/11), done.
Delta compression using up to 8 threads
Compressing objects: 100% (10/10), done.
Writing objects: 100% (10/10), 6.95 KiB | 2.32 MiB/s, done.
Total 10 (delta 3), reused 0 (delta 0), pack-reused 0
remote: Resolving deltas: 100% (3/3), completed with 1 local object.
remote: 
remote: Create a pull request for 'maho-dev' on GitHub by visiting:
remote:      https://github.com/mxvxzzz/mbedtls/pull/new/maho-dev
remote: 
To https://github.com/mxvxzzz/mbedtls.git
 * [new branch]            maho-dev -> maho-dev
branch 'maho-dev' set up to track 'myfork/maho-dev'.
tabkioum@lmecxl1194:~/mbedtls$ 

### key SSH
tabkioum@lmecxl1194:~/mbedtls$ eval "$(ssh-agent -s)"
Agent pid XXXXXX
tabkioum@lmecxl1194:~/mbedtls$ ssh-add ~/.ssh/id_ed25519
Identity added: /local/home/tabkioum/.ssh/id_ed25519 (mohamedtabkioui@gmail.com)
tabkioum@lmecxl1194:~/mbedtls$ cat ~/.ssh/id_ed25519.pub
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
tabkioum@lmecxl1194:~/mbedtls$ ssh -T git@github.com
ssh: Could not resolve hostname ssh.github.com: Name or service not known