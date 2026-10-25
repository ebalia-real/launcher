# 1.2.4 (2026-10-25)

- Les skins locaux fonctionnent en 1.21.9 et 26.x : ces versions nomment autrement le format des packs de ressources, et le launcher les refusait comme incompatibles. Le pack du skin suit désormais le format de chaque version, de 1.6 à 26.x.
- Skins → Appliquer localement à toutes les instances : le skin devient le skin local de toutes les instances, même avant d’en créer une. Les instances installées le reçoivent tout de suite, les nouvelles dès leur installation, et celles qui sont ouvertes ou en cours d’installation à leur démarrage. Appliquer localement à une instance donne toujours son propre skin à une seule instance, et Restaurer l’apparence locale propose d’abord toutes les instances.
- Sous NixOS, les fenêtres de Minecraft 26.x ont leur barre de titre et leurs bordures sous GNOME (Wayland).

# 1.2.3 (2026-10-20)

- Les Versions perdues se téléchargent depuis la publication d’EBALIA, et leurs archives ne contiennent plus les journaux et rapports de plantage laissés par d’autres launchers. Celles que vous avez déjà installées fonctionnent toujours ; la prochaine que vous installez retélécharge son archive une fois.

# 1.2.2 (2026-10-18)

- Découvrir des mods cherche pendant la saisie (mods, packs de ressources et shaders, ainsi que les modpacks à la création d’une instance) : les résultats arrivent dès que vous faites une pause, sans appuyer sur Entrée, qui lance toujours la recherche aussitôt. Quand une recherche est impossible (hors ligne, sans instance), la liste l’indique au lieu d’une fenêtre.
- La recherche dans Découvrir des mods ne saccade plus : les images se téléchargent et passent par leur cache dans un fil à part, la liste se redessine une seule fois par recherche, et la page complète, la galerie et la couverture d’un mod se chargent quand le choix reste immobile et que vous arrêtez de taper.
- Les collaborations d’EBALIA (comme Neighbor.jar) n’apparaissent plus comme ses propres mods : un nouveau mod d’EBALIA avec d’autres auteurs rejoint les collaborations, avec le crédit de ces auteurs, sur le site et dans le launcher. L’Accueil ne montre que ses propres mods.
- La fenêtre EBALIA Mods est nouvelle : les mods en solo et les collaborations à part, comme sur le site, dans des cartes comme celles de l’Accueil avec l’image, les téléchargements, qui l’a fait, la description dans votre langue et de petits boutons CurseForge et Modrinth ; en haut, le total des téléchargements ; Patreon avec ce qu’il apporte. L’invitation Patreon a le même style.
- Bedrock : Jouer est à côté de la version au lieu de traverser la carte, les actions partagent une ligne et l’aide est une ligne de liens.

# 1.2.1 (2026-10-18)

- L’avis de mise à jour montre les nouveautés : les notes de chaque version depuis la vôtre jusqu’à celle proposée, dans la langue du launcher, avec Mettre à jour et redémarrer ou Plus tard en dessous.
- Changements : une section de la barre latérale (et la version affichée en dessous) liste chaque version avec sa date, de la plus récente à la plus ancienne, et montre ce qui a changé dans celle que vous choisissez, dans la langue du launcher ; la recherche garde les versions qui mentionnent un mot et le met en évidence. La vôtre est aussi signalée.
- Les champs de recherche (le filtre du journal, Découvrir des mods, les instances et les versions perdues) gardent leur loupe centrée entre le bord et le texte.
- Nouvelles icônes dans la barre latérale : du pixel art dans le style du jeu, dessiné pour EBALIA : un bloc d’herbe pour l’Accueil, une pioche pour les instances, une boussole, un fantôme pour les versions perdues, un coffre pour Mes packs, un joueur pour les skins, une feuille de papier pour les actualités, deux joueurs pour la communauté, un livre pour le guide, un bloc de bedrock, une horloge pour les changements et une enclume pour les paramètres.
- L’Accueil présente le site officiel d’EBALIA, ebalia.gitgud.site : sa carte d’aperçu dans votre langue, ce qu’on y trouve et un bouton pour l’ouvrir.
- Corrigé : en changeant le fond de l’Accueil avec les flèches, l’image affichée sautait à la place de la suivante pendant son chargement, comme une bande en travers de la bannière.
- La pioche des instances est celle de Minetest Game (CC BY-SA 3.0).
- Les paramètres ouvrent le dossier où se trouve le launcher (où il a été installé, ou où vous avez mis la copie portable) et l’affichent.
- Désinstaller, dans les paramètres : le launcher se ferme et se retire (avec le programme de désinstallation sous Windows ; d’un dossier portable ou décompressé, seulement les fichiers qu’il liste, le reste est conservé ; le fichier AppImage ou l’app macOS) et, si vous cochez la case, vos données aussi. Les copies installées par le système (Nix, paquets) se retirent avec son gestionnaire de paquets.

