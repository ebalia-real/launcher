# 1.2.3 (2026-10-20)

- Las Versiones perdidas se descargan desde la publicación propia de EBALIA, y sus archivos ya no traen registros ni informes de errores que sobraban de otros launchers. Las que ya instalaste siguen funcionando; la próxima que instales vuelve a descargar su archivo una vez.

# 1.2.2 (2026-10-18)

- Explorar mods busca mientras escribís (mods, paquetes de recursos y shaders, y los modpacks al crear una instancia): los resultados llegan apenas hacés una pausa, sin apretar Enter, que sigue buscando al instante. Si una búsqueda no puede hacerse (sin conexión, sin instancia), lo dice la lista en vez de una ventana.
- Buscar en Explorar mods ya no da tirones: las imágenes se descargan y pasan por su caché en un hilo propio, la lista se redibuja una sola vez por búsqueda, y la página completa, la galería y la portada de un mod se cargan cuando la elección se queda quieta y dejás de escribir.
- Las colaboraciones de EBALIA (como Neighbor.jar) ya no aparecen como propias: un mod nuevo de EBALIA con otros autores entra en las colaboraciones, con el crédito de esos autores, en la página y en el launcher. Inicio muestra solo los mods propios.
- La ventana EBALIA Mods es nueva: los mods propios y las colaboraciones por separado, como en la página, en tarjetas como las de Inicio con la imagen, las descargas, quién lo hizo, la descripción en tu idioma y botones chicos de CurseForge y Modrinth; arriba, el total de descargas; Patreon con lo que trae. La invitación de Patreon tiene el mismo estilo.
- Bedrock: Jugar queda al lado de la versión en vez de cruzar la tarjeta, las acciones comparten una fila y la ayuda es una fila de enlaces.

# 1.2.1 (2026-10-18)

- El aviso de actualización muestra las novedades: las notas de cada versión desde la tuya hasta la que se ofrece, en el idioma del launcher, con Actualizar y reiniciar o Más tarde debajo.
- Cambios: una sección de la barra lateral (y la versión que aparece debajo) lista cada versión con su fecha, de la más nueva a la más vieja, y muestra qué cambió en la que elijas, en el idioma del launcher; la búsqueda deja las versiones que mencionan una palabra y la marca. La tuya también aparece marcada.
- Los campos de búsqueda (el filtro del registro, Explorar mods, instancias y versiones perdidas) mantienen la lupa centrada entre el borde y el texto.
- Íconos nuevos en la barra lateral: pixel art al estilo del juego, dibujado para EBALIA: un bloque de pasto para Inicio, un pico para Instancias, una brújula, un fantasma para las versiones perdidas, un cofre para Mis packs, un jugador para Skins, una hoja de papel para Noticias, dos jugadores para Comunidad, un libro para la guía, un bloque de bedrock, un reloj para Cambios y un yunque para Ajustes.
- Inicio muestra la página oficial de EBALIA, ebalia.gitgud.site: su tarjeta de vista previa en tu idioma, qué tiene y un botón para abrirla.
- Arreglado: al cambiar el fondo de Inicio con las flechas, la imagen que se veía saltaba al lugar de la siguiente mientras esta cargaba, y quedaba como una franja cruzando el banner.
- El pico de Instancias es el de Minetest Game (CC BY-SA 3.0).
- Ajustes abre la carpeta donde está el launcher (donde se instaló, o donde pusiste la copia portable) y la muestra.
- Desinstalar, en Ajustes: el launcher se cierra y se quita (con el desinstalador del instalador en Windows; de una carpeta portable o descomprimida, solo los archivos que lista, y lo demás queda; el archivo AppImage o la app de macOS) y, si lo marcás, también tus datos. Las copias instaladas por el sistema (Nix, paquetes) se quitan con su gestor de paquetes.

# 1.2.0 (2026-10-15)

