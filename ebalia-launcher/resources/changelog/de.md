# 1.2.3 (2026-10-20)

- Verschollene Versionen werden aus EBALIAs eigener Veröffentlichung geladen, und ihre Archive enthalten keine übrig gebliebenen Protokolle und Absturzberichte anderer Launcher mehr. Bereits installierte Versionen funktionieren weiter; die nächste, die du installierst, lädt ihr Archiv noch einmal herunter.

# 1.2.2 (2026-10-18)

- Mods entdecken sucht beim Tippen (Mods, Ressourcenpakete und Shader sowie Modpacks beim Erstellen einer Instanz): Die Ergebnisse kommen, sobald du kurz innehältst, ohne Enter, das weiterhin sofort sucht. Wenn eine Suche nicht möglich ist (offline, keine Instanz), sagt es die Liste statt eines Fensters.
- Die Suche in Mods entdecken ruckelt nicht mehr: Bilder werden in einem eigenen Thread heruntergeladen und zwischengespeichert, die Liste wird pro Suche einmal neu gezeichnet, und die ganze Seite, Galerie und das Titelbild eines Mods laden, sobald die Auswahl ruht und du nicht mehr tippst.
- Kooperationen von EBALIA (wie Neighbor.jar) erscheinen nicht mehr als eigene Mods: Ein neuer Mod von EBALIA mit weiteren Autoren kommt zu den Kooperationen, mit deren Namen, auf der Website und im Launcher. Start zeigt nur die eigenen Mods.
- Das Fenster EBALIA Mods ist neu: eigene Mods und Kooperationen getrennt, wie auf der Website, in Karten wie auf Start mit Bild, Downloads, wer es gemacht hat, Beschreibung in deiner Sprache und kleinen Schaltflächen für CurseForge und Modrinth; oben die Downloads insgesamt; Patreon mit dem, was es bietet. Die Patreon-Einladung sieht genauso aus.
- Bedrock: Spielen steht neben der Version statt quer über die Karte, die Aktionen teilen sich eine Zeile und die Hilfe ist eine Zeile mit Links.

# 1.2.1 (2026-10-18)

- Der Hinweis zur Aktualisierung zeigt, was neu ist: die Versionshinweise jeder Version seit deiner bis zur angebotenen, in der Sprache des Launchers, darunter Aktualisieren und neu starten oder Später.
- Änderungen: ein Bereich der Seitenleiste (und die Version darunter) listet jede Version mit Datum auf, die neueste zuerst, und zeigt, was sich in der gewählten geändert hat, in der Sprache des Launchers; die Suche behält die Versionen, die ein Wort erwähnen, und hebt es hervor. Deine Version ist ebenfalls markiert.
- Suchfelder (der Filter des Protokolls, Mods entdecken, Instanzen und verschollene Versionen) halten ihre Lupe mittig zwischen Rand und Text.
- Neue Symbole in der Seitenleiste: Pixelkunst im Stil des Spiels, für EBALIA gezeichnet: ein Grasblock für Start, eine Spitzhacke für Instanzen, ein Kompass, ein Geist für verschollene Versionen, eine Truhe für Meine Packs, ein Spieler für Skins, ein Blatt Papier für Neuigkeiten, zwei Spieler für Community, ein Buch für die Anleitung, ein Grundgesteinblock, eine Uhr für Änderungen und ein Amboss für Einstellungen.
- Start zeigt die offizielle Website von EBALIA, ebalia.gitgud.site: ihre Vorschaukarte in deiner Sprache, was es dort gibt, und eine Schaltfläche, die sie öffnet.
- Behoben: Beim Wechseln des Start-Hintergrunds mit den Pfeilen sprang das angezeigte Bild an die Stelle des nächsten, solange dieses lud, und wirkte wie ein Streifen quer durch das Banner.
- Die Spitzhacke von Instanzen stammt aus Minetest Game (CC BY-SA 3.0).
- Die Einstellungen öffnen den Ordner, in dem der Launcher liegt (wo er installiert wurde oder wo du die portable Kopie abgelegt hast), und zeigen ihn an.
- Deinstallieren, in den Einstellungen: Der Launcher schließt sich und entfernt sich (unter Windows mit dem Deinstallationsprogramm der Installation; aus einem portablen oder entpackten Ordner nur die Dateien, die er auflistet, alles andere bleibt; die AppImage-Datei oder die macOS-App) und, wenn du es ankreuzt, auch deine Daten. Vom System installierte Kopien (Nix, Pakete) werden mit dessen Paketverwaltung entfernt.

