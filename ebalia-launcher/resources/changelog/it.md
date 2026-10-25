# 1.2.4 (2026-10-25)

- Le skin locali funzionano in 1.21.9 e 26.x: queste versioni chiamano diversamente il formato dei pacchetti di risorse e il launcher le rifiutava come non supportate. Il pack della skin ora segue il formato di ogni versione, dalla 1.6 alla 26.x.
- Skin → Applica localmente a tutte le istanze: la skin diventa la skin locale di tutte le istanze, anche prima di crearne una. Le istanze installate la ricevono subito, le nuove appena installate, e quelle aperte o in installazione all’avvio. Applica localmente a un’istanza continua a dare a una sola istanza la sua skin, e Ripristina aspetto locale propone prima tutte le istanze.
- Su NixOS, le finestre di Minecraft 26.x hanno barra del titolo e bordi su GNOME (Wayland).

# 1.2.3 (2026-10-20)

- Le Versioni perdute si scaricano dalla pubblicazione di EBALIA, e i loro archivi non contengono più i log e i rapporti di crash rimasti da altri launcher. Quelle che hai già installato continuano a funzionare; la prossima che installi scarica di nuovo il suo archivio, una volta.

# 1.2.2 (2026-10-18)

- Scopri mod cerca mentre scrivi (mod, pacchetti di risorse e shader, e i modpack quando crei un’istanza): i risultati arrivano appena fai una pausa, senza premere Invio, che continua a cercare subito. Se una ricerca non è possibile (offline, nessuna istanza), lo dice l’elenco invece di una finestra.
- La ricerca in Scopri mod non scatta più: le immagini si scaricano e passano dalla loro cache in un thread a parte, l’elenco si ridisegna una sola volta per ricerca, e la pagina completa, la galleria e la copertina di una mod si caricano quando la scelta resta ferma e smetti di scrivere.
- Le collaborazioni di EBALIA (come Neighbor.jar) non appaiono più come mod proprie: una nuova mod di EBALIA con altri autori entra tra le collaborazioni, con il credito di quegli autori, sul sito e nel launcher. La Home mostra solo le mod proprie.
- La finestra EBALIA Mods è nuova: le mod proprie e le collaborazioni separate, come sul sito, in schede come quelle della Home con l’immagine, i download, chi l’ha fatta, la descrizione nella tua lingua e piccoli pulsanti di CurseForge e Modrinth; in alto, il totale dei download; Patreon con ciò che offre. L’invito di Patreon ha lo stesso stile.
- Bedrock: Gioca sta accanto alla versione invece di attraversare la scheda, le azioni condividono una riga e l’aiuto è una riga di link.

# 1.2.1 (2026-10-18)

- L’avviso di aggiornamento mostra le novità: le note di ogni versione dalla tua fino a quella proposta, nella lingua del launcher, con Aggiorna e riavvia o Più tardi sotto.
- Modifiche: una sezione della barra laterale (e la versione indicata sotto) elenca ogni versione con la sua data, dalla più recente, e mostra cosa è cambiato in quella che scegli, nella lingua del launcher; la ricerca tiene le versioni che citano una parola e la evidenzia. Anche la tua è segnalata.
- I campi di ricerca (il filtro del registro, Scopri mod, le istanze e le versioni perdute) tengono la lente centrata tra il bordo e il testo.
- Nuove icone nella barra laterale: pixel art nello stile del gioco, disegnata per EBALIA: un blocco d’erba per la Home, un piccone per le istanze, una bussola, un fantasma per le versioni perdute, un baule per I miei pack, un giocatore per le skin, un foglio di carta per le notizie, due giocatori per la community, un libro per la guida, un blocco di bedrock, un orologio per le modifiche e un’incudine per le impostazioni.
- La Home mostra il sito ufficiale di EBALIA, ebalia.gitgud.site: la sua scheda di anteprima nella tua lingua, cosa contiene e un pulsante per aprirlo.
- Corretto: cambiando lo sfondo della Home con le frecce, l’immagine mostrata saltava al posto della successiva mentre questa si caricava, come una striscia attraverso il banner.
- Il piccone delle istanze è quello di Minetest Game (CC BY-SA 3.0).
- Le impostazioni aprono la cartella in cui si trova il launcher (dove è stato installato, o dove hai messo la copia portable) e la mostrano.
- Disinstalla, nelle impostazioni: il launcher si chiude e si rimuove (con il programma di disinstallazione su Windows; da una cartella portable o estratta, solo i file che elenca, il resto rimane; il file AppImage o l’app macOS) e, se lo selezioni, anche i tuoi dati. Le copie installate dal sistema (Nix, pacchetti) si rimuovono con il suo gestore di pacchetti.

