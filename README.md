<p align="center">
  <img src="site/assets/icon.png" width="96" height="96" alt="Icono de EBALIA Launcher">
</p>

<h1 align="center">EBALIA Launcher</h1>
<p align="center">Minecraft Java, modpacks, Lost Versions y las creaciones de EBALIA.</p>
<p align="center">
  <a href="https://ebalia.gitgud.site/#descargas">Descargar</a> ·
  <a href="https://github.com/ebalia-real/launcher/releases">Versiones</a> ·
  <a href="https://www.patreon.com/EBALIA">Patreon</a> ·
  <a href="https://github.com/ebalia-real/launcher/issues">Reportar un problema</a>
</p>

[![Compilación y pruebas](https://github.com/ebalia-real/launcher/actions/workflows/ebalia-launcher.yml/badge.svg)](https://github.com/ebalia-real/launcher/actions/workflows/ebalia-launcher.yml)

**Español** · [English](README.en.md)

## Descargar e instalar

La [página oficial](https://ebalia.gitgud.site/#descargas) detecta el idioma del navegador, muestra la versión más nueva publicada en GitHub o GitGud y el SHA-256 de cada paquete con el comando para comprobarlo. Mientras está visible, vuelve a consultar las versiones cada 10 minutos como máximo. Los enlaces de esta tabla descargan siempre la última versión de GitHub (ahora, **1.2.0**).

| Sistema | Paquete | Instalación |
| --- | --- | --- |
| Windows 10 / 11 · x64 | [Instalador `.exe`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-windows-x64-setup.exe) · [Portable `.zip`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-windows-x64-portable.zip) | Instalador: ejecutalo y seguí el asistente. Instala solo para tu usuario, sin permisos de administrador, en `%LOCALAPPDATA%\Programs\EBALIA Launcher`, crea accesos directos en el menú Inicio y el escritorio, incluye las DLL de Visual C++ y reutiliza una instalación existente. Portable: descomprimilo en cualquier carpeta o USB y abrí `ebalia-launcher.exe`, sin instalar. |
| macOS · Apple Silicon | [`.dmg`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-macos-arm64.dmg) · [Portable `.zip`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-macos-arm64-portable.zip) | Abrí la imagen y copiá `ebalia-launcher.app` a tus aplicaciones, o descomprimí la versión portable donde quieras. |
| macOS · Intel | [`.dmg`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-macos-x64.dmg) · [Portable `.zip`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-macos-x64-portable.zip) | Igual que Apple Silicon. |
| Linux · x86_64 (cualquier distribución) | [AppImage](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-linux-x86_64.AppImage) · [Portable `.tar.gz`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-linux-x86_64-portable.tar.gz) | Trae Qt, libarchive y OpenSSL: `chmod +x ebalia-linux-x86_64.AppImage` y abrilo. Funciona con glibc 2.35+ (Ubuntu 22.04+, Debian 12+, Fedora, Arch, openSUSE Tumbleweed, Mint 21+). Sin FUSE, abrilo con `APPIMAGE_EXTRACT_AND_RUN=1`. Alpine/musl y ARM necesitan su propia compilación. |
| Linux · Ubuntu 24.04 / Mint 22 · x64 | [`.tar.gz` clásico](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-linux-x64.tar.gz) | Usa el Qt del sistema: instalá `libqt6widgets6 libqt6network6 libqt6concurrent6 qt6-qpa-plugins qt6-image-formats-plugins libarchive13t64`, descomprimilo y abrí `bin/ebalia-launcher`. No funciona directamente en NixOS. |
| NixOS | Flake del proyecto | Nix prepara las dependencias y compila el launcher. Usá las instrucciones de abajo. |

Las versiones **portables** incluyen un archivo `portable.txt`. Mientras esté junto al programa (la carpeta del `.exe` en Windows, la que contiene la `.app` en macOS, la del AppImage en Linux), instancias, mundos, mods, cuentas y ajustes se guardan en la carpeta `ebalia-data` de al lado, y podés llevar todo en un USB. Borrá `portable.txt` para volver a la carpeta de datos normal del sistema.

Los paquetes de Windows y macOS se distribuyen **sin firma digital** del editor (en macOS, la primera vez abrí la app con clic derecho → Abrir). Cada versión publica `SHA256SUMS.txt` con el SHA-256 de todos sus archivos: comprobá cada descarga con `certutil -hashfile <archivo> SHA256` (Windows), `shasum -a 256 <archivo>` (macOS) o `sha256sum <archivo>` (Linux). Los paquetes de una versión nueva aparecen recién cuando terminaron sus compilaciones y pruebas.

### NixOS

```sh
mkdir ebalia-launcher-1.2.4
cd ebalia-launcher-1.2.4
nix --extra-experimental-features 'nix-command flakes' build 'github:ebalia-real/launcher/v1.2.4?dir=ebalia-launcher'
./result/bin/ebalia-launcher
```

Creá la carpeta sin `sudo`. La primera compilación puede tardar y descargar dependencias; el paquete corre las pruebas al compilar y le da al launcher Java 8, 17, 21 y 25. Para Bedrock en NixOS también hace falta `services.flatpak.enable = true` en la configuración del sistema (y reconstruirlo).

## Novedades de 1.2.4

- **Skins locales en 1.21.9 y 26.x,** que antes el launcher rechazaba.
- **Aplicar localmente a todas las instancias:** la skin elegida pasa a ser la de todas, incluso antes de crear una; las nuevas la reciben apenas se instalan.

En 1.2.3 las versiones perdidas pasaron a descargarse desde la publicación de EBALIA. En 1.2.2:

- **Explorar mods busca mientras escribís:** mods, paquetes de recursos, shaders y los modpacks al crear una instancia; los resultados llegan apenas hacés una pausa, sin Enter, y buscar ya no da tirones.
- **Mods propios y colaboraciones por separado,** como en la página: Inicio muestra solo los propios, y un mod nuevo de EBALIA con otros autores (como Neighbor.jar) entra solo en las colaboraciones, con su crédito.
- **La ventana EBALIA Mods es nueva:** tarjetas como las de Inicio con la imagen, las descargas, quién lo hizo, la descripción en tu idioma y botones de CurseForge y Modrinth; arriba, el total de descargas.
- **Bedrock más ordenado:** Jugar al lado de la versión, las acciones en una fila y la ayuda como enlaces.

Si venís de **1.0.0**, instalá **1.2.4 una vez** a mano, porque 1.0.0 no trae este actualizador; después las actualizaciones llegan desde el launcher. No hace falta desinstalar antes ni borrar tus datos. Entre 1.1.1 y 1.2.1 llegaron, además de la mitad de memoria, las capturas de las versiones perdidas, los íconos en pixel art, la sección Cambios, el desinstalador y las descargas de los mods al día, entre otras cosas, paquetes de recursos y shaders en Explorar mods, la marca «Ya instalado», skins en 3D y temas. [Notas de todas las versiones](ebalia-launcher/CHANGELOG.md) (en inglés).

## Qué podés hacer

| Sección | Funciones |
| --- | --- |
| Inicio | Banner con cuatro fondos que rotan cada 9 s (flechas, puntos y una pausa que se recuerda), selector de instancia, **Jugar** con tu nombre y avatar, las tarjetas de los mods propios de **EBALIA** (nombre, portada y descargas al día; **Ver todo** abre los propios y las colaboraciones) con el botón **EBALIA · Patreon**, la **Página oficial** con su vista previa, y **Novedades**. |
| Instancias | Perfiles independientes con mundos, mods y ajustes propios, en grupos plegables, con filtros (todas, vanilla, con mods, modpacks) y búsqueda. Copiar, exportar a ZIP, cambiar de grupo y eliminar, que mueve la instancia y sus mundos a una papelera local después de confirmar. |
| Crear e importar | Personalizado, ATLauncher, CurseForge, FTB, FTB Legacy, app de FTB, Modrinth y Technic, con Vanilla, Fabric, Quilt, Forge o NeoForge. Importa `.mrpack`, ZIP de CurseForge, Prism Launcher/MultiMC, Technic y ATLauncher, carpetas de las apps de CurseForge y FTB, exportaciones y packs de EBALIA y carpetas `.minecraft` (TLauncher, SKLauncher), también desde un enlace https o arrastrando el archivo. Avisa qué mods no coinciden con la versión o el cargador. |
| Página de la instancia | Cargador, versión, memoria y grupo; **Jugar** instala lo que falte. Tarjetas de Mods, Paquetes de recursos, Shaders y Mundos; hasta 30 capturas (F2 en el juego); servidores de `servers.dat`; registro en vivo con filtro, **Copiar** y pantalla completa; fondo e icono a elección. |
| Ventanas y ajustes de la instancia | Mods con su icono para activar, quitar o agregar un JAR local; paquetes de recursos con su imagen y descripción, para activarlos o desactivarlos (el último activado queda arriba); un shader a la vez, con Iris u Oculus; mundos que se exportan e importan en ZIP. Ajustes: icono, nombre, memoria (512–65536 MB), fondo, Java, **Registro** y **Reparar instalación**. |
| Explorar mods | Mods, paquetes de recursos y shaders de Modrinth o CurseForge. Elegí varios con Ctrl/Shift y un solo **Instalar (N)** los instala con sus dependencias; lo que ya tenés aparece como «Ya instalado». La página del mod muestra su galería y la descripción completa. Busca mientras escribís. |
| Versiones perdidas | 15 versiones del archivo de EBALIA, cada una con sus capturas y su ficha. Funcionan con Java 8 en Windows, macOS y Linux (uno detectado o el jre-legacy de Mojang, que se descarga solo), guardan sus partidas en su propia carpeta y tienen botones de carpeta y registro. **Desinstalar** las mueve con sus mundos a la papelera del sistema. |
| Mis packs | **Guardar mods como pack** en una instancia, **Importar pack** y **Exportar pack** (`.json`) y **Aplicar a una instancia**. |
| Skins | Modelo 3D (arrastrá para girar, **Frente / espalda**, **Vista 2D**). Importá un PNG de 64 × 64 o 64 × 32, Clásico o Brazos finos, y aplicalo a la cuenta Microsoft o localmente a una instancia (los de 64 × 32 solo con Clásico; aplicar localmente necesita 64 × 64). |
| Noticias y Comunidad | Noticias de Minecraft y de EBALIA · Patreon. Comunidad: Fan arts, Servidores, Aprender, Soporte y comunidad, y EBALIA · Patreon, con publicaciones públicas sin iniciar sesión y las exclusivas al vincular una membresía de pago activa de Patreon. |
| Guía y tutorial | Recorrido interactivo de 14 pasos con Silence, que arranca solo la primera vez y tiene sonidos que se pueden silenciar; pasos de la guía y 12 preguntas frecuentes. |
| Cambios | Todas las versiones del launcher con su fecha y qué cambió en cada una, en tu idioma, con búsqueda. También se abre desde la versión que aparece abajo de la barra lateral. |
| Ajustes | Cuentas Microsoft o perfiles no premium, idioma, temas (EBALIA, Deep Dark, Nether, End, Océano, Cerezo y Personalizado), **Preparación y diagnóstico** (Java, OpenGL, OpenAL, Wine), clave de CurseForge, la ubicación del launcher y las carpetas de datos y de papelera, y **Desinstalar** (solo sus archivos y, si lo marcás, tus datos). |

La interfaz y el recorrido están en diez idiomas: español, inglés, portugués, alemán, francés, italiano, ruso, japonés, coreano y chino simplificado. La ventana funciona desde 640 × 480; en Wayland con GNOME dibuja su propia barra de título (`EBALIA_TITLE_BAR=system` o `custom` para elegir). No hace falta configurar Java: EBALIA usa un Java compatible que ya tengas o descarga el oficial de Mojang que pide cada versión (en NixOS usa los del paquete Nix), y podés elegir otro en los ajustes de la instancia. Cerrar el launcher lo termina al instante.

## Actualizaciones y archivos del usuario

El launcher busca una versión estable nueva 5 segundos después de abrirse y luego cada 6 horas. Consulta GitHub primero y usa GitGud solo si GitHub falla o no muestra nada más nuevo. Cuando hay una, aparece **Actualizar a &lt;versión&gt;** en la barra lateral y el aviso permite **Actualizar y reiniciar** o **Más tarde**. Antes de actualizar, cerrá las partidas y esperá a que terminen las instalaciones.

Descarga `SHA256SUMS.txt` y el paquete de tu sistema, comprueba el SHA-256, lo prepara en `<datos>/updates` y prueba que arranque antes de reemplazar nada. Si falla la copia o el segundo arranque, se restauran los archivos anteriores. En Windows y Linux (`.tar.gz`) se reemplazan solo los archivos del manifiesto `update-files.json`; en macOS, el bundle `.app`; con AppImage, solo el archivo `.AppImage`. La carpeta de instalación debe permitir escritura para tu usuario. En NixOS se compila la versión nueva con `nix build` y los siguientes inicios usan esa compilación.

Actualizar conserva instancias, Versiones perdidas, mundos, mods, cuentas y ajustes (y en modo portable, `portable.txt` y `ebalia-data`), y **no cambia la versión de Minecraft de tus instancias**. Tampoco equivale a desinstalar una versión perdida: esa acción separada mueve su carpeta y sus mundos a la papelera, después de una confirmación.

Los datos están en la carpeta de datos de aplicaciones del sistema (`EBALIA/EBALIA Launcher`), en `ebalia-data` en modo portable o donde indique `EBALIA_DATA_DIR`; **Ajustes → Datos → Abrir carpeta de datos** la abre. Adentro, las instancias van en `mc/instances`, los archivos compartidos en `mc/libraries` y `mc/assets`, el Java descargado en `mc/java` y los packs en `packs`.

## Bedrock · Beta

EBALIA abre tu propia copia de Minecraft Bedrock: nunca descarga, desbloquea ni importa el juego, y no incluye una licencia.

| Sistema | Cómo funciona | De dónde sale el juego |
| --- | --- | --- |
| Windows | Detecta Minecraft para Windows y Minecraft Preview, además de Gaming Services, y los abre directamente con `minecraft:` / `minecraft-preview:`, sin otro launcher. Desinstalar copia antes los mundos; en máquinas virtuales sin GPU avisa, porque Bedrock usa DirectX. | Microsoft Store (producto 9NBLGGH2JHXJ) o la app de Xbox (Game Pass). La Store lo mantiene actualizado. |
| Linux | La edición Android con [Minecraft Linux](https://minecraft-linux.github.io/), la app de Flathub `io.mrarm.mcpelauncher`. EBALIA crea el perfil `EBALIA` y lo inicia en modo perfil, que mantiene la comprobación de compra. Necesita Flatpak. | Google Play, con la cuenta que compró la edición Android. El inicio de sesión y las descargas se hacen en la ventana de Minecraft Linux. |
| macOS | Minecraft Linux para macOS: el `.dmg` estable más nuevo de minecraft-linux/macos-builder, verificado con su sha256. Necesita Apple Silicon y macOS 15 o posterior; los Mac Intel no son compatibles. | Google Play, igual que en Linux. |

**Comprar Bedrock para Windows no habilita la descarga Android en Linux/macOS.** La cuenta Microsoft/Xbox se usa dentro del juego (Linux, macOS) o en la app de Xbox (Windows). La página busca el juego una vez y de nuevo después de lo que hagas desde el launcher o con **Comprobar de nuevo**; en Linux y macOS, el registro de la última partida queda en `bedrock/last-game.log` (dentro de la carpeta de datos). [Detalles de la integración](ebalia-launcher/BEDROCK.md) (en inglés).

## Compilar y colaborar

El código está en [`ebalia-launcher/`](ebalia-launcher). Requiere CMake ≥ 3.21, un compilador C++20, Qt ≥ 6.4 (Widgets, Network y Concurrent; Qt Test para las pruebas) y libarchive. md4c viene incluido en `third_party`; con Python 3 se suman dos pruebas.

```sh
git clone https://github.com/ebalia-real/launcher.git
cd launcher/ebalia-launcher
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
cmake --install build --config Release --prefix package
```

Las pruebas ya usan `QT_QPA_PLATFORM=offscreen`, así que `ctest` funciona sin pantalla. Windows usa MSVC y libarchive de vcpkg; macOS, Qt y libarchive de Homebrew; en NixOS, `nix build` dentro de `ebalia-launcher/` o `scripts/build-local.sh`. La clave de CurseForge (`-DEBALIA_CURSEFORGE_API_KEY` o `curseforge-api-key.txt`) y el ID de cliente de Microsoft (`-DEBALIA_MS_CLIENT_ID`) se pasan al configurar.

El [workflow de GitHub Actions](https://github.com/ebalia-real/launcher/blob/main/.github/workflows/ebalia-launcher.yml) compila y prueba cada push a `main` o pull request que toque `ebalia-launcher/` o el workflow (los cambios solo en la web o en el README no lo disparan): Linux (Ubuntu 24.04), AppImage (Ubuntu 22.04; se comprueba que arranque en Debian 12, Fedora, Arch y openSUSE Tumbleweed), Windows (Windows 2022, Qt 6.8.3, MSVC 2022) y macOS (arm64 e Intel). Una etiqueta `v*` publica la versión, con sus paquetes y `SHA256SUMS.txt`, solo si pasan los cinco trabajos (Linux, AppImage, arranque del AppImage en otras distribuciones, Windows y macOS) y la etiqueta coincide con la versión de CMake.

- [Documentación técnica y configuración](ebalia-launcher/README.md)
- [Notas de las versiones](ebalia-launcher/CHANGELOG.md) (en inglés)
- [Integración de Bedrock](ebalia-launcher/BEDROCK.md) (en inglés)
- [Servicio de Patreon](ebalia-launcher/services/patreon/README.md)
- [Código de la web](site/README.md) y [su generador](site-src/README.md) (en inglés)
- [Repositorio espejo en GitGud](https://gitgud.io/castigarse/launcher): GitHub es el repositorio principal. El espejo recibe el código (sin los workflows de GitHub) y sus versiones, que enlazan a los paquetes publicados en GitHub, con un paso manual (`ebalia-launcher/scripts/publish-gitgud.sh`), así que puede ir detrás de GitHub.

Reportá los problemas en [GitHub Issues](https://github.com/ebalia-real/launcher/issues). Incluí el sistema, la versión del launcher y el registro relevante (el registro en vivo de la instancia tiene **Copiar**). No publiques contraseñas, tokens ni claves privadas.

## Aviso

Proyecto independiente de EBALIA, no afiliado a Mojang ni Microsoft. Las cuentas y licencias de Minecraft no se incluyen.