# 1.2.0 (2026-10-15)

- Braucht etwa halb so viel Arbeitsspeicher: Bilder werden in der Größe geladen, in der sie angezeigt werden, statt in voller Größe (die Hintergründe von Start werden nicht mehr alle im Voraus gelesen, Bilder der Neuigkeiten und Karten der verschollenen Versionen sind klein, Mod-Symbole im Mods-Fenster werden mit 64 Pixeln behalten), die Bildzwischenspeicher sind kleiner, und wenn du den Launcher minimierst (etwa beim Spielen), gibt er die für später behaltenen Bilder frei. Unter Linux geht freigegebener Speicher an das System zurück.
- Verschollene Versionen installieren nur ihr Spiel. Manche Archivpakete wurden zusammen mit dem Launcher gesichert, der sie ausführte (UltimMC, BetaCraft), und mit anderen Versionen; EBALIA startet das Spiel selbst, deshalb wird das nicht mehr installiert: NSSS belegt 4 MB statt 700, Alpha 1.0.16 und Minecraft a1.1.1 kommen ohne UltimMC und BetaCraft aus. Früher installierte Versionen werden beim nächsten Start aufgeräumt und behalten ihre Welten, Optionen und ihr Protokoll.
- Verschollene Versionen zeigen eigene Screenshots, in jeder Version aufgenommen: das große Bild, eine Leiste zur Auswahl eines anderen und eine Ansicht in voller Größe. Jede erzählt, was sie ist (eine längere Beschreibung in jeder Sprache), was das Spiel als Version anzeigt, ob sie ein Nachbau ist oder von EBALIA gerettet wurde, wer sie gemacht hat, ihre Größe installiert und zum Herunterladen und womit sie läuft. Die Karten der Liste zeigen dieselben Screenshots.
- Behoben: Minecraft 1.Li02 schloss sich beim Erstellen einer Welt mit einem Fehler (sein Bildschirm Create New World richtete den Spieler nie ein). EBALIA startet es jetzt mit korrigiertem Bildschirm, ohne die Dateien des Spiels zu ändern; Welten im Überlebens- und Hardcore-Modus funktionieren.
- Das Live-Protokoll ist Teil der Instanzseite, unter den Screenshots und Servern, und folgt dem Spiel, solange es läuft. Vollbild gibt ihm die ganze Seite (die Protokoll-Schaltfläche oben öffnet es so); Escape oder der Zurück-Link führen zurück.

# 1.1.9 (2026-10-13)