# 1.2.0 (2026-10-15)

- Utilise environ deux fois moins de mémoire : les images se chargent à la taille où elles s’affichent plutôt qu’en taille réelle (les fonds de l’Accueil ne sont plus tous lus à l’avance, les images des actualités et les cartes des versions perdues sont petites, les icônes de la fenêtre des mods sont gardées en 64 pixels), les caches d’images sont plus petits, et réduire le launcher (comme pendant une partie) libère les images gardées pour plus tard. Sous Linux, la mémoire libérée retourne au système.
- Les versions perdues n’installent que leur jeu. Certains paquets de l’archive avaient été conservés avec le launcher qui les lançait (UltimMC, BetaCraft) et avec d’autres versions ; EBALIA lance le jeu lui-même, donc tout cela n’est plus installé : NSSS occupe 4 Mo au lieu de 700, Alpha 1.0.16 et Minecraft a1.1.1 se passent d’UltimMC et de BetaCraft. Les versions installées auparavant sont rangées à leur prochain lancement et gardent leurs mondes, options et journal.
- Les versions perdues montrent leurs propres captures, prises dans chaque version : la grande image, une bande pour en choisir une autre et une visionneuse en taille réelle. Chacune dit ce qu’elle est (une description plus longue dans chaque langue), ce que le jeu affiche comme version, s’il s’agit d’une recréation ou d’une version sauvée par EBALIA, qui l’a faite, sa taille une fois installée et à télécharger, et avec quoi elle fonctionne. Les cartes de la liste montrent les mêmes captures.
- Corrigé : Minecraft 1.Li02 se fermait avec une erreur à la création d’un monde (son écran Create New World ne préparait jamais le joueur). EBALIA le lance maintenant avec cet écran corrigé, sans modifier les fichiers du jeu ; les mondes en Survie et en Extrême fonctionnent.
- Le journal en direct fait partie de la page de l’instance, sous les captures et les serveurs, et suit le jeu pendant qu’il tourne. Plein écran lui donne toute la page (le bouton du journal en haut l’ouvre ainsi) ; Échap ou le lien de retour ramènent en arrière.

# 1.1.9 (2026-10-13)

- Plus rapide : lire les icônes des mods, les images et descriptions des packs, trouver les packs sur Modrinth et les têtes des skins se fait sur quelques threads d’arrière-plan dédiés, et le résultat est gardé pour la session. Avant, ouvrir la fenêtre des mods occupait tous les threads du processeur pendant quelques secondes et tout ce que vous faisiez ensuite (installer, chercher, retirer, ouvrir une autre fenêtre) attendait derrière ; maintenant, cela commence tout de suite. Rouvrir une fenêtre, retirer un mod ou le désactiver ne relit plus tous les fichiers.
- Fermer le launcher l’arrête aussitôt : les téléchargements et recherches encore en cours s’arrêtent au lieu de garder le processus en vie pendant des minutes sur une connexion lente.
- Les images d’Internet sont décodées hors du thread de la fenêtre, le cache des images téléchargées a une limite, et la couverture de la page du mod est mise à l’échelle une fois par taille ; les images sont nettes sur les écrans mis à l’échelle.
- Découvrir des mods indique ce que l’instance a déjà : les mods, packs de ressources et shaders installés apparaissent comme « Déjà installé » dans la liste, le bouton Installer le dit aussi, et en installer plusieurs les laisse de côté. Les packs sont enregistrés à l’installation ; ceux ajoutés à la main ou avant sont reconnus via Modrinth. Un pack ou un mod retiré de son dossier peut être réinstallé.
- Les images des mods sont nettes : la visionneuse de la galerie et la couverture de la page du mod montrent les images telles qu’elles ont été envoyées au lieu des aperçus de 350 pixels de Modrinth (l’aperçu s’affiche pendant le téléchargement de l’original), et la bande de CurseForge utilise ses petits aperçus.
- Les listes déroulantes (Mods, Packs de ressources, Shaders et tout autre choix) s’ouvrent sous leur case avec une seule bordure ; sous Windows, des parties de la liste étaient dessinées à l’extérieur.
- Les cartes Packs de ressources et Shaders n’ont qu’un lien, comme Mods : leur fenêtre contient le bouton de téléchargement.
- Les shaders, et les packs de ressources sans image, montrent l’icône de leur projet Modrinth (trouvé grâce au fichier et gardé en cache). Les boutons Utiliser ce shader et Activer gardent le même espace entre icône et texte que les autres.
- Journal en direct : la fenêtre du journal montre la sortie du jeu au fur et à mesure qu’elle s’écrit, la suit sauf si vous remontez, colore les erreurs en rouge et les avertissements en jaune, filtre les lignes, copie tout et ne bloque plus le launcher derrière elle. Les versions perdues ont aussi un bouton de journal.
- Versions perdues : Désinstaller se trouve à côté de Jouer et Réinstaller dès que la version est installée.
- Les téléchargements affichent leur pourcentage (et la taille dans l’infobulle) : les versions perdues et leur Java, et l’installation des instances à côté de ce qui est en cours.
- Bedrock cherche le jeu une fois, puis de nouveau seulement après une action faite depuis le launcher (installer, mettre à jour, désinstaller, ouvrir le Store ou Minecraft Linux) ou avec Vérifier à nouveau, au lieu de chaque ouverture de la page.