# 1.2.0 (2026-10-15)

- Usa circa metà della memoria: le immagini si caricano alla dimensione in cui vengono mostrate invece che a grandezza piena (gli sfondi della Home non vengono più letti tutti in anticipo, le immagini delle notizie e le schede delle versioni perdute sono piccole, le icone della finestra delle mod sono tenute a 64 pixel), le cache delle immagini sono più piccole e ridurre a icona il launcher (come mentre giochi) libera le immagini tenute per dopo. Su Linux, la memoria liberata torna al sistema.
- Le versioni perdute installano solo il loro gioco. Alcuni pacchetti dell’archivio erano stati salvati insieme al launcher che li avviava (UltimMC, BetaCraft) e ad altre versioni; EBALIA avvia il gioco da sé, quindi quelle parti non vengono più installate: NSSS occupa 4 MB invece di 700, Alpha 1.0.16 e Minecraft a1.1.1 fanno a meno di UltimMC e BetaCraft. Le versioni installate prima vengono riordinate al prossimo avvio e mantengono mondi, opzioni e registro.
- Le versioni perdute mostrano i propri screenshot, scattati in ogni versione: l’immagine grande, una striscia per sceglierne un’altra e un visualizzatore a grandezza piena. Ognuna racconta cos’è (una descrizione più lunga in ogni lingua), cosa mostra il gioco come versione, se è una ricreazione o è stata recuperata da EBALIA, chi l’ha fatta, la sua dimensione installata e da scaricare, e con cosa funziona. Le schede dell’elenco mostrano gli stessi screenshot.
- Corretto: Minecraft 1.Li02 si chiudeva con un errore alla creazione di un mondo (la sua schermata Create New World non preparava mai il giocatore). Ora EBALIA lo avvia con quella schermata corretta, senza modificare i file del gioco; i mondi in Sopravvivenza e Hardcore funzionano.
- Il registro in diretta fa parte della pagina dell’istanza, sotto gli screenshot e i server, e segue il gioco mentre è in esecuzione. Schermo intero gli dà tutta la pagina (il pulsante del registro in alto lo apre così); Esc o il link per tornare indietro riportano alla pagina.

# 1.1.9 (2026-10-13)

- Più veloce: leggere le icone delle mod, le immagini e le descrizioni dei pacchetti, cercare i pacchetti su Modrinth e le teste delle skin avvengono su pochi thread in background dedicati e vengono ricordati per la sessione. Prima, aprire la finestra delle mod occupava tutti i thread del processore per qualche secondo e qualsiasi cosa facessi dopo (installare, cercare, rimuovere, aprire un’altra finestra) aspettava dietro; ora parte subito. Riaprire una finestra, rimuovere una mod o disattivarla non rilegge più tutti i file.
- Chiudere il launcher lo termina subito: i download e le ricerche ancora in corso si fermano invece di tenere vivo il processo per minuti con una connessione lenta.
- Le immagini da internet vengono decodificate fuori dal thread della finestra, la cache delle immagini scaricate ha un limite e la copertina della pagina della mod viene ridimensionata una volta per dimensione; le immagini sono nitide sugli schermi con ridimensionamento.
- Scopri mod dice cosa ha già l’istanza: le mod, i pacchetti di risorse e gli shader installati appaiono come «Già installato» nell’elenco, anche il pulsante Installa lo dice e installarne diversi li lascia fuori. I pacchetti vengono registrati quando si installano; quelli aggiunti a mano o prima vengono riconosciuti tramite Modrinth. Un pacchetto o una mod rimossi dalla loro cartella si possono reinstallare.
- Le immagini delle mod sono nitide: il visualizzatore della galleria e la copertina della pagina della mod mostrano le immagini come sono state caricate invece delle anteprime da 350 pixel di Modrinth (l’anteprima appare mentre si scarica l’originale), e la striscia di CurseForge usa le sue anteprime piccole.
- Gli elenchi a discesa (Mod, Pacchetti di risorse, Shader e ogni altra scelta) si aprono sotto la loro casella con un solo bordo; su Windows parti dell’elenco venivano disegnate fuori.
- Le schede Pacchetti di risorse e Shader hanno un solo link, come Mod: la loro finestra ha il pulsante per scaricare.
- Gli shader, e i pacchetti di risorse senza immagine, mostrano l’icona del loro progetto su Modrinth (trovato dal file e tenuto in cache). I pulsanti Usa questo shader e Attiva mantengono lo stesso spazio tra icona e testo degli altri.
- Registro in diretta: la finestra del registro mostra l’output del gioco mentre viene scritto, lo segue a meno che tu non scorra in su, colora gli errori di rosso e gli avvisi di giallo, filtra le righe, copia tutto e non blocca più il launcher dietro. Anche le versioni perdute hanno un pulsante del registro.
- Versioni perdute: Disinstalla sta accanto a Gioca e Reinstalla ogni volta che la versione è installata.
- I download mostrano la percentuale (e la dimensione nel suggerimento): le versioni perdute e il loro Java, e l’installazione delle istanze accanto a ciò che si sta facendo.
- Bedrock cerca il gioco una volta, e di nuovo solo dopo un’azione fatta dal launcher (installare, aggiornare, disinstallare, aprire lo Store o Minecraft Linux) o con Controlla di nuovo, invece che a ogni apertura della pagina.