- Schneller: Mod-Symbole, Bilder und Beschreibungen von Paketen lesen, Pakete auf Modrinth finden und Skin-Köpfe laufen auf einigen eigenen Hintergrund-Threads und werden für die Sitzung gemerkt. Vorher belegte das Öffnen des Mods-Fensters jeden Prozessor-Thread für einige Sekunden, und alles, was du danach tatest (installieren, suchen, entfernen, ein anderes Fenster öffnen), wartete dahinter; jetzt beginnt es sofort. Ein Fenster erneut öffnen, einen Mod entfernen oder ausschalten liest nicht mehr jede Datei neu.
- Das Schließen des Launchers beendet ihn sofort: noch laufende Downloads und Abfragen stoppen, statt den Prozess bei langsamer Verbindung minutenlang am Leben zu halten.
- Bilder aus dem Internet werden außerhalb des Fenster-Threads dekodiert, der Zwischenspeicher heruntergeladener Bilder hat eine Grenze, und das Titelbild der Mod-Seite wird einmal pro Größe skaliert; auf skalierten Bildschirmen sind Bilder scharf.
- Mods entdecken zeigt, was die Instanz schon hat: installierte Mods, Ressourcenpakete und Shader erscheinen in der Liste als „Bereits installiert“, die Schaltfläche Installieren sagt es ebenfalls, und beim Installieren mehrerer werden sie ausgelassen. Pakete werden bei der Installation vermerkt; von Hand oder früher hinzugefügte werden über Modrinth erkannt. Ein aus seinem Ordner entferntes Paket oder Mod kann erneut installiert werden.
- Mod-Bilder sind scharf: die Galerieansicht und das Titelbild der Mod-Seite zeigen die Bilder wie hochgeladen statt der 350-Pixel-Vorschauen von Modrinth (die Vorschau erscheint, während das Original lädt), und die Leiste von CurseForge nutzt deren kleine Vorschauen.
- Aufklappbare Listen (Mods, Ressourcenpakete, Shader und jede andere Auswahl) öffnen sich unter ihrem Feld mit einem einzigen Rand; unter Windows wurden Teile der Liste außerhalb davon gezeichnet.
- Die Karten Ressourcenpakete und Shader haben einen einzigen Link, wie Mods: ihr Fenster enthält die Download-Schaltfläche.
- Shader und Ressourcenpakete ohne Bild zeigen das Symbol ihres Modrinth-Projekts (über die Datei gefunden und zwischengespeichert). Die Schaltflächen Diesen Shader verwenden und Aktivieren haben denselben Abstand zwischen Symbol und Text wie die anderen.
- Live-Protokoll: das Protokollfenster zeigt die Ausgabe des Spiels, während sie geschrieben wird, folgt ihr, außer du scrollst nach oben, färbt Fehler rot und Warnungen gelb, filtert Zeilen, kopiert alles und blockiert den Launcher dahinter nicht mehr. Verschollene Versionen haben ebenfalls eine Protokoll-Schaltfläche.
- Verschollene Versionen: Deinstallieren steht neben Spielen und Neu installieren, sobald die Version installiert ist.
- Downloads zeigen ihren Prozentsatz (und die Größe im Tooltip): verschollene Versionen und ihr Java sowie Instanzinstallationen neben dem, was gerade passiert.
- Bedrock sucht das Spiel einmal und erneut nur nach einer Aktion im Launcher (installieren, aktualisieren, deinstallieren, den Store oder Minecraft Linux öffnen) oder mit Erneut prüfen, statt bei jedem Öffnen der Seite.

# 1.1.8 (2026-10-13)