- Usa cerca de la mitad de memoria: las imágenes se cargan al tamaño en que se muestran y no a tamaño completo (los fondos de Inicio ya no se leen todos de antemano, las imágenes de las noticias y las tarjetas de las versiones perdidas son chicas, los íconos de la ventana de Mods se guardan a 64 píxeles), los cachés de imágenes son más chicos y minimizar el launcher (como al jugar) suelta las imágenes guardadas para después. En Linux, la memoria liberada vuelve al sistema.
- Las versiones perdidas instalan solo su juego. Algunos paquetes del archivo se guardaron junto con el launcher que los ejecutaba (UltimMC, BetaCraft) y con otras versiones; EBALIA inicia el juego por su cuenta, así que eso ya no se instala: NSSS ocupa 4 MB en vez de 700, y Alpha 1.0.16 y Minecraft a1.1.1 dejan atrás UltimMC y BetaCraft. Las versiones instaladas antes se ordenan la próxima vez que se inician y conservan sus mundos, opciones y registro.
- Las versiones perdidas muestran sus propias capturas, tomadas en cada versión: la imagen grande, una tira para elegir otra y un visor a tamaño completo. Cada una cuenta qué es (una descripción más larga en cada idioma), qué muestra el juego como versión, si es una recreación o fue rescatada por EBALIA, quién la hizo, su tamaño instalada y de descarga, y con qué funciona. Las tarjetas de la lista muestran las mismas capturas.
- Arreglado: Minecraft 1.Li02 se cerraba con un error al crear un mundo (su pantalla Create New World nunca preparaba al jugador). EBALIA ahora la inicia con esa pantalla corregida, sin cambiar los archivos del juego; funcionan los mundos de Supervivencia y Extremo.
- El registro en vivo es parte de la página de la instancia, debajo de las capturas y los servidores, y sigue al juego mientras está abierto. Pantalla completa le da toda la página (el botón de registro de arriba lo abre así); Escape o el enlace de volver regresan.

# 1.1.9 (2026-10-13)

- Más rápido: leer los íconos de los mods, las imágenes y descripciones de los paquetes, buscar paquetes en Modrinth y las cabezas de las skins se hacen en unos pocos hilos propios en segundo plano y se recuerdan durante la sesión. Antes, abrir la ventana de Mods ocupaba todos los hilos del procesador por unos segundos y lo que hicieras después (instalar, buscar, quitar, abrir otra ventana) esperaba detrás; ahora empieza al instante. Volver a abrir una ventana, quitar un mod o desactivarlo ya no vuelve a leer todos los archivos.
- Cerrar el launcher lo termina al instante: las descargas y consultas que siguen en curso se detienen en vez de mantener vivo el proceso durante minutos con una conexión lenta.
- Las imágenes de internet se decodifican fuera del hilo de la ventana, el caché de imágenes descargadas tiene un límite y la portada de la página del mod se escala una vez por tamaño; las imágenes se ven nítidas en pantallas con escala.
- Explorar mods dice qué tiene ya la instancia: los mods, paquetes de recursos y shaders instalados aparecen como «Ya instalado» en la lista, el botón Instalar también lo dice e instalar varios los deja afuera. Los paquetes quedan registrados al instalarse; los que se agregaron a mano o antes se reconocen por Modrinth. Un paquete o mod quitado de su carpeta se puede volver a instalar.
- Las imágenes de los mods se ven nítidas: el visor de la galería y la portada de la página del mod muestran las imágenes tal como se subieron en vez de las vistas previas de 350 píxeles de Modrinth (la vista previa se muestra mientras se descarga la original), y la tira de CurseForge usa sus vistas previas chicas.
- Las listas desplegables (Mods, Paquetes de recursos, Shaders y cualquier otra opción) se abren debajo de su casilla con un solo borde; en Windows, partes de la lista se dibujaban por fuera.
- Las tarjetas de Paquetes de recursos y Shaders tienen un solo enlace, como Mods: su ventana tiene el botón de descarga.
- Los shaders, y los paquetes de recursos sin imagen, muestran el ícono de su proyecto de Modrinth (encontrado por el archivo y guardado en caché). Los botones Usar este shader y Activar mantienen el mismo espacio entre ícono y texto que los demás.
- Registro en vivo: la ventana del registro muestra la salida del juego a medida que se escribe, la sigue salvo que subas, marca los errores en rojo y las advertencias en amarillo, filtra líneas, copia todo y ya no bloquea el launcher que está detrás. Las versiones perdidas también tienen un botón de registro.
- Versiones perdidas: Desinstalar queda junto a Jugar y Reinstalar siempre que la versión está instalada.
- Las descargas muestran su porcentaje (y el tamaño en la ayuda emergente): las versiones perdidas y su Java, y la instalación de las instancias junto a lo que se está haciendo.
- Bedrock busca el juego una vez, y de nuevo solo después de algo hecho desde el launcher (instalar, actualizar, desinstalar, abrir la Store o Minecraft Linux) o con Comprobar de nuevo, en vez de cada vez que se abre la página.