# 1.1.8 (2026-10-13)

- Corretto: i pulsanti Installa e Apri pagina della pagina della mod chiudevano il launcher su tutti i sistemi (e potevano farlo anche i pulsanti «Installa <mod>» e «Scarica il driver» del riepilogo dei crash).
- Scopri mod ha un solo pulsante Installa: installa ogni voce selezionata (Ctrl o Maiusc per sceglierne diverse) e dice quante; le mod senza versione per l’istanza vengono elencate mentre il resto si installa.
- Pacchetti di risorse e shader da CurseForge e Modrinth: scegli Mod, Pacchetti di risorse o Shader in Scopri mod, oppure premi Scarica su quelle schede di un’istanza. Gli shader propongono Iris (Fabric, Quilt, NeoForge) o Oculus (Forge) quando l’istanza non ha nessuno dei due. Le istanze vanilla possono ricevere pacchetti di risorse.
- La pagina della mod mostra la sua galleria: miniature, e le immagini in grande con ◀ ▶ e i tasti freccia.
- L’icona della mod è centrata e non tocca più il bordo del pannello quando la mod non ha un’immagine di copertina.
- Il menu dell’icona dell’istanza mostra sempre le stesse immagini delle istanze (teneva quelle incluse dopo aver letto le texture del gioco).
- Le immagini degli account mostrano la testa della skin che il giocatore indossa (Microsoft) o riceve di default (senza premium) invece dell’immagine Xbox; un’immagine che hai scelto ha ancora la precedenza. L’elenco delle skin mostra la testa di ogni skin.
- Pacchetti di risorse, shader e mondi aprono la propria finestra dalla pagina dell’istanza (clic in qualsiasi punto della scheda), come le mod: ogni pacchetto con immagine e descrizione, attivare o disattivare i pacchetti di risorse e scegliere lo shader che il gioco carica, aggiungere ZIP, rimuovere (conservati in removed-… dentro l’istanza), aprire la cartella e scaricarne altri. Mondi: aprire la cartella di un mondo, esportarlo come ZIP, importare un mondo in ZIP. Anche la finestra delle mod apre la sua cartella.
- La pagina Skin mostra la skin su un modello 3D: trascinala per girarla, doppio clic per rivederla di fronte, Fronte / retro le fa fare mezzo giro. La vista piatta è a un pulsante di distanza.
- Su GNOME e sugli altri desktop Wayland che lasciano la barra del titolo all’app, EBALIA disegna la sua: doppio clic per massimizzare o ripristinare, trascinala per spostare la finestra e ridimensiona dai bordi. Windows, macOS, X11, KDE e gli altri mantengono la barra del titolo del sistema, dove il doppio clic lo fa già. EBALIA_TITLE_BAR=system o custom forza l’una o l’altra.
- Le skin si possono eliminare dalla libreria (Elimina skin, o il tasto Canc); il PNG da cui sono state importate resta.
- Guida e tutorial: i passi della guida sono schede affiancate, tutte visibili senza scorrere, e le domande e risposte hanno la loro scheda.
- Silence parla nel tour interattivo: il testo appare lettera per lettera con piccoli suoni di voce, un rintocco segna ogni passo e una breve melodia la fine. Il pulsante con l’altoparlante del tour lo silenzia (viene ricordato). I suoni li crea il launcher stesso e li riproduce tramite il sistema (PlaySound su Windows, suoni di sistema su macOS, PulseAudio o PipeWire su Linux): nessun file audio e nessuna nuova libreria.
- Traduzioni: la nota sulla skin predefinita dei profili senza premium era in inglese in sette lingue; il tour ora chiama le Versioni perdute come la barra laterale in ogni lingua; i suggerimenti della barra del titolo seguono il cambio di lingua.