# 1.1.8 (2026-10-13)

- Corrigé : les boutons Installer et Ouvrir la page de la page du mod fermaient le launcher sur tous les systèmes (ce que pouvaient aussi faire les boutons « Installer <mod> » et « Télécharger le pilote » du résumé de plantage).
- Découvrir des mods a un seul bouton Installer : il installe chaque entrée choisie (Ctrl ou Maj pour en choisir plusieurs) et dit combien ; les mods sans version pour l’instance sont listés pendant que le reste s’installe.
- Packs de ressources et shaders de CurseForge et Modrinth : choisissez Mods, Packs de ressources ou Shaders dans Découvrir des mods, ou appuyez sur Télécharger sur ces cartes d’une instance. Les shaders proposent Iris (Fabric, Quilt, NeoForge) ou Oculus (Forge) quand l’instance n’a aucun des deux. Les instances vanilla peuvent recevoir des packs de ressources.
- La page du mod montre sa galerie : des miniatures, et les images en grand avec ◀ ▶ et les flèches du clavier.
- L’icône du mod est centrée et ne touche plus le bord du panneau quand le mod n’a pas d’image de bannière.
- Le menu de l’icône d’instance montre toujours les mêmes images que les instances (il gardait celles fournies après la lecture des textures du jeu).
- Les images des comptes montrent la tête du skin que porte le joueur (Microsoft) ou qu’il reçoit par défaut (sans premium) au lieu de l’image Xbox ; une image que vous avez choisie reste prioritaire. La liste des skins montre la tête de chaque skin.
- Les packs de ressources, les shaders et les mondes ouvrent leur propre fenêtre depuis la page de l’instance (cliquez n’importe où sur la carte), comme les mods : chaque pack avec son image et sa description, activer ou désactiver les packs de ressources et choisir le shader que charge le jeu, ajouter des ZIP, retirer (gardés dans removed-… à l’intérieur de l’instance), ouvrir le dossier et en télécharger d’autres. Mondes : ouvrir le dossier d’un monde, l’exporter en ZIP, importer un monde en ZIP. La fenêtre des mods ouvre aussi son dossier.
- La page Skins montre le skin sur un modèle 3D : faites-le glisser pour le tourner, double-cliquez pour le revoir de face, Face / dos lui fait faire un demi-tour. La vue à plat est à un bouton.
- Sous GNOME et les autres bureaux Wayland qui laissent la barre de titre à l’application, EBALIA dessine la sienne : double-cliquez pour agrandir ou restaurer, faites-la glisser pour déplacer la fenêtre et redimensionnez par les bords. Windows, macOS, X11, KDE et les autres gardent la barre de titre du système, où le double-clic le fait déjà. EBALIA_TITLE_BAR=system ou custom force l’une ou l’autre.
- Les skins peuvent être supprimés de la bibliothèque (Supprimer le skin, ou la touche Suppr) ; le PNG d’où ils ont été importés est conservé.
- Guide et tutoriel : les étapes du guide sont des cartes côte à côte, toutes visibles sans défiler, et les questions et réponses ont leur propre onglet.
- Silence parle pendant la visite interactive : le texte apparaît lettre par lettre avec de petits sons de voix, un carillon marque chaque étape et un petit air la fin. Le bouton haut-parleur de la visite la rend muette (c’est mémorisé). Les sons sont créés par le launcher lui-même et joués par le système (PlaySound sous Windows, sons système sous macOS, PulseAudio ou PipeWire sous Linux) : aucun fichier son et aucune nouvelle bibliothèque.
- Traductions : la note sur le skin par défaut des profils sans premium était en anglais dans sept langues ; la visite nomme désormais les Versions perdues comme la barre latérale dans chaque langue ; les infobulles de la barre de titre suivent un changement de langue.