# 1.1.8 (2026-10-13)

- Arreglado: los botones Instalar y Abrir página de la página del mod cerraban el launcher en todos los sistemas (y también podían hacerlo los botones «Instalar <mod>» y «Descargar el controlador» del resumen de errores).
- Explorar mods tiene un solo botón Instalar: instala cada entrada elegida (Ctrl o Shift para elegir varias) y dice cuántas; los mods que no tienen versión para la instancia se listan mientras el resto se instala.
- Paquetes de recursos y shaders de CurseForge y Modrinth: elegí Mods, Paquetes de recursos o Shaders en Explorar mods, o tocá Descargar en esas tarjetas de una instancia. Los shaders ofrecen Iris (Fabric, Quilt, NeoForge) u Oculus (Forge) cuando la instancia no tiene ninguno. Las instancias vanilla pueden recibir paquetes de recursos.
- La página del mod muestra su galería: miniaturas, y las imágenes en grande con ◀ ▶ y las flechas del teclado.
- El ícono del mod queda centrado y ya no toca el borde del panel cuando el mod no tiene imagen de portada.
- El menú del ícono de la instancia siempre muestra las mismas imágenes que las instancias (se quedaba con las incluidas después de leer las texturas del juego).
- Las imágenes de las cuentas muestran la cabeza de la skin que usa el jugador (Microsoft) o la que recibe por defecto (no premium) en vez de la imagen de Xbox; una imagen que elegiste sigue teniendo prioridad. La lista de Skins muestra la cabeza de cada skin.
- Paquetes de recursos, shaders y mundos abren su propia ventana desde la página de la instancia (clic en cualquier parte de la tarjeta), como los mods: cada paquete con su imagen y descripción, activar o desactivar paquetes de recursos y elegir el shader que carga el juego, agregar ZIP, quitar (se guardan en removed-… dentro de la instancia), abrir la carpeta y descargar más. Mundos: abrir la carpeta de un mundo, exportarlo como ZIP, importar un mundo en ZIP. La ventana de mods también abre su carpeta.
- La página Skins muestra la skin en un modelo 3D: arrastrala para girarla, doble clic para verla de frente otra vez, Frente / espalda le da media vuelta. La vista plana está a un botón.
- En GNOME y otros escritorios Wayland que dejan la barra de título a la aplicación, EBALIA dibuja la suya: doble clic para maximizar o restaurar, arrastrala para mover la ventana y cambiá el tamaño desde los bordes. Windows, macOS, X11, KDE y el resto mantienen la barra de título del sistema, donde el doble clic ya hace esto. EBALIA_TITLE_BAR=system o custom fuerza una u otra.
- Las skins se pueden eliminar de la biblioteca (Eliminar skin, o la tecla Supr); el PNG del que se importaron se conserva.
- Guía y tutorial: los pasos de la guía son tarjetas una al lado de la otra, todas a la vista sin desplazarse, y las preguntas frecuentes tienen su propia pestaña.
- Silence habla en el recorrido interactivo: el texto aparece letra por letra con pequeños sonidos de voz, un timbre marca cada paso y una melodía corta el final. El botón de altavoz del recorrido lo silencia (se recuerda). Los sonidos los genera el propio launcher y suenan por el sistema (PlaySound en Windows, sonidos del sistema en macOS, PulseAudio o PipeWire en Linux): sin archivos de sonido ni bibliotecas nuevas.
- Traducciones: la nota sobre la skin por defecto de los perfiles no premium estaba en inglés en siete idiomas; el recorrido ahora nombra las Versiones perdidas como la barra lateral en cada idioma; las ayudas emergentes de la barra de título siguen el cambio de idioma.