# 1.1.7 (2026-10-12)

- Bedrock su Linux e macOS passa dalla modalità profilo di Minecraft Linux stesso, che verifica l’acquisto su Google Play e carica il suo modulo di compatibilità: la versione che scegli in EBALIA si avvia come dal suo pulsante Gioca. L’importazione dell’APK e l’avvio diretto del gioco non ci sono più (saltavano la verifica dell’acquisto).
- Bedrock elenca le versioni scaricate tra cui scegliere, tiene aggiornato Minecraft Linux (Flathub su Linux; l’app verificata per Apple Silicon su macOS 15+) e dice perché un gioco si è chiuso (acquisto non confermato, versione incompatibile, componenti mancanti).
- Bedrock su Windows: Minecraft e Minecraft Preview tra cui scegliere, un pulsante Game Pass (app Xbox), un controllo dei Gaming Services, un avviso nelle macchine virtuali senza scheda grafica e una disinstallazione che prima copia i mondi da dove le versioni attuali li conservano.
- Aiuto per accedere a Xbox (Remote Connect su Linux e macOS).
- Creare un profilo senza premium prima di aver scaricato una versione del gioco poteva bloccare il launcher se era stato aperto da un terminale (un percorso vuoto del file del gioco faceva aspettare al lettore di archivi un input da tastiera).

# 1.1.6 (2026-10-12)

- Scopri mod: la pagina della mod legge le descrizioni come Modrinth (Markdown con HTML dentro, tramite md4c), così compaiono testo, tabelle, titoli e immagini; le immagini si sistemano mentre si scaricano senza che la pagina salti, e la copertina mostra l’immagine intera invece di un ritaglio ingrandito.
- Il doppio clic installa anche mentre l’elenco sta ancora caricando (parte appena finisce); il pannello della pagina è sempre visibile, quindi l’elenco non si sposta più sotto il mouse.
- Tema personalizzato: un’anteprima dal vivo di una parte del launcher nei colori scelti.
- Home a schermo intero: il banner cresce con la finestra invece di ridursi a una striscia.

# 1.1.5 (2026-10-11)

- Bedrock senza un altro launcher: su Windows EBALIA rileva Minecraft per Windows e lo avvia direttamente (o apre la sua pagina sul Microsoft Store); su Linux installa l’ambiente di Minecraft Linux, importa l’APK di Minecraft che hai acquistato e avvia il gioco da sé. Nuovo pulsante Disinstalla (su Windows i mondi vengono copiati prima; su Linux i mondi restano a meno che tu non scelga di eliminarli).
- Scopri mod: un’anteprima della mod selezionata (immagine, descrizione, download), doppio clic per installare, solo mod per la versione dell’istanza e nessuna barra di caricamento quando altri risultati si caricano in fondo all’elenco.
- Due mod che richiedono versioni diverse della stessa dipendenza (spesso Fabric API) non fermano più l’installazione: si tiene la più recente.
- Tutto tradotto: i testi più recenti, le domande e risposte, il tour e i messaggi di installazione, download, Java, skin, Bedrock e aggiornamento, in tutte e dieci le lingue.
- La rotella del mouse non cambia mai un elenco a discesa (come la lingua), nemmeno dopo averci cliccato.

# 1.1.4 (2026-10-11)