# 1.1.7 (2026-10-12)

- Bedrock sous Linux et macOS passe par le mode profil de Minecraft Linux lui-même, qui vérifie l’achat sur Google Play et charge son module de compatibilité : la version que vous choisissez dans EBALIA démarre comme depuis son bouton Jouer. L’import d’APK et le lancement direct du jeu disparaissent (ils contournaient la vérification d’achat).
- Bedrock liste les versions téléchargées pour choisir, garde Minecraft Linux à jour (Flathub sous Linux ; l’app Apple Silicon vérifiée sous macOS 15+) et dit pourquoi un jeu s’est fermé (achat non confirmé, version incompatible, composants manquants).
- Bedrock sous Windows : Minecraft et Minecraft Preview au choix, un bouton Game Pass (app Xbox), une vérification des Gaming Services, un avertissement dans les machines virtuelles sans carte graphique, et une désinstallation qui copie d’abord les mondes depuis l’endroit où les versions actuelles les gardent.
- Aide pour se connecter à Xbox (Remote Connect sous Linux et macOS).
- Créer un profil sans premium avant d’avoir téléchargé une version du jeu pouvait figer le launcher s’il avait été ouvert depuis un terminal (un chemin de fichier de jeu vide faisait attendre une saisie au clavier au lecteur d’archives).

# 1.1.6 (2026-10-12)

- Découvrir des mods : la page du mod lit les descriptions comme Modrinth (Markdown avec du HTML dedans, grâce à md4c), si bien que le texte, les tableaux, les titres et les images s’affichent ; les images se mettent en place au fil du téléchargement sans que la page saute, et la couverture montre l’image entière au lieu d’un recadrage agrandi.
- Le double-clic installe même pendant que la liste charge encore (cela démarre dès qu’elle a fini) ; le panneau de la page est toujours visible, donc la liste ne bouge plus sous la souris.
- Thème personnalisé : un aperçu en direct d’un morceau du launcher dans les couleurs choisies.
- Accueil en plein écran : la bannière grandit avec la fenêtre au lieu de se réduire à une bande.

# 1.1.5 (2026-10-11)

- Bedrock sans autre launcher : sous Windows, EBALIA détecte Minecraft pour Windows et le lance directement (ou ouvre sa page sur le Microsoft Store) ; sous Linux, il installe l’environnement de Minecraft Linux, importe l’APK de Minecraft que vous avez acheté et lance le jeu lui-même. Nouveau bouton Désinstaller (sous Windows, les mondes sont copiés d’abord ; sous Linux, les mondes restent sauf si vous choisissez de les supprimer).
- Découvrir des mods : un aperçu du mod sélectionné (image, description, téléchargements), double-clic pour installer, uniquement les mods pour la version de l’instance, et plus de barre de chargement quand d’autres résultats se chargent en fin de liste.
- Deux mods qui demandent des versions différentes d’une même dépendance (souvent Fabric API) n’arrêtent plus l’installation : la plus récente est gardée.
- Tout est traduit : les textes récents, les questions et réponses, la visite et les messages d’installation, de téléchargement, de Java, de skins, de Bedrock et de mise à jour, dans les dix langues.
- La molette de la souris ne change jamais une liste déroulante (comme la langue), même après avoir cliqué dessus.

# 1.1.4 (2026-10-11)