# 1.1.7 (2026-10-12)

- Bedrock en Linux y macOS pasa por el modo de perfil del propio Minecraft Linux, que comprueba la compra en Google Play y carga su módulo de compatibilidad: la versión que elegís en EBALIA arranca igual que desde su botón Jugar. Se quitaron la importación de APK y el inicio directo del juego (se saltaban la comprobación de compra).
- Bedrock lista las versiones descargadas para elegir, mantiene actualizado Minecraft Linux (Flathub en Linux; la app verificada para Apple Silicon en macOS 15+) y dice por qué se cerró un juego (compra no confirmada, versión incompatible, componentes faltantes).
- Bedrock en Windows: Minecraft y Minecraft Preview para elegir, un botón de Game Pass (app de Xbox), una comprobación de Gaming Services, un aviso en máquinas virtuales sin tarjeta gráfica y una desinstalación que primero copia los mundos desde donde los guardan las versiones actuales.
- Ayuda para iniciar sesión en Xbox (Remote Connect en Linux y macOS).
- Crear un perfil no premium antes de descargar cualquier versión del juego podía congelar el launcher cuando se había abierto desde una terminal (una ruta vacía del archivo del juego hacía que el lector de archivos esperara entrada del teclado).

# 1.1.6 (2026-10-12)

- Explorar mods: la página del mod lee las descripciones como lo hace Modrinth (Markdown con HTML adentro, mediante md4c), así que aparecen el texto, las tablas, los títulos y las imágenes; las imágenes se acomodan a medida que se descargan sin que la página salte, y la portada muestra la imagen completa en vez de un recorte ampliado.
- El doble clic instala aunque la lista todavía esté cargando (empieza en cuanto termina); el panel de la página siempre está visible, así que la lista ya no se mueve debajo del mouse.
- Tema personalizado: una vista previa en vivo de una parte del launcher con los colores elegidos.
- Inicio en pantalla completa: el banner crece con la ventana en vez de achicarse a una franja.

# 1.1.5 (2026-10-11)

- Bedrock sin otro launcher: en Windows, EBALIA detecta Minecraft para Windows y lo inicia directamente (o abre su página de Microsoft Store); en Linux instala el entorno de Minecraft Linux, importa el APK de Minecraft que compraste e inicia el juego por su cuenta. Nuevo botón Desinstalar (en Windows primero se copian los mundos; en Linux los mundos se quedan salvo que elijas borrarlos).
- Explorar mods: una vista previa del mod elegido (imagen, descripción, descargas), doble clic para instalar, solo mods para la versión de la instancia y sin barra de carga cuando se cargan más resultados al final de la lista.
- Dos mods que necesitan versiones distintas de la misma dependencia (a menudo Fabric API) ya no frenan la instalación: se queda la más nueva.
- Todo traducido: los textos más nuevos, las preguntas frecuentes, el recorrido y los mensajes de instalación, descarga, Java, skins, Bedrock y actualización, en los diez idiomas.
- La rueda del mouse nunca cambia una lista desplegable (como la del idioma), ni siquiera después de hacerle clic.

# 1.1.4 (2026-10-11)