- Scopri mod carica le mod popolari appena si apre la scheda (e quando cambiano l’istanza o il fornitore); Invio cerca anche mentre è in corso un’altra attività, e scorrere fino in fondo carica altri risultati.
- Tema personalizzato: scegli i tuoi colori di accento e di sfondo in Impostazioni → Tema → Personalizzato.
- La rotella del mouse scorre la pagina: gli elenchi a discesa (come la lingua) cambiano solo dopo averci cliccato.
- Il launcher non diventa più grigio dietro una finestra di dialogo aperta; ignora semplicemente i clic finché la finestra non si chiude.
- Cambiare tema è istantaneo: vengono riapplicati solo i colori invece di ricostruire ogni pagina.
- Le mod mostrano le proprie icone nella finestra delle mod, per ogni loader (Fabric, Quilt, Forge, NeoForge, il vecchio mcmod.info); le mod senza icona ricevono quella di Modrinth.
- Skin mostra la skin che indossi: la skin attuale dell’account Microsoft o, per i profili senza premium, la skin predefinita che Minecraft dà loro.
- Anche le versioni perdute usano texture di Minecraft (blocco d’erba, lanterna di zucca per le versioni horror, banco da lavoro).
- Nuova istanza: Personalizzato mostra il blocco d’erba di Minecraft e Importa un’icona di importazione pulita.
- Server nella pagina dell’istanza: un pulsante Modifica, i nomi completi dei server (senza i codici colore di Minecraft) e i titoli Server e Screenshot restano in alto quando gli elenchi sono vuoti.

# 1.1.3 (2026-10-08)

- Temi: EBALIA (verde), Oscurità profonda (verde acqua), Nether (rosso), End (viola), Oceano (blu) e Ciliegio (rosa), nelle Impostazioni, applicati subito.
- Domande e risposte in Guida e tutorial: importare pack, crash, comprare Minecraft, mod, Java, macchine virtuali, dove sono i mondi, aggiornamenti, uso portatile, icone e immagini, versioni perdute e avvisi dell’antivirus.
- Le copertine di EBALIA Mods nella Home usano illustrazioni scure (Nether, Deep Dark, miniera abbandonata).

# 1.1.2 (2026-10-08)

- I crash del driver grafico e la mancanza di OpenGL mostrano la tua scheda grafica (dedicata o integrata nel processore) e aprono il suo driver: la pagina esatta del driver per le schede NVIDIA GeForce e AMD Radeon RX, le pagine di Intel per Arc e la grafica dalla 7ª alla 10ª generazione, e lo strumento di rilevamento automatico del produttore (NVIDIA App, AMD Auto-Detect, Intel Driver & Support Assistant) quando la pagina esatta non si trova. Portatili e PC di marca ricevono anche la pagina di assistenza del loro produttore (Dell, HP, Lenovo, ASUS, Acer, MSI, GIGABYTE, Samsung).
- Bedrock: il launcher installa il runtime Microsoft Visual C++ di cui ha bisogno il gestore di Bedrock (si fermava con «You need VC++ Runtime 14.14…»), e una volta installato il gestore un pulsante Gioca lo avvia.
- Le icone delle istanze usano le texture di Minecraft stesso (piccone di diamante, TNT, banco da lavoro, creeper…), lette dal gioco scaricato dal launcher.
- Scegli la tua immagine del profilo dal menu dell’account (e torna a quella di Microsoft quando vuoi).
- La Home mostra EBALIA Mods con copertine, sopra le notizie.
- Una pagina Patreon più ordinata: scheda dell’abbonamento, post in ordine con date leggibili e «Mostra di più», in Comunità e Notizie.
- Rimossa l’impostazione dell’indirizzo del servizio Patreon; niente più striscia vuota sotto le pagine; «Assistenza e comunità» mostra la sua «e».

# 1.1.1 (2026-10-07)