- Découvrir des mods charge les mods populaires dès que l’onglet s’ouvre (et quand l’instance ou le fournisseur change) ; Entrée lance la recherche même si une autre tâche est en cours, et arriver en bas de la liste charge d’autres résultats.
- Thème personnalisé : choisissez vos propres couleurs d’accent et de fond dans Paramètres → Thème → Personnalisé.
- La molette de la souris fait défiler la page : les listes déroulantes (comme la langue) ne changent qu’après un clic.
- Le launcher ne devient plus gris derrière une boîte de dialogue ouverte ; il ignore simplement les clics jusqu’à sa fermeture.
- Changer de thème est instantané : seules les couleurs sont réappliquées au lieu de reconstruire toutes les pages.
- Les mods montrent leurs propres icônes dans la fenêtre des mods, pour tous les chargeurs (Fabric, Quilt, Forge, NeoForge, l’ancien mcmod.info) ; les mods sans icône reçoivent celle de Modrinth.
- Skins montre le skin que vous portez : le skin actuel du compte Microsoft ou, pour les profils sans premium, le skin par défaut que Minecraft leur donne.
- Les versions perdues utilisent aussi des textures de Minecraft (bloc d’herbe, citrouille-lanterne pour les versions d’horreur, établi).
- Nouvelle instance : Personnalisé montre le bloc d’herbe de Minecraft et Importer une icône d’import épurée.
- Serveurs sur la page de l’instance : un bouton Modifier, les noms complets des serveurs (sans les codes couleur de Minecraft), et les titres Serveurs et Captures d’écran restent en haut quand les listes sont vides.

# 1.1.3 (2026-10-08)

- Thèmes : EBALIA (vert), Abîmes (bleu-vert), Nether (rouge), End (violet), Océan (bleu) et Cerisier (rose), dans les Paramètres, appliqués aussitôt.
- Questions et réponses dans Guide et tutoriel : importer des packs, plantages, acheter Minecraft, mods, Java, machines virtuelles, où sont les mondes, mises à jour, utilisation portable, icônes et images, versions perdues et alertes d’antivirus.
- Les couvertures d’EBALIA Mods sur l’Accueil utilisent des illustrations sombres (Nether, Deep Dark, puits de mine).

# 1.1.2 (2026-10-08)

- Les plantages du pilote graphique et l’absence d’OpenGL montrent votre carte graphique (dédiée ou intégrée au processeur) et ouvrent son pilote : la page exacte du pilote pour les cartes NVIDIA GeForce et AMD Radeon RX, les pages d’Intel pour Arc et les graphismes de 7e à 10e génération, et l’outil de détection automatique du fabricant (NVIDIA App, AMD Auto-Detect, Intel Driver & Support Assistant) quand la page exacte est introuvable. Les ordinateurs portables et PC de marque reçoivent aussi la page d’assistance de leur fabricant (Dell, HP, Lenovo, ASUS, Acer, MSI, GIGABYTE, Samsung).
- Bedrock : le launcher installe le runtime Microsoft Visual C++ dont le gestionnaire Bedrock a besoin (il s’arrêtait avec « You need VC++ Runtime 14.14… »), et une fois le gestionnaire installé, un bouton Jouer le lance.
- Les icônes d’instance utilisent les propres textures de Minecraft (pioche en diamant, TNT, établi, creeper…), lues dans le jeu que le launcher a téléchargé.
- Choisissez votre propre photo de profil depuis le menu du compte (et revenez à celle de Microsoft quand vous voulez).
- L’Accueil montre EBALIA Mods avec des couvertures, au-dessus des actualités.
- Une page Patreon plus claire : carte d’abonnement, publications ordonnées avec des dates lisibles et « Voir plus », dans Communauté et Actualités.
- Le réglage de l’adresse du service Patreon a été retiré ; plus de bande vide sous les pages ; « Aide et communauté » affiche son « et ».

# 1.1.1 (2026-10-07)