- Behoben: die Schaltflächen Installieren und Seite öffnen auf der Mod-Seite schlossen den Launcher auf jedem System (das konnten auch die Schaltflächen „<Mod> installieren“ und „Treiber herunterladen“ der Absturzübersicht).
- Mods entdecken hat eine einzige Schaltfläche Installieren: sie installiert jeden ausgewählten Eintrag (Strg oder Umschalt, um mehrere zu wählen) und sagt, wie viele; Mods ohne Version für die Instanz werden aufgelistet, während der Rest installiert wird.
- Ressourcenpakete und Shader von CurseForge und Modrinth: wähle Mods, Ressourcenpakete oder Shader in Mods entdecken, oder drücke auf Herunterladen auf diesen Karten einer Instanz. Shader bieten Iris (Fabric, Quilt, NeoForge) oder Oculus (Forge) an, wenn die Instanz keines davon hat. Vanilla-Instanzen können Ressourcenpakete bekommen.
- Die Mod-Seite zeigt die Galerie des Mods: Miniaturen und die Bilder groß mit ◀ ▶ und den Pfeiltasten.
- Das Symbol des Mods ist zentriert und berührt nicht mehr den Rand des Bereichs, wenn der Mod kein Titelbild hat.
- Das Menü für das Instanzsymbol zeigt immer dieselben Bilder wie die Instanzen (es behielt die mitgelieferten, nachdem die Texturen des Spiels gelesen waren).
- Kontobilder zeigen den Kopf des Skins, den der Spieler trägt (Microsoft) oder standardmäßig bekommt (ohne Premium), statt des Xbox-Bilds; ein Bild, das du gewählt hast, hat weiterhin Vorrang. Die Skins-Liste zeigt den Kopf jedes Skins.
- Ressourcenpakete, Shader und Welten öffnen ihr eigenes Fenster von der Instanzseite aus (Klick irgendwo auf die Karte), wie Mods: jedes Paket mit Bild und Beschreibung, Ressourcenpakete ein- oder ausschalten und den Shader wählen, den das Spiel lädt, ZIPs hinzufügen, entfernen (aufbewahrt in removed-… innerhalb der Instanz), den Ordner öffnen und mehr herunterladen. Welten: den Ordner einer Welt öffnen, sie als ZIP exportieren, ein Welt-ZIP importieren. Das Mods-Fenster öffnet ebenfalls seinen Ordner.
- Die Seite Skins zeigt den Skin an einem 3D-Modell: ziehen zum Drehen, Doppelklick, um ihn wieder von vorne zu sehen, Vorderseite / Rückseite dreht ihn halb herum. Die flache Ansicht ist eine Schaltfläche entfernt.
- Unter GNOME und anderen Wayland-Desktops, die die Titelleiste der App überlassen, zeichnet EBALIA seine eigene: Doppelklick zum Maximieren oder Wiederherstellen, ziehen, um das Fenster zu bewegen, und an den Rändern die Größe ändern. Windows, macOS, X11, KDE und die übrigen behalten die Titelleiste des Systems, wo ein Doppelklick das bereits tut. EBALIA_TITLE_BAR=system oder custom erzwingt das eine oder andere.
- Skins können aus der Bibliothek gelöscht werden (Skin löschen oder die Entf-Taste); das PNG, aus dem sie importiert wurden, bleibt erhalten.
- Anleitung: die Schritte der Anleitung sind Karten nebeneinander, alle ohne Scrollen sichtbar, und Fragen & Antworten haben ihren eigenen Tab.
- Silence spricht in der interaktiven Tour: der Text erscheint Buchstabe für Buchstabe mit kleinen Stimmgeräuschen, ein Klang markiert jeden Schritt und eine kurze Melodie das Ende. Die Lautsprecher-Schaltfläche der Tour schaltet sie stumm (wird gemerkt). Die Klänge erzeugt der Launcher selbst und spielt sie über das System ab (PlaySound unter Windows, Systemklänge unter macOS, PulseAudio oder PipeWire unter Linux): keine Sounddateien und keine neuen Bibliotheken.
- Übersetzungen: der Hinweis zum Standard-Skin von Profilen ohne Premium war in sieben Sprachen auf Englisch; die Tour nennt Verschollene Versionen jetzt wie die Seitenleiste in jeder Sprache; die Tooltips der Titelleiste folgen einem Sprachwechsel.

# 1.1.7 (2026-10-12)

- Bedrock unter Linux und macOS läuft über den Profilmodus von Minecraft Linux selbst, der den Kauf bei Google Play prüft und sein Kompatibilitätsmodul lädt: die Version, die du in EBALIA wählst, startet wie über dessen Schaltfläche Spielen. Der APK-Import und der direkte Spielstart entfallen (sie übersprangen die Kaufprüfung).
- Bedrock listet die heruntergeladenen Versionen zur Auswahl, hält Minecraft Linux aktuell (Flathub unter Linux; die geprüfte Apple-Silicon-App unter macOS 15+) und sagt, warum sich ein Spiel geschlossen hat (Kauf nicht bestätigt, inkompatible Version, fehlende Komponenten).
- Bedrock unter Windows: Minecraft und Minecraft Preview zur Auswahl, eine Game-Pass-Schaltfläche (Xbox-App), eine Prüfung der Gaming Services, eine Warnung in virtuellen Maschinen ohne Grafikkarte und eine Deinstallation, die zuerst die Welten von dort kopiert, wo aktuelle Versionen sie speichern.
- Hilfe bei der Anmeldung bei Xbox (Remote Connect unter Linux und macOS).
- Ein Profil ohne Premium zu erstellen, bevor eine Spielversion heruntergeladen war, konnte den Launcher einfrieren, wenn er aus einem Terminal geöffnet worden war (ein leerer Pfad zur Spieldatei ließ den Archivleser auf Tastatureingaben warten).

# 1.1.6 (2026-10-12)