- Explorar mods carga los mods populares en cuanto se abre la pestaña (y cuando cambia la instancia o el proveedor); Enter busca aunque haya otra tarea en curso, y al llegar al final se cargan más resultados.
- Tema personalizado: elegí tus propios colores de acento y de fondo en Ajustes → Tema → Personalizado.
- La rueda del mouse desplaza la página: las listas desplegables (como la del idioma) solo cambian después de hacerles clic.
- El launcher ya no se pone gris detrás de un diálogo abierto; solo ignora los clics hasta que el diálogo se cierra.
- Cambiar el tema es instantáneo: solo se vuelven a aplicar los colores en vez de reconstruir todas las páginas.
- Los mods muestran sus propios íconos en la ventana de mods, para todos los cargadores (Fabric, Quilt, Forge, NeoForge, el viejo mcmod.info); los mods sin ícono reciben el de Modrinth.
- Skins muestra la skin que usás: la skin actual de la cuenta Microsoft o, en los perfiles no premium, la skin por defecto que les da Minecraft.
- Las versiones perdidas también usan texturas de Minecraft (bloque de pasto, calabaza de Halloween para las versiones de terror, mesa de trabajo).
- Nueva instancia: Personalizado muestra el bloque de pasto de Minecraft e Importar un ícono de importación limpio.
- Servidores en la página de la instancia: un botón Editar, los nombres completos de los servidores (sin los códigos de color de Minecraft) y los títulos Servidores y Capturas se quedan arriba cuando las listas están vacías.

# 1.1.3 (2026-10-08)

- Temas: EBALIA (verde), Deep Dark (turquesa), Nether (rojo), End (violeta), Océano (azul) y Cerezo (rosa), en Ajustes, aplicados al instante.
- Preguntas frecuentes en Guía y tutorial: importar packs, cierres del juego, comprar Minecraft, mods, Java, máquinas virtuales, dónde están los mundos, actualizaciones, uso portable, íconos e imágenes, versiones perdidas y avisos del antivirus.
- Las portadas de EBALIA Mods en Inicio usan arte oscuro (Nether, Deep Dark, mina abandonada).

# 1.1.2 (2026-10-08)

- Los cierres por el controlador gráfico y la falta de OpenGL muestran tu tarjeta gráfica (dedicada o integrada en el procesador) y abren su controlador: la página exacta del controlador para tarjetas NVIDIA GeForce y AMD Radeon RX, las páginas de Intel para Arc y los gráficos de 7.ª a 10.ª generación, y la herramienta de detección automática del fabricante (NVIDIA App, AMD Auto-Detect, Intel Driver & Support Assistant) cuando no se encuentra la página exacta. Las notebooks y las PC de marca también reciben la página de soporte de su fabricante (Dell, HP, Lenovo, ASUS, Acer, MSI, GIGABYTE, Samsung).
- Bedrock: el launcher instala el runtime de Microsoft Visual C++ que necesita el gestor de Bedrock (se detenía con «You need VC++ Runtime 14.14…»), y una vez instalado el gestor, un botón Jugar lo inicia.
- Los íconos de las instancias usan las texturas del propio Minecraft (pico de diamante, TNT, mesa de trabajo, creeper…), leídas del juego que descargó el launcher.
- Elegí tu propia imagen de perfil desde el menú de la cuenta (y volvé a la de Microsoft cuando quieras).
- Inicio muestra EBALIA Mods con portadas, arriba de las noticias.
- Una página de Patreon más clara: tarjeta de membresía, publicaciones ordenadas con fechas legibles y «Ver más», en Comunidad y Noticias.
- Se quitó el ajuste de la dirección del servicio de Patreon; ya no queda una franja vacía debajo de las páginas; «Soporte y comunidad» muestra su «y».

# 1.1.1 (2026-10-07)