- Machines virtuelles et ordinateurs sans pilote graphique : quand Minecraft se ferme faute d’OpenGL, le launcher propose de le lancer avec un rendu logiciel (OpenGL dessiné par le processeur ; Mesa sous Windows, le Mesa du système sous Linux). On peut aussi l’activer dans les paramètres de l’instance. Windows l’utilise pour Minecraft 1.13 et plus récent.
- Téléchargements du jeu plus rapides : les connexions aux serveurs de Mojang sont réutilisées et les bibliothèques se téléchargent en parallèle. Un Minecraft 1.21.1 neuf (3 900 fichiers, 850 Mo) est passé de 143 à 28 secondes lors de notre test.
- Fonds d’instance : choisissez n’importe quelle image du launcher ou une de votre ordinateur, depuis la page de l’instance ou ses paramètres.
- L’icône de l’instance peut aussi se changer depuis les paramètres de l’instance.
- Quand Minecraft se ferme sur une erreur, un résumé dit ce qui a échoué avec un bouton pour le corriger : un mod manquant (installé depuis Modrinth pour la version et le chargeur de l’instance), un plantage du pilote graphique (lien vers le pilote AMD, NVIDIA ou Intel, ou rendu logiciel), l’absence d’OpenGL, le mauvais Java ou trop peu de mémoire. Le journal complet est à un clic.
- Importer des dossiers .minecraft : celui du Minecraft Launcher officiel et les dossiers de version des launchers qui utilisent son format (TLauncher, SKLauncher…), compressés ou non. La version de Minecraft et le chargeur viennent du profil de version ou, à défaut, des mods eux-mêmes. Ces dossiers apparaissent aussi dans la liste des instances trouvées sur l’ordinateur, prêts à être importés. Les fichiers de compte ne sont jamais copiés.
- Les dossiers des instances portent le nom de l’instance au lieu d’un numéro aléatoire ; renommer une instance renomme son dossier. Les instances des versions précédentes sont renommées au démarrage et continuent de fonctionner (un dossier en cours d’utilisation garde son ancien nom).
- Collez des instances d’autres launchers directement dans le dossier des instances (Prism Launcher, MultiMC, app CurseForge, FTB App, ATLauncher, GDLauncher, Modrinth App, TLauncher ou dossiers de version de .minecraft, fichiers .zip et .mrpack) : le launcher en fait des instances avec leur version de Minecraft et leur chargeur, même pendant qu’il est ouvert. L’original ne va dans la corbeille des instances que si cela a fonctionné.
- Chaque import vérifie que ses mods correspondent à la version de Minecraft et au chargeur du pack et nomme ceux qui ne correspondent pas.

# 1.1.0 (2026-10-05)