- Mods entdecken: die Mod-Seite liest Beschreibungen wie Modrinth (Markdown mit HTML darin, über md4c), sodass Text, Tabellen, Überschriften und Bilder erscheinen; Bilder fügen sich beim Laden ein, ohne dass die Seite springt, und das Titelbild zeigt das ganze Bild statt eines vergrößerten Ausschnitts.
- Ein Doppelklick installiert, auch während die Liste noch lädt (es beginnt, sobald das endet); der Seitenbereich ist immer sichtbar, sodass sich die Liste nicht mehr unter der Maus verschiebt.
- Benutzerdefiniertes Design: eine Live-Vorschau eines Teils des Launchers in den gewählten Farben.
- Start im Vollbild: das Banner wächst mit dem Fenster, statt zu einem Streifen zu schrumpfen.

# 1.1.5 (2026-10-11)

- Bedrock ohne anderen Launcher: unter Windows erkennt EBALIA Minecraft für Windows und startet es direkt (oder öffnet seine Seite im Microsoft Store); unter Linux installiert es die Laufzeit von Minecraft Linux, importiert die Minecraft-APK, die du gekauft hast, und startet das Spiel selbst. Neue Schaltfläche Deinstallieren (unter Windows werden die Welten zuerst kopiert; unter Linux bleiben die Welten, außer du löschst sie).
- Mods entdecken: eine Vorschau des gewählten Mods (Bild, Beschreibung, Downloads), Doppelklick zum Installieren, nur Mods für die Version der Instanz und kein Ladebalken, wenn am Ende der Liste mehr Ergebnisse laden.
- Zwei Mods, die unterschiedliche Versionen derselben Abhängigkeit brauchen (oft Fabric API), stoppen die Installation nicht mehr: die neueste wird behalten.
- Alles übersetzt: die neueren Texte, Fragen & Antworten, die Tour und die Meldungen zu Installation, Download, Java, Skins, Bedrock und Aktualisierung, in allen zehn Sprachen.
- Das Mausrad ändert nie eine aufklappbare Liste (wie die Sprache), auch nicht nach einem Klick darauf.

# 1.1.4 (2026-10-11)

- Mods entdecken lädt beliebte Mods, sobald der Tab geöffnet wird (und wenn sich die Instanz oder der Anbieter ändert); Enter sucht, auch während eine andere Aufgabe läuft, und am Ende der Liste laden mehr Ergebnisse.
- Benutzerdefiniertes Design: wähle eigene Akzent- und Hintergrundfarben unter Einstellungen → Design → Benutzerdefiniert.
- Das Mausrad scrollt die Seite: aufklappbare Listen (wie die Sprache) ändern sich erst, nachdem du sie angeklickt hast.
- Der Launcher wird hinter einem offenen Dialog nicht mehr grau; er ignoriert nur Klicks, bis der Dialog geschlossen ist.
- Das Design zu wechseln geht sofort: nur die Farben werden neu angewendet, statt jede Seite neu aufzubauen.
- Mods zeigen im Mods-Fenster ihre eigenen Symbole, für jeden Loader (Fabric, Quilt, Forge, NeoForge, altes mcmod.info); Mods ohne eigenes bekommen ihr Modrinth-Symbol.
- Skins zeigt den Skin, den du trägst: den aktuellen Skin des Microsoft-Kontos oder bei Profilen ohne Premium den Standard-Skin, den Minecraft ihnen gibt.
- Verschollene Versionen nutzen ebenfalls Minecraft-Texturen (Grasblock, Kürbislaterne für Horror-Versionen, Werkbank).
- Neue Instanz: Benutzerdefiniert zeigt den Grasblock von Minecraft und Importieren ein klares Import-Symbol.
- Server auf der Instanzseite: eine Schaltfläche Bearbeiten, vollständige Servernamen (ohne die Farbcodes von Minecraft), und die Titel Server und Screenshots bleiben oben, wenn die Listen leer sind.

# 1.1.3 (2026-10-08)