- Máquinas virtuales y computadoras sin controlador gráfico: cuando Minecraft se cierra porque no hay OpenGL, el launcher ofrece iniciarlo con gráficos por software (OpenGL dibujado por el procesador; Mesa en Windows, el Mesa del sistema en Linux). También se puede activar en los ajustes de la instancia. Windows lo usa para Minecraft 1.13 y posteriores.
- Descargas del juego más rápidas: las conexiones a los servidores de Mojang se reutilizan y las bibliotecas se descargan en paralelo. Un Minecraft 1.21.1 nuevo (3.900 archivos, 850 MB) pasó de 143 a 28 segundos en nuestra prueba.
- Fondos de instancia: elegí cualquier imagen del launcher o una de tu computadora, desde la página de la instancia o sus ajustes.
- El ícono de la instancia también se puede cambiar desde los ajustes de la instancia.
- Cuando Minecraft se cierra con un error, un resumen dice qué falló, con un botón para arreglarlo: un mod faltante (instalado desde Modrinth para la versión y el cargador de la instancia), un cierre del controlador gráfico (enlace al controlador de AMD, NVIDIA o Intel, o gráficos por software), falta de OpenGL, la Java equivocada o muy poca memoria. El registro completo está a un clic.
- Importar carpetas .minecraft: la del Minecraft Launcher oficial y las carpetas de versión de los launchers que usan su formato (TLauncher, SKLauncher…), comprimidas o no. La versión de Minecraft y el cargador salen del perfil de la versión o, cuando no lo hay, de los propios mods. Estas carpetas también aparecen en la lista de instancias encontradas en la computadora, listas para importar. Los archivos de cuentas nunca se copian.
- Las carpetas de las instancias llevan el nombre de la instancia en vez de un número al azar; cambiarle el nombre a una instancia cambia el de su carpeta. Las instancias de versiones anteriores se renombran al iniciar y siguen funcionando (una carpeta en uso conserva su nombre anterior).
- Pegá instancias de otros launchers directamente en la carpeta de instancias (Prism Launcher, MultiMC, app de CurseForge, FTB App, ATLauncher, GDLauncher, Modrinth App, TLauncher o carpetas de versión de .minecraft, archivos .zip y .mrpack): el launcher las convierte en instancias con su versión de Minecraft y su cargador, también mientras está abierto. El original va a la papelera de instancias solo cuando eso funcionó.
- Cada importación comprueba que sus mods coincidan con la versión de Minecraft y el cargador del pack y nombra los que no.

# 1.1.0 (2026-10-05)