- La connexion Microsoft est activée avec l’identifiant public d’application d’EBALIA (`microsoft-client-id.txt`).
- Les comptes Microsoft montrent maintenant leur photo de profil (l’image de joueur Xbox du compte ; le visage du skin Minecraft quand il n’y en a pas) dans la barre latérale, la carte du joueur de l’Accueil, le menu du compte et les Paramètres. Les images sont stockées dans `mc/avatars`.
- Silence, la mascotte d’EBALIA, accompagne la visite interactive et la page Guide et tutoriel, animée : elle flotte, respire, cligne des yeux, a les yeux qui brillent, balance un couteau, fait un petit saut à chaque étape et se penche vers le contrôle mis en évidence.
- Mode portable : un `portable.txt` à côté du launcher (à côté de l’AppImage ou du bundle `.app`) garde les instances, les comptes et les paramètres dans un dossier `ebalia-data` à côté.
- Silence a maintenant une planche de sprites en pixel art avec sept poses (au repos, joyeuse, surprise, clin d’œil, montrant du doigt, en fête, pensive), chacune avec des images pour parler et cligner des yeux ; chaque étape de la visite utilise sa propre pose.
- Nouvelles icônes d’instance de 16×16 dessinées à la main. N’importe quelle image de votre ordinateur peut servir d’icône d’instance, à la création de l’instance ou plus tard depuis sa page.
- La page de l’instance montre ses captures (clic pour ouvrir) et sa liste de serveurs multijoueur, où l’on peut ajouter, copier et retirer des serveurs ; les champs inconnus de `servers.dat` sont conservés.
- Passe de traduction : la visite interactive, les pages Patreon et créations, la désinstallation des Versions perdues, les messages de connexion Microsoft et le système de mise à jour du launcher sont traduits dans les dix langues.
- Site web : les téléchargements de l’installateur et de la version portable côte à côte, le SHA-256 de chaque paquet avec un bouton pour le copier et la commande pour le vérifier, et une mise en page pour téléphone refaite où chaque lien reste visible.
- Nouveaux paquets : AppImage Linux pour toute distribution avec glibc 2.35+ (Qt, libarchive et OpenSSL inclus ; testée sous Debian, Fedora, Arch et openSUSE en CI) avec mises à jour dans l’app, `.zip` portable Windows, `.tar.gz` portable Linux, et versions macOS Intel en plus d’Apple Silicon, chacune avec un `.zip` portable.
- Correction des instances Forge et NeoForge : les bibliothèques du chargeur sont fusionnées par artefact et classificateur, si bien que Forge garde son jar universel (« Failed to find system mod: forge »). Vérifié en jeu avec Forge 26.3, 1.20.1, 1.16.5, 1.12.2, 1.8.9 et 1.7.10, NeoForge 26.3 et 1.21.1, Fabric et Quilt 26.3.
- Les instances Forge, NeoForge, Fabric et Quilt installées avec un launcher précédent sont de nouveau préparées la prochaine fois que vous appuyez sur Jouer (les fichiers téléchargés sont réutilisés).
- NeoForge 26.x et NeoForge pour 1.20.1 sont disponibles ; Fabric et Quilt prennent le chargeur stable le plus récent ; Forge 1.7.10–1.12.2 s’installe avec son ancien installateur.
- Les Versions perdues se lancent avec Java directement sous Windows, macOS et Linux (sans Wine) : a1.1.1, Alpha 1.0.16, 1.1 Patch 1, Ghost, 554e, NSSS et toutes les autres versions archivées atteignent leurs menus. Les archives sont vérifiées avec SHA-256 et chaque version garde ses sauvegardes dans son propre dossier.
- Import de modpacks depuis `.mrpack`, `.zip` CurseForge, Prism Launcher / MultiMC / PolyMC, app CurseForge, FTB App, ATLauncher, GDLauncher, Modrinth App et Technic, avec Forge, NeoForge, Fabric, Quilt ou vanilla. Les ZIP enveloppés dans des dossiers ou faits sous macOS et Windows, les noms de fichiers dans toutes les langues, les URL miroirs et fichiers optionnels de Modrinth, et les packs de ressources et shaders de CurseForge sont pris en charge.
- Quand une nouvelle version est disponible, un bouton « Mettre à jour vers … » apparaît dans la barre latérale ; il télécharge, vérifie et installe la mise à jour sans passer par le site web.
- Ajout d’un gestionnaire Bedrock externe expérimental avec installation en arrière-plan. Windows utilise BedrockLauncher ; Linux et macOS utilisent le moteur Android et exigent l’achat sur Google Play. L’achat sur Windows ne vaut pas pour Android. Le jeu réel reste non vérifié sur toutes les plateformes.
- Ajout de la désinstallation des Versions perdues avec confirmation et mise à la corbeille.
- La progression des opérations est passée dans la barre d’état du bas.
- Le runtime Visual C++ de Windows est inclus et l’installateur par utilisateur ajoute un raccourci sur le bureau.
- Ajout de Mettre à jour et redémarrer dans le launcher. Les paquets de chaque version sont vérifiés avec SHA-256, préparés et testés au démarrage avant de remplacer les fichiers du programme installé. Les anciens fichiers du launcher et les téléchargements temporaires sont supprimés après un démarrage réussi. NixOS compile la version avec Nix ; les chemins partagés du store restent soumis au ramasse-miettes de Nix.
- Le système de mise à jour ne supprime jamais les instances, Versions perdues, mondes, mods ni contenus téléchargés. Les mises à jour sont bloquées pendant que des parties ou des installations sont en cours. Si le remplacement échoue, les anciens fichiers du programme sont restaurés.
- L’installateur Windows détecte le dossier de l’installation existante, y compris le format de registre de l’installateur précédent, et la met à jour sur place.
- Ajout d’une visite interactive de 14 étapes au premier lancement, avec contrôles mis en évidence, Retour/Suivant/Passer, et rejouable depuis Guide et tutoriel.
- Les personnes qui utilisent la 1.0.0 doivent installer la 1.1.0 une fois pour obtenir le système de mise à jour intégré ; les versions compatibles suivantes se mettent à jour depuis le launcher.
- L’emplacement existant des données de l’application est conservé.

Le launcher n’inclut ni Minecraft, ni comptes, ni licences du jeu. Les paquets Windows et macOS ne sont pas signés. Le binaire Linux vise Ubuntu 24.04 x64 ; les autres distributions peuvent compiler depuis les sources, y compris NixOS via le flake.
