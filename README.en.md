<p align="center">
  <img src="site/assets/icon.png" width="96" height="96" alt="EBALIA Launcher icon">
</p>

<h1 align="center">EBALIA Launcher</h1>
<p align="center">Minecraft Java, modpacks, Lost Versions and EBALIA's creations.</p>
<p align="center">
  <a href="https://ebalia.gitgud.site/#descargas">Download</a> ·
  <a href="https://github.com/ebalia-real/launcher/releases">Releases</a> ·
  <a href="https://www.patreon.com/EBALIA">Patreon</a> ·
  <a href="https://github.com/ebalia-real/launcher/issues">Report a problem</a>
</p>

[![Build and tests](https://github.com/ebalia-real/launcher/actions/workflows/ebalia-launcher.yml/badge.svg)](https://github.com/ebalia-real/launcher/actions/workflows/ebalia-launcher.yml)

[Español](README.md) · **English**

## Download and install

The [official page](https://ebalia.gitgud.site/#descargas) follows your browser's language and shows the newest version published on GitHub or GitGud, plus each package's SHA-256 and the command to check it. While it is visible, it checks for releases again at most every 10 minutes. The links in this table always download the latest GitHub release (currently **1.2.0**).

| System | Package | How to install |
| --- | --- | --- |
| Windows 10 / 11 · x64 | [Installer `.exe`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-windows-x64-setup.exe) · [Portable `.zip`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-windows-x64-portable.zip) | Installer: run it and follow the wizard. It installs for your user only, without administrator rights, into `%LOCALAPPDATA%\Programs\EBALIA Launcher`, creates Start Menu and Desktop shortcuts, includes the Visual C++ DLLs and reuses an existing install. Portable: unzip it into any folder or USB drive and open `ebalia-launcher.exe`, nothing to install. |
| macOS · Apple Silicon | [`.dmg`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-macos-arm64.dmg) · [Portable `.zip`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-macos-arm64-portable.zip) | Open the disk image and copy `ebalia-launcher.app` to your Applications folder, or unzip the portable version wherever you like. |
| macOS · Intel | [`.dmg`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-macos-x64.dmg) · [Portable `.zip`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-macos-x64-portable.zip) | Same as Apple Silicon. |
| Linux · x86_64 (any distribution) | [AppImage](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-linux-x86_64.AppImage) · [Portable `.tar.gz`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-linux-x86_64-portable.tar.gz) | Bundles Qt, libarchive and OpenSSL: `chmod +x ebalia-linux-x86_64.AppImage` and open it. Runs with glibc 2.35+ (Ubuntu 22.04+, Debian 12+, Fedora, Arch, openSUSE Tumbleweed, Mint 21+). Without FUSE, start it with `APPIMAGE_EXTRACT_AND_RUN=1`. Alpine/musl and ARM need their own build. |
| Linux · Ubuntu 24.04 / Mint 22 · x64 | [Classic `.tar.gz`](https://github.com/ebalia-real/launcher/releases/latest/download/ebalia-linux-x64.tar.gz) | Uses the system Qt: install `libqt6widgets6 libqt6network6 libqt6concurrent6 qt6-qpa-plugins qt6-image-formats-plugins libarchive13t64`, extract it and open `bin/ebalia-launcher`. It does not run directly on NixOS. |
| NixOS | Project flake | Nix fetches the dependencies and builds the launcher. Follow the instructions below. |

The **portable** versions include a `portable.txt` file. While it sits next to the program (the `.exe` folder on Windows, the folder holding the `.app` on macOS, the AppImage's folder on Linux), instances, worlds, mods, accounts and settings are stored in the `ebalia-data` folder beside it, so you can carry everything on a USB drive. Delete `portable.txt` to go back to the system's regular data folder.

The Windows and macOS packages ship **without a publisher's digital signature** (on macOS, the first time, open the app with right-click → Open). Every release publishes `SHA256SUMS.txt` with the SHA-256 of all its files: check a download with `certutil -hashfile <file> SHA256` (Windows), `shasum -a 256 <file>` (macOS) or `sha256sum <file>` (Linux). A new version's packages appear only after its builds and tests have finished.

### NixOS

```sh
mkdir ebalia-launcher-1.2.4
cd ebalia-launcher-1.2.4
nix --extra-experimental-features 'nix-command flakes' build 'github:ebalia-real/launcher/v1.2.4?dir=ebalia-launcher'
./result/bin/ebalia-launcher
```

Create the folder without `sudo`. The first build can take a while and download dependencies; the package runs the tests while building and gives the launcher Java 8, 17, 21 and 25. Bedrock on NixOS also needs `services.flatpak.enable = true` in the system configuration (then rebuild it).

## What's new in 1.2.4

- **Local skins on 1.21.9 and 26.x,** which the launcher used to refuse.
- **Apply locally to all instances:** the chosen skin becomes every instance's, even before you create one; new instances get it as soon as they are installed.

In 1.2.3 the lost versions moved to EBALIA's own release. In 1.2.2:

- **Discover mods searches while you type:** mods, resource packs, shaders and the modpacks when creating an instance; the results come once you pause, without Enter, and searching no longer stutters.
- **EBALIA's own mods and the collaborations apart,** as on the site: Home shows only its own, and a new mod by EBALIA with other authors (like Neighbor.jar) joins the collaborations by itself, credited.
- **A new EBALIA Mods window:** cards like Home's with the picture, the downloads, who made it, the description in your language and CurseForge and Modrinth buttons; the total downloads at the top.
- **A tidier Bedrock page:** Play beside the version, the actions in one row and the help as links.

If you are on **1.0.0**, install **1.2.4 once** by hand, because 1.0.0 does not have this updater; after that, updates come from the launcher. You do not need to uninstall first or delete your data. Releases 1.1.1 to 1.2.1 brought, besides half the memory, screenshots for the lost versions, pixel-art icons, the Changelog section, the uninstaller and the mods' downloads up to date, among other things, resource packs and shaders in Discover mods, the "Already installed" mark, 3D skins and themes. [Notes for every release](ebalia-launcher/CHANGELOG.md).

## What you can do

| Section | Features |
| --- | --- |
| Home | A banner with four backgrounds that rotate every 9 s (arrows, dots and a pause that is remembered), the instance picker, **Play** with your name and avatar, the cards of **EBALIA**'s own mods (name, picture and downloads up to date; **See all** opens its own and the collaborations) with the **EBALIA · Patreon** button, the **Official website** with its preview, and **What's new**. |
| Instances | Independent profiles with their own worlds, mods and settings, in collapsible groups, with filters (all, vanilla, with mods, modpacks) and search. Copy, export to ZIP, change group and delete, which moves the instance and its worlds to a local trash after you confirm. |
| Create and import | Custom, ATLauncher, CurseForge, FTB, FTB Legacy, FTB App, Modrinth and Technic, with Vanilla, Fabric, Quilt, Forge or NeoForge. Imports `.mrpack`, CurseForge ZIPs, Prism Launcher/MultiMC, Technic and ATLauncher, CurseForge App and FTB App folders, EBALIA exports and packs, and `.minecraft` folders (TLauncher, SKLauncher), also from an https link or by dragging the file. It names the mods that do not match the version or loader. |
| Instance page | Loader, version, memory and group; **Play** installs whatever is missing. Mods, Resource packs, Shader packs and Worlds cards; up to 30 screenshots (F2 in game); servers from `servers.dat`; a live log with a filter, **Copy** and full screen; a background and icon of your choice. |
| Instance windows and settings | Mods with their icons to turn on, remove or add a local JAR; resource packs with their picture and description, to turn on or off (the last one turned on goes on top); one shader at a time, with Iris or Oculus; worlds exported and imported as ZIP. Settings: icon, name, memory (512–65536 MB), background, Java, **Log** and **Repair installation**. |
| Discover mods | Mods, resource packs and shader packs from Modrinth or CurseForge. Pick several with Ctrl/Shift and a single **Install (N)** installs them with their dependencies; what you already have shows as "Already installed". The mod page shows its gallery and full description. It searches while you type. |
| Lost versions | 15 versions from the EBALIA archive, each with its own screenshots and details. They run with Java 8 on Windows, macOS and Linux (a detected one or Mojang's jre-legacy, downloaded automatically), keep their saves in their own folder and have folder and log buttons. **Uninstall** moves them, worlds included, to the system trash. |
| My packs | **Save mods as a pack** on an instance, **Import pack** and **Export pack** (`.json`) and **Apply to an instance**. |
| Skins | A 3D model (drag to turn, **Front / back**, **2D view**). Import a 64 × 64 or 64 × 32 PNG, Classic or Slim, and apply it to the Microsoft account or locally to an instance (64 × 32 skins only with Classic; applying locally needs 64 × 64). |
| News and Community | Minecraft news and EBALIA · Patreon. Community: Fan arts, Servers, Learn, Support & community, and EBALIA · Patreon, with public posts without signing in and members-only posts after linking an active paid Patreon membership. |
| Guide & tutorial | A 14-step interactive tour with Silence, which starts by itself on first run and has sounds you can mute; the guide's steps and 12 questions & answers. |
| Changelog | Every launcher version with its date and what changed in each one, with search. It also opens from the version shown at the bottom of the sidebar. |
| Settings | Microsoft accounts or non-premium profiles, language, themes (EBALIA, Deep Dark, Nether, End, Ocean, Cherry and Custom), **Setup & diagnostics** (Java, OpenGL, OpenAL, Wine), the CurseForge key, the launcher's location and the data and trash folders, and **Uninstall** (only its own files and, if you tick it, your data). |

The interface and the tour are in ten languages: Spanish, English, Portuguese, German, French, Italian, Russian, Japanese, Korean and Simplified Chinese. The window works from 640 × 480; on Wayland with GNOME it draws its own title bar (`EBALIA_TITLE_BAR=system` or `custom` to choose). Java needs no setup: EBALIA uses a compatible Java you already have or downloads the official Mojang Java each version needs (on NixOS it uses the ones in the Nix package), and you can pick another in the instance settings. Closing the launcher ends it at once.

## Updates and user files

The launcher looks for a newer stable release 5 seconds after it opens and then every 6 hours. It asks GitHub first and uses GitGud only if GitHub fails or shows nothing newer. When there is one, **Update to &lt;version&gt;** appears in the sidebar and the prompt offers **Update and restart** or **Later**. Before updating, close your games and wait for installs to finish.

It downloads `SHA256SUMS.txt` and your system's package, checks the SHA-256, stages it in `<data>/updates` and makes sure it starts before replacing anything. If the copy or the second startup fails, the previous files are restored. On Windows and Linux (`.tar.gz`) only the files in the `update-files.json` manifest are replaced; on macOS, the `.app` bundle; with the AppImage, only the `.AppImage` file. The install folder must be writable by your user. On NixOS the new version is built with `nix build` and later launches use that build.

Updating keeps instances, Lost Versions, worlds, mods, accounts and settings (and, in portable mode, `portable.txt` and `ebalia-data`), and **does not change the Minecraft version of your instances**. It is not the same as uninstalling a lost version either: that separate action moves its folder and worlds to the trash, after a confirmation.

Data lives in the system's application data folder (`EBALIA/EBALIA Launcher`), in `ebalia-data` in portable mode, or wherever `EBALIA_DATA_DIR` points; **Settings → Data → Open data folder** opens it. Inside, instances go in `mc/instances`, shared files in `mc/libraries` and `mc/assets`, downloaded Java in `mc/java` and packs in `packs`.

## Bedrock · Beta

EBALIA launches your own copy of Minecraft Bedrock: it never downloads, unlocks or imports the game, and no licence is included.

| System | How it works | Where the game comes from |
| --- | --- | --- |
| Windows | Detects Minecraft for Windows and Minecraft Preview, plus Gaming Services, and opens them directly through `minecraft:` / `minecraft-preview:`, with no other launcher. Uninstall copies the worlds first; virtual machines without a GPU get a warning, because Bedrock uses DirectX. | The Microsoft Store (product 9NBLGGH2JHXJ) or the Xbox app (Game Pass). The Store keeps it updated. |
| Linux | The Android edition through [Minecraft Linux](https://minecraft-linux.github.io/), the Flathub app `io.mrarm.mcpelauncher`. EBALIA writes an `EBALIA` profile and starts it in profile mode, which keeps the purchase check. Needs Flatpak. | Google Play, with the account that bought the Android edition. Sign-in and downloads happen in Minecraft Linux's window. |
| macOS | Minecraft Linux for macOS: the newest stable `.dmg` from minecraft-linux/macos-builder, verified with its sha256. Needs Apple Silicon and macOS 15 or newer; Intel Macs are not supported. | Google Play, as on Linux. |

**Buying Bedrock for Windows does not unlock the Android download on Linux/macOS.** The Microsoft/Xbox account is used inside the game (Linux, macOS) or in the Xbox app (Windows). The page looks for the game once, and again after something you do from the launcher or with **Check again**; on Linux and macOS, the last game's log is kept in `bedrock/last-game.log` (inside the data folder). [Integration details](ebalia-launcher/BEDROCK.md).

## Build and contribute

The code lives in [`ebalia-launcher/`](ebalia-launcher). It needs CMake ≥ 3.21, a C++20 compiler, Qt ≥ 6.4 (Widgets, Network and Concurrent; Qt Test for the tests) and libarchive. md4c is bundled in `third_party`; with Python 3, two more tests run.

```sh
git clone https://github.com/ebalia-real/launcher.git
cd launcher/ebalia-launcher
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
cmake --install build --config Release --prefix package
```

The tests already use `QT_QPA_PLATFORM=offscreen`, so `ctest` works without a display. Windows uses MSVC and libarchive from vcpkg; macOS, Qt and libarchive from Homebrew; on NixOS, `nix build` inside `ebalia-launcher/` or `scripts/build-local.sh`. The CurseForge key (`-DEBALIA_CURSEFORGE_API_KEY` or `curseforge-api-key.txt`) and the Microsoft client ID (`-DEBALIA_MS_CLIENT_ID`) are passed at configure time.

The [GitHub Actions workflow](https://github.com/ebalia-real/launcher/blob/main/.github/workflows/ebalia-launcher.yml) builds and tests every push to `main` or pull request that touches `ebalia-launcher/` or the workflow (changes only to the website or the README do not trigger it): Linux (Ubuntu 24.04), AppImage (Ubuntu 22.04; checked to start on Debian 12, Fedora, Arch and openSUSE Tumbleweed), Windows (Windows 2022, Qt 6.8.3, MSVC 2022) and macOS (arm64 and Intel). A `v*` tag publishes the release, with its packages and `SHA256SUMS.txt`, only if all five jobs pass (Linux, AppImage, AppImage start-up on other distributions, Windows and macOS) and the tag matches the CMake version.

- [Technical documentation and configuration](ebalia-launcher/README.md) (in Spanish)
- [Release notes](ebalia-launcher/CHANGELOG.md)
- [Bedrock integration](ebalia-launcher/BEDROCK.md)
- [Patreon service](ebalia-launcher/services/patreon/README.md) (in Spanish)
- [Website code](site/README.md) and [its generator](site-src/README.md)
- [GitGud mirror repository](https://gitgud.io/castigarse/launcher): GitHub is the main repository. The mirror receives the code (without the GitHub workflows) and its releases, which link to the packages published on GitHub, through a manual step (`ebalia-launcher/scripts/publish-gitgud.sh`), so it can lag behind GitHub.

Report problems on [GitHub Issues](https://github.com/ebalia-real/launcher/issues). Include your system, the launcher version and the relevant log (the instance's live log has **Copy**). Do not post passwords, tokens or private keys.

## Disclaimer

An independent EBALIA project, not affiliated with Mojang or Microsoft. Minecraft accounts and licences are not included.