- Designs: EBALIA (Grün), Tiefes Dunkel (Petrol), Nether (Rot), End (Lila), Ozean (Blau) und Kirschblüte (Rosa), in den Einstellungen, sofort angewendet.
- Fragen & Antworten in der Anleitung: Packs importieren, Abstürze, Minecraft kaufen, Mods, Java, virtuelle Maschinen, wo die Welten liegen, Aktualisierungen, portable Nutzung, Symbole und Bilder, verschollene Versionen und Warnungen von Antivirenprogrammen.
- Die Titelbilder von EBALIA Mods auf Start nutzen dunkle Motive (Nether, Deep Dark, verlassener Minenschacht).

# 1.1.2 (2026-10-08)

- Abstürze des Grafiktreibers und fehlendes OpenGL zeigen deine Grafikkarte (eigene oder im Prozessor) und öffnen ihren Treiber: die genaue Treiberseite für NVIDIA-GeForce- und AMD-Radeon-RX-Karten, Intels Seiten für Arc und Grafik der 7. bis 10. Generation und das automatische Erkennungstool des Herstellers (NVIDIA App, AMD Auto-Detect, Intel Driver & Support Assistant), wenn die genaue Seite nicht gefunden wird. Laptops und Marken-PCs bekommen zusätzlich die Support-Seite ihres Herstellers (Dell, HP, Lenovo, ASUS, Acer, MSI, GIGABYTE, Samsung).
- Bedrock: der Launcher installiert die Microsoft-Visual-C++-Laufzeit, die der Bedrock-Manager braucht (er brach mit „You need VC++ Runtime 14.14…“ ab), und sobald der Manager installiert ist, startet ihn eine Schaltfläche Spielen.
- Instanzsymbole nutzen die Texturen von Minecraft selbst (Diamantspitzhacke, TNT, Werkbank, Creeper…), gelesen aus dem Spiel, das der Launcher heruntergeladen hat.
- Wähle im Kontomenü ein eigenes Profilbild (und kehre jederzeit zu dem von Microsoft zurück).
- Start zeigt EBALIA Mods mit Titelbildern über den Neuigkeiten.
- Eine aufgeräumtere Patreon-Seite: Mitgliedskarte, ordentliche Beiträge mit lesbaren Daten und „Mehr anzeigen“, in Community und Neuigkeiten.
- Die Einstellung für die Adresse des Patreon-Dienstes wurde entfernt; kein leerer Streifen mehr unter den Seiten; „Hilfe & Community“ zeigt sein „&“.

# 1.1.1 (2026-10-07)

- Virtuelle Maschinen und Computer ohne Grafiktreiber: wenn sich Minecraft schließt, weil kein OpenGL vorhanden ist, bietet der Launcher an, es mit Software-Grafik zu starten (OpenGL vom Prozessor gezeichnet; Mesa unter Windows, das System-Mesa unter Linux). Das lässt sich auch in den Instanzeinstellungen einschalten. Windows nutzt es für Minecraft 1.13 und neuer.
- Schnellere Spiel-Downloads: Verbindungen zu Mojangs Servern werden wiederverwendet und Bibliotheken parallel heruntergeladen. Ein frisches Minecraft 1.21.1 (3.900 Dateien, 850 MB) ging in unserem Test von 143 auf 28 Sekunden.
- Instanzhintergründe: wähle ein beliebiges Bild des Launchers oder eines von deinem Computer, auf der Instanzseite oder in ihren Einstellungen.
- Das Instanzsymbol lässt sich auch in den Instanzeinstellungen ändern.
- Wenn sich Minecraft mit einem Fehler schließt, sagt eine Übersicht, was fehlgeschlagen ist, mit einer Schaltfläche zur Behebung: ein fehlender Mod (von Modrinth für die Version und den Loader der Instanz installiert), ein Absturz des Grafiktreibers (Link zum Treiber von AMD, NVIDIA oder Intel oder Software-Grafik), kein OpenGL, das falsche Java oder zu wenig Arbeitsspeicher. Das vollständige Protokoll ist einen Klick entfernt.
- .minecraft-Ordner importieren: den des offiziellen Minecraft Launchers und Versionsordner von Launchern, die sein Format nutzen (TLauncher, SKLauncher…), gezippt oder nicht. Minecraft-Version und Loader kommen aus dem Versionsprofil oder, wenn es keines gibt, aus den Mods selbst. Diese Ordner erscheinen auch in der Liste der auf dem Computer gefundenen Instanzen, bereit zum Import. Kontodateien werden nie kopiert.
- Instanzordner tragen den Namen der Instanz statt einer zufälligen Nummer; eine Instanz umzubenennen benennt ihren Ordner um. Instanzen aus früheren Versionen werden beim Start umbenannt und funktionieren weiter (ein Ordner in Benutzung behält seinen alten Namen).
- Füge Instanzen anderer Launcher direkt in den Instanzordner ein (Prism Launcher, MultiMC, CurseForge-App, FTB App, ATLauncher, GDLauncher, Modrinth App, TLauncher oder Versionsordner von .minecraft, .zip- und .mrpack-Dateien): der Launcher macht daraus Instanzen mit ihrer Minecraft-Version und ihrem Loader, auch während er geöffnet ist. Das Original kommt nur in den Papierkorb der Instanzen, wenn das geklappt hat.
- Jeder Import prüft, ob seine Mods zur Minecraft-Version und zum Loader des Packs passen, und nennt die, die nicht passen.