- Macchine virtuali e computer senza driver grafico: quando Minecraft si chiude perché non c’è OpenGL, il launcher propone di avviarlo con la grafica software (OpenGL disegnato dal processore; Mesa su Windows, il Mesa di sistema su Linux). Si può anche attivare nelle impostazioni dell’istanza. Windows lo usa per Minecraft 1.13 e successivi.
- Download del gioco più veloci: le connessioni ai server di Mojang vengono riutilizzate e le librerie si scaricano in parallelo. Un Minecraft 1.21.1 nuovo (3.900 file, 850 MB) è passato da 143 a 28 secondi nel nostro test.
- Sfondi delle istanze: scegli qualsiasi immagine del launcher o una dal tuo computer, dalla pagina dell’istanza o dalle sue impostazioni.
- L’icona dell’istanza si può cambiare anche dalle impostazioni dell’istanza.
- Quando Minecraft si chiude con un errore, un riepilogo dice cosa non ha funzionato con un pulsante per sistemarlo: una mod mancante (installata da Modrinth per la versione e il loader dell’istanza), un crash del driver grafico (link al driver AMD, NVIDIA o Intel, o grafica software), la mancanza di OpenGL, il Java sbagliato o troppa poca memoria. Il registro completo è a un clic.
- Importare cartelle .minecraft: quella del Minecraft Launcher ufficiale e le cartelle di versione dei launcher che usano il suo formato (TLauncher, SKLauncher…), compresse o no. La versione di Minecraft e il loader vengono dal profilo della versione o, se non c’è, dalle mod stesse. Queste cartelle compaiono anche nell’elenco delle istanze trovate sul computer, pronte da importare. I file degli account non vengono mai copiati.
- Le cartelle delle istanze portano il nome dell’istanza invece di un numero casuale; rinominare un’istanza rinomina la sua cartella. Le istanze delle versioni precedenti vengono rinominate all’avvio e continuano a funzionare (una cartella in uso tiene il vecchio nome).
- Incolla istanze di altri launcher direttamente nella cartella delle istanze (Prism Launcher, MultiMC, app di CurseForge, FTB App, ATLauncher, GDLauncher, Modrinth App, TLauncher o cartelle di versione di .minecraft, file .zip e .mrpack): il launcher le trasforma in istanze con la loro versione di Minecraft e il loro loader, anche mentre è aperto. L’originale va nel cestino delle istanze solo se tutto è andato bene.
- Ogni importazione controlla che le sue mod corrispondano alla versione di Minecraft e al loader del pack e nomina quelle che non corrispondono.

# 1.1.0 (2026-10-05)