- El inicio de sesión de Microsoft está habilitado con el ID público de aplicación de EBALIA (`microsoft-client-id.txt`).
- Las cuentas Microsoft ahora muestran su imagen de perfil (la imagen de jugador de Xbox de la cuenta; la cara de la skin de Minecraft cuando no hay ninguna) en la barra lateral, la tarjeta del jugador de Inicio, el menú de la cuenta y Ajustes. Las imágenes se guardan en `mc/avatars`.
- Silence, la mascota de EBALIA, acompaña el recorrido interactivo y la página Guía y tutorial, animada: flota, respira, parpadea, le brillan los ojos, balancea un cuchillo, da un saltito en cada paso y se inclina hacia el control resaltado.
- Modo portable: un `portable.txt` junto al launcher (junto al AppImage o al bundle `.app`) guarda instancias, cuentas y ajustes en una carpeta `ebalia-data` al lado.
- Silence ahora tiene una hoja de sprites en pixel art con siete poses (quieta, feliz, sorprendida, guiñando, señalando, festejando, pensando), cada una con cuadros para hablar y parpadear; cada paso del recorrido usa su propia pose.
- Nuevos íconos de instancia de 16×16 dibujados a mano. Cualquier imagen de tu computadora se puede usar como ícono de instancia, al crear la instancia o después desde su página.
- La página de la instancia muestra sus capturas (clic para abrir) y su lista de servidores multijugador, donde se pueden agregar, copiar y quitar servidores; los campos desconocidos de `servers.dat` se conservan.
- Pasada de traducción: el recorrido interactivo, las páginas de Patreon y de creaciones, la desinstalación de Versiones perdidas, los mensajes de inicio de sesión de Microsoft y el actualizador del launcher están traducidos a los diez idiomas.
- Sitio web: las descargas del instalador y la portable una al lado de la otra, el SHA-256 de cada paquete con un botón para copiarlo y el comando para verificarlo, y un diseño para teléfonos rehecho donde todos los enlaces siguen visibles.
- Paquetes nuevos: AppImage de Linux para cualquier distribución con glibc 2.35+ (con Qt, libarchive y OpenSSL incluidos; probado en Debian, Fedora, Arch y openSUSE en CI) con actualizaciones desde la app, `.zip` portable de Windows, `.tar.gz` portable de Linux y compilaciones de macOS para Intel además de Apple Silicon, cada una con un `.zip` portable.
- Arreglo de las instancias Forge y NeoForge: las bibliotecas del cargador se combinan por artefacto y clasificador, así que Forge conserva su jar universal («Failed to find system mod: forge»). Verificado en el juego con Forge 26.3, 1.20.1, 1.16.5, 1.12.2, 1.8.9 y 1.7.10, NeoForge 26.3 y 1.21.1, Fabric y Quilt 26.3.
- Las instancias Forge, NeoForge, Fabric y Quilt instaladas con un launcher anterior se preparan de nuevo la próxima vez que tocás Jugar (los archivos descargados se reutilizan).
- NeoForge 26.x y NeoForge para 1.20.1 están disponibles; Fabric y Quilt eligen el cargador estable más nuevo; Forge 1.7.10–1.12.2 se instala con su instalador antiguo.
- Las Versiones perdidas se inician con Java directamente en Windows, macOS y Linux (sin Wine): a1.1.1, Alpha 1.0.16, 1.1 Patch 1, Ghost, 554e, NSSS y todas las demás versiones archivadas llegan a sus menús. Los archivos se comprueban con SHA-256 y cada versión guarda sus partidas dentro de su propia carpeta.
- Importación de modpacks desde `.mrpack`, `.zip` de CurseForge, Prism Launcher / MultiMC / PolyMC, app de CurseForge, FTB App, ATLauncher, GDLauncher, Modrinth App y Technic, con Forge, NeoForge, Fabric, Quilt o vanilla. Se manejan los ZIP envueltos en carpetas o hechos en macOS y Windows, los nombres de archivo en cualquier idioma, las URL espejo y los archivos opcionales de Modrinth, y los paquetes de recursos y shaders de CurseForge.
- Cuando hay una versión nueva, aparece un botón «Actualizar a …» en la barra lateral; descarga, verifica e instala la actualización sin visitar el sitio web.
- Se agregó un gestor externo experimental de Bedrock con instalación en segundo plano. Windows usa BedrockLauncher; Linux y macOS usan el motor de Android y requieren la compra en Google Play. La compra en Windows no se transfiere a Android. El juego real sigue sin verificarse en todas las plataformas.
- Se agregó la desinstalación de Versiones perdidas con confirmación y envío a la papelera.
- El progreso de las operaciones pasó a la barra de estado inferior.
- Se incluyó el runtime de Visual C++ de Windows y se agregó un acceso directo en el escritorio al instalador por usuario.
- Se agregó Actualizar y reiniciar dentro del launcher. Los paquetes de cada versión se verifican con SHA-256, se preparan y se prueba que arranquen antes de reemplazar los archivos del programa instalado. Los archivos anteriores del launcher y las descargas temporales se eliminan después de un arranque correcto. NixOS compila la versión con Nix; las rutas compartidas del almacén siguen sujetas a la recolección de basura de Nix.
- El actualizador nunca elimina instancias, Versiones perdidas, mundos, mods ni contenido descargado del usuario. Las actualizaciones se bloquean mientras hay juegos o instalaciones en curso. Si falla el reemplazo, se restauran los archivos anteriores del programa.
- El instalador de Windows detecta la carpeta de la instalación existente, incluido el formato de registro del instalador anterior, y la actualiza en el mismo lugar.
- Se agregó un recorrido interactivo de 14 pasos para el primer inicio, con controles resaltados, Atrás/Siguiente/Omitir, y que se puede repetir desde Guía y tutorial.
- Quienes usan 1.0.0 tienen que instalar 1.1.0 una vez para obtener el actualizador integrado; las versiones compatibles siguientes se actualizan desde el launcher.
- Se conservó la ubicación existente de los datos de la aplicación.

El launcher no incluye Minecraft, cuentas ni licencias del juego. Los paquetes de Windows y macOS no están firmados. El binario de Linux apunta a Ubuntu 24.04 x64; las demás distribuciones pueden compilar desde el código, incluido NixOS mediante el flake.