# 1.1.0 (2026-10-05)

- Die Microsoft-Anmeldung ist mit der öffentlichen Anwendungs-ID von EBALIA aktiviert (`microsoft-client-id.txt`).
- Microsoft-Konten zeigen jetzt ihr Profilbild (das Xbox-Spielerbild des Kontos; das Gesicht des Minecraft-Skins, wenn es keins gibt) in der Seitenleiste, auf der Spielerkarte von Start, im Kontomenü und in den Einstellungen. Die Bilder werden in `mc/avatars` gespeichert.
- Silence, das Maskottchen von EBALIA, begleitet die interaktive Tour und die Seite Anleitung, animiert: schwebt, atmet, blinzelt, leuchtende Augen, ein schwingendes Messer, ein Hüpfer bei jedem Schritt und ein Neigen zum hervorgehobenen Bedienelement.
- Portabler Modus: eine `portable.txt` neben dem Launcher (neben dem AppImage oder dem `.app`-Bundle) speichert Instanzen, Konten und Einstellungen in einem Ordner `ebalia-data` daneben.
- Silence hat jetzt ein Pixel-Art-Spritesheet mit sieben Posen (ruhig, fröhlich, überrascht, zwinkernd, zeigend, jubelnd, nachdenklich), jede mit Bildern zum Sprechen und Blinzeln; jeder Schritt der Tour nutzt seine eigene Pose.
- Neue handgezeichnete Instanzsymbole in 16×16. Jedes Bild von deinem Computer kann als Instanzsymbol dienen, beim Erstellen der Instanz oder später auf ihrer Seite.
- Die Instanzseite zeigt ihre Screenshots (zum Öffnen klicken) und ihre Mehrspieler-Serverliste, in der Server hinzugefügt, kopiert und entfernt werden können; unbekannte Felder von `servers.dat` bleiben erhalten.
- Übersetzungsdurchgang: die interaktive Tour, die Seiten zu Patreon und Kreationen, die Deinstallation verschollener Versionen, die Meldungen der Microsoft-Anmeldung und der Updater des Launchers sind in alle zehn Sprachen übersetzt.
- Website: Downloads von Installer und portabler Version nebeneinander, SHA-256 jedes Pakets mit Kopierschaltfläche und dem Befehl zur Prüfung, und ein überarbeitetes Handy-Layout, in dem jeder Link sichtbar bleibt.
- Neue Pakete: Linux-AppImage für jede Distribution mit glibc 2.35+ (Qt, libarchive und OpenSSL enthalten; in der CI unter Debian, Fedora, Arch und openSUSE getestet) mit Updates in der App, portable Windows-`.zip`, portable Linux-`.tar.gz` und macOS-Builds für Intel neben Apple Silicon, jeweils mit einer portablen `.zip`.
- Forge- und NeoForge-Instanzen repariert: Loader-Bibliotheken werden nach Artefakt und Classifier zusammengeführt, sodass Forge sein universelles Jar behält („Failed to find system mod: forge“). Im Spiel geprüft mit Forge 26.3, 1.20.1, 1.16.5, 1.12.2, 1.8.9 und 1.7.10, NeoForge 26.3 und 1.21.1, Fabric und Quilt 26.3.
- Forge-, NeoForge-, Fabric- und Quilt-Instanzen, die mit einem früheren Launcher installiert wurden, werden beim nächsten Druck auf Spielen neu vorbereitet (heruntergeladene Dateien werden wiederverwendet).
- NeoForge 26.x und NeoForge für 1.20.1 sind verfügbar; Fabric und Quilt wählen den neuesten stabilen Loader; Forge 1.7.10–1.12.2 installiert über seinen alten Installer.
- Verschollene Versionen starten mit Java direkt unter Windows, macOS und Linux (ohne Wine): a1.1.1, Alpha 1.0.16, 1.1 Patch 1, Ghost, 554e, NSSS und jede andere archivierte Version erreichen ihre Menüs. Archive werden mit SHA-256 geprüft, und jede Version behält ihre Spielstände in ihrem eigenen Ordner.
- Modpack-Import aus `.mrpack`, CurseForge-`.zip`, Prism Launcher / MultiMC / PolyMC, CurseForge-App, FTB App, ATLauncher, GDLauncher, Modrinth App und Technic, mit Forge, NeoForge, Fabric, Quilt oder Vanilla. ZIPs in Ordnern oder unter macOS und Windows erstellt, Dateinamen in jeder Sprache, Spiegel-URLs und optionale Dateien von Modrinth sowie Ressourcenpakete und Shader von CurseForge werden verarbeitet.
- Wenn eine neue Version verfügbar ist, erscheint in der Seitenleiste eine Schaltfläche „Aktualisieren auf …“; sie lädt das Update herunter, prüft und installiert es, ohne die Website zu besuchen.
- Ein experimenteller externer Bedrock-Manager mit Installation im Hintergrund wurde hinzugefügt. Windows nutzt BedrockLauncher; Linux/macOS nutzen das Android-Backend und erfordern den Kauf bei Google Play. Ein Kauf unter Windows gilt nicht für Android. Echtes Spielen ist noch nicht auf allen Plattformen geprüft.
- Deinstallation verschollener Versionen mit Bestätigung und Verschieben in den Papierkorb hinzugefügt.
- Der Fortschritt von Vorgängen ist in die untere Statusleiste umgezogen.
- Die Windows-Laufzeit von Visual C++ ist enthalten, und der Installer pro Benutzer legt eine Desktop-Verknüpfung an.
- Aktualisieren und neu starten im Launcher hinzugefügt. Die Pakete einer Version werden mit SHA-256 geprüft, vorbereitet und beim Start getestet, bevor die installierten Programmdateien ersetzt werden. Vorherige Launcher-Dateien und temporäre Downloads werden nach einem erfolgreichen Start entfernt. NixOS baut die Version mit Nix; gemeinsame Store-Pfade unterliegen weiter der Garbage Collection von Nix.
- Der Updater entfernt nie Instanzen, verschollene Versionen, Welten, Mods oder heruntergeladene Inhalte. Aktualisierungen sind gesperrt, solange Spiele oder Installationen laufen. Schlägt das Ersetzen fehl, werden die vorherigen Programmdateien wiederhergestellt.
- Der Windows-Installer erkennt den Ordner der vorhandenen Installation, auch im Registrierungsformat des vorherigen Installers, und aktualisiert ihn an Ort und Stelle.
- Eine interaktive Tour mit 14 Schritten beim ersten Start hinzugefügt, mit hervorgehobenen Bedienelementen, Zurück/Weiter/Überspringen und Wiederholung in der Anleitung.
- Wer 1.0.0 nutzt, muss 1.1.0 einmal installieren, um den eingebauten Updater zu bekommen; künftige kompatible Versionen aktualisieren sich aus dem Launcher heraus.
- Der bisherige Speicherort der Anwendungsdaten bleibt erhalten.

Der Launcher enthält weder Minecraft noch Konten oder Spiellizenzen. Die Pakete für Windows und macOS sind nicht signiert. Das Linux-Binärprogramm zielt auf Ubuntu 24.04 x64; andere Distributionen können aus dem Quellcode bauen, auch NixOS über das Flake.