- L’accesso con Microsoft è attivo con l’ID pubblico dell’applicazione di EBALIA (`microsoft-client-id.txt`).
- Gli account Microsoft ora mostrano la loro immagine del profilo (l’immagine giocatore Xbox dell’account; il volto della skin di Minecraft quando non ce n’è una) nella barra laterale, nella scheda del giocatore della Home, nel menu dell’account e nelle Impostazioni. Le immagini sono salvate in `mc/avatars`.
- Silence, la mascotte di EBALIA, accompagna il tour interattivo e la pagina Guida e tutorial, animata: fluttua, respira, sbatte le palpebre, ha gli occhi che brillano, fa oscillare un coltello, fa un saltello a ogni passo e si sporge verso il controllo evidenziato.
- Modalità portatile: un `portable.txt` accanto al launcher (accanto all’AppImage o al bundle `.app`) tiene istanze, account e impostazioni in una cartella `ebalia-data` lì accanto.
- Silence ora ha uno sprite sheet in pixel art con sette pose (ferma, felice, sorpresa, occhiolino, che indica, che festeggia, pensierosa), ognuna con fotogrammi per parlare e sbattere le palpebre; ogni passo del tour usa la sua posa.
- Nuove icone delle istanze 16×16 disegnate a mano. Qualsiasi immagine del tuo computer può fare da icona dell’istanza, alla creazione dell’istanza o più tardi dalla sua pagina.
- La pagina dell’istanza mostra i suoi screenshot (clic per aprire) e il suo elenco di server multigiocatore, dove si possono aggiungere, copiare e rimuovere server; i campi sconosciuti di `servers.dat` vengono conservati.
- Giro di traduzione: il tour interattivo, le pagine di Patreon e delle creazioni, la disinstallazione delle Versioni perdute, i messaggi di accesso Microsoft e il sistema di aggiornamento del launcher sono tradotti in tutte e dieci le lingue.
- Sito web: download dell’installer e della versione portatile affiancati, lo SHA-256 di ogni pacchetto con un pulsante per copiarlo e il comando per verificarlo, e un layout per telefono rifatto in cui ogni link resta visibile.
- Nuovi pacchetti: AppImage per Linux per qualsiasi distribuzione con glibc 2.35+ (Qt, libarchive e OpenSSL inclusi; testata su Debian, Fedora, Arch e openSUSE nella CI) con aggiornamenti dall’app, `.zip` portatile per Windows, `.tar.gz` portatile per Linux e build macOS per Intel oltre che per Apple Silicon, ognuna con uno `.zip` portatile.
- Correzione delle istanze Forge e NeoForge: le librerie del loader vengono unite per artefatto e classificatore, così Forge mantiene il suo jar universale («Failed to find system mod: forge»). Verificato in gioco con Forge 26.3, 1.20.1, 1.16.5, 1.12.2, 1.8.9 e 1.7.10, NeoForge 26.3 e 1.21.1, Fabric e Quilt 26.3.
- Le istanze Forge, NeoForge, Fabric e Quilt installate con un launcher precedente vengono preparate di nuovo la prossima volta che premi Gioca (i file scaricati vengono riutilizzati).
- NeoForge 26.x e NeoForge per 1.20.1 sono disponibili; Fabric e Quilt scelgono il loader stabile più recente; Forge 1.7.10–1.12.2 si installa con il suo vecchio installer.
- Le Versioni perdute si avviano con Java direttamente su Windows, macOS e Linux (senza Wine): a1.1.1, Alpha 1.0.16, 1.1 Patch 1, Ghost, 554e, NSSS e ogni altra versione archiviata arrivano ai loro menu. Gli archivi vengono verificati con SHA-256 e ogni versione tiene i suoi salvataggi nella propria cartella.
- Importazione di modpack da `.mrpack`, `.zip` di CurseForge, Prism Launcher / MultiMC / PolyMC, app di CurseForge, FTB App, ATLauncher, GDLauncher, Modrinth App e Technic, con Forge, NeoForge, Fabric, Quilt o vanilla. Vengono gestiti ZIP racchiusi in cartelle o creati su macOS e Windows, nomi di file in qualsiasi lingua, URL mirror e file opzionali di Modrinth, e pacchetti di risorse e shader di CurseForge.
- Quando è disponibile una nuova versione, nella barra laterale compare un pulsante «Aggiorna a …»; scarica, verifica e installa l’aggiornamento senza visitare il sito web.
- Aggiunto un gestore esterno sperimentale di Bedrock con installazione in background. Windows usa BedrockLauncher; Linux e macOS usano il motore Android e richiedono l’acquisto su Google Play. L’acquisto su Windows non vale per Android. Il gioco vero e proprio resta non verificato su tutte le piattaforme.
- Aggiunta la disinstallazione delle Versioni perdute con conferma e spostamento nel cestino.
- L’avanzamento delle operazioni è passato alla barra di stato in basso.
- Incluso il runtime di Visual C++ per Windows e aggiunto un collegamento sul desktop all’installer per utente.
- Aggiunto Aggiorna e riavvia nel launcher. I pacchetti di ogni versione vengono verificati con SHA-256, preparati e provati all’avvio prima di sostituire i file del programma installato. I file precedenti del launcher e i download temporanei vengono rimossi dopo un avvio riuscito. NixOS compila la versione con Nix; i percorsi condivisi dello store restano soggetti alla garbage collection di Nix.
- Il sistema di aggiornamento non rimuove mai istanze, Versioni perdute, mondi, mod o contenuti scaricati. Gli aggiornamenti sono bloccati mentre ci sono partite o installazioni in corso. Se la sostituzione fallisce, i file precedenti del programma vengono ripristinati.
- L’installer di Windows rileva la cartella dell’installazione esistente, compreso il formato di registro dell’installer precedente, e la aggiorna sul posto.
- Aggiunto un tour interattivo di 14 passi al primo avvio, con controlli evidenziati, Indietro/Avanti/Salta, e ripetibile da Guida e tutorial.
- Chi usa la 1.0.0 deve installare la 1.1.0 una volta per avere il sistema di aggiornamento integrato; le versioni compatibili successive si aggiornano dal launcher.
- Conservata la posizione esistente dei dati dell’applicazione.

Il launcher non include Minecraft, account né licenze del gioco. I pacchetti per Windows e macOS non sono firmati. Il binario per Linux è pensato per Ubuntu 24.04 x64; le altre distribuzioni possono compilare dai sorgenti, NixOS compreso tramite il flake.
