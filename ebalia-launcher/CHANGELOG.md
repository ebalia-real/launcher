# 1.1.9

- Mod pictures are sharp: the gallery viewer and the mod page's cover show the pictures as uploaded instead of
  Modrinth's 350 pixel previews (the preview shows while the original downloads), and CurseForge's strip uses its
  small previews.
- The lists that drop down (Mods, Resource packs, Shader packs and every other choice) open under their box with a
  single border; on Windows parts of the list were drawn outside it.
- The Resource packs and Shader packs cards have one link, like Mods: their window has the Download button.
- Shader packs, and resource packs without a picture, show their Modrinth project's icon (found by the file and kept
  in cache). The Use this shader and Turn on buttons keep the same space between icon and text as the others.
- Live log: the log window shows the game's output as it is written, follows it unless you scroll up, colours errors
  red and warnings yellow, filters lines, copies everything and no longer blocks the launcher behind it. Lost versions
  have a log button too.
- Lost versions: Uninstall sits with Play and Reinstall whenever the version is installed.
- Downloads show their percentage (and the size in the tooltip): lost versions and their Java, and instance installs
  next to what is being done.
- Bedrock looks for the game once, and again only after something done from the launcher (installing, updating,
  uninstalling, opening the Store or Minecraft Linux) or Check again, instead of every time the page opens.

# 1.1.8

- Fixed: the Install and Open page buttons of the mod page closed the launcher on every system (and so could the
  "Install <mod>" and "Download the driver" buttons of the crash summary).
- Discover has a single Install button: it installs every selected entry (Ctrl or Shift to choose several) and says
  how many; mods that have no version for the instance are listed while the rest install.
- Resource packs and shaders from CurseForge and Modrinth: choose Mods, Resource packs or Shader packs in Discover, or
  press Download on those cards of an instance. Shaders offer Iris (Fabric, Quilt, NeoForge) or Oculus (Forge) when
  the instance has neither. Vanilla instances can get resource packs.
- The mod page shows the mod's gallery: thumbnails, and the pictures large with ◀ ▶ and the arrow keys.
- The mod's icon is centred and no longer touches the panel's edge when the mod has no banner picture.
- The instance icon menu always shows the same pictures as the instances (it kept the bundled ones after the game's
  textures were read).
- Account pictures show the head of the skin the player wears (Microsoft) or gets by default (no premium) instead of
  the Xbox picture; a picture you chose still comes first. The Skins list shows each skin's head.
- Resource packs, shader packs and worlds open their own window from the instance page (click anywhere on the card),
  like mods: each pack with its picture and description, turn resource packs on or off and choose the shader pack the
  game loads, add ZIPs, remove (kept in removed-… inside the instance), open the folder, and download more. Worlds:
  open a world's folder, export it as a ZIP, import a world ZIP. The mods window also opens its folder.
- The Skins page shows the skin on a 3D model: drag to turn it, double-click to face it again, Front / back gives it
  half a turn. The flat view is one button away.
- On GNOME and other Wayland desktops that leave the title bar to the app, EBALIA draws its own: double-click it to
  maximize or restore, drag it to move the window, and resize from the edges. Windows, macOS, X11, KDE and the rest
  keep the system's title bar, where double-click already does this. EBALIA_TITLE_BAR=system or custom forces either.
- Skins can be deleted from the library (Delete skin, or the Delete key); the PNG they were imported from stays.
- Guide & tutorial: the guide's steps are cards side by side, all in view without scrolling, and the questions and
  answers have their own tab.
- Silence talks in the interactive tour: the text appears letter by letter with little voice blips, a chime marks
  each step and a short tune the end. The speaker button on the tour mutes it (remembered). The sounds are made by the
  launcher itself and play through the system (PlaySound on Windows, system sounds on macOS, PulseAudio or PipeWire on
  Linux): no sound files and no new libraries.
- Translations: the note on the default skin of profiles without premium was in English in seven languages; the tour
  now names Lost versions as the sidebar does in each language; the title bar's tooltips follow a change of language.

# 1.1.7

- Bedrock on Linux and macOS goes through Minecraft Linux's own profile mode, which checks the Google Play purchase and loads its compatibility module: the version you choose in EBALIA starts like from its Play button. APK import and the direct game start are gone (they skipped the purchase check).
- Bedrock lists the downloaded versions to choose from, keeps Minecraft Linux updated (Flathub on Linux; the verified Apple Silicon app on macOS 15+) and says why a game closed (purchase not confirmed, incompatible version, missing components).
- Bedrock on Windows: Minecraft and Minecraft Preview to choose from, a Game Pass (Xbox app) button, a Gaming Services check, a warning in virtual machines without a graphics card, and uninstall that first copies the worlds from where current versions keep them.
- Help for signing in to Xbox (Remote Connect on Linux and macOS).
- Creating a profile without premium before any game version was downloaded could freeze the launcher when it had been opened from a terminal (an empty game file path made the archive reader wait for keyboard input).

# 1.1.6

- Discover mods: the mod page reads descriptions like Modrinth does (Markdown with HTML inside, through md4c), so text, tables, headings and pictures all appear; pictures fall into place as they download without the page jumping, and the cover shows the whole picture instead of a zoomed crop.
- Double-click installs even while the list is still loading (it starts as soon as that ends); the page panel is always visible, so the list no longer moves under the mouse.
- Custom theme: a live preview of a piece of the launcher in the chosen colours.
- Home in full screen: the banner grows with the window instead of shrinking to a strip.

# 1.1.5

- Bedrock without another launcher: on Windows EBALIA detects Minecraft for Windows and starts it directly (or opens its Microsoft Store page); on Linux it installs the Minecraft Linux runtime, imports the Minecraft APK you bought and starts the game itself. New Uninstall button (Windows worlds are copied first; on Linux worlds stay unless you choose to delete them).
- Discover mods: a preview of the selected mod (picture, description, downloads), double-click to install, only mods for the instance's version, and no loading bar when more results load at the end of the list.
- Two mods that need different versions of the same dependency (often Fabric API) no longer stop the install: the newest is kept.
- Everything translated: the newer texts, the questions & answers, the tour and the installation, download, Java, skin, Bedrock and update messages, in all ten languages.
- The mouse wheel never changes a drop-down list (like the language), even after clicking it.

# 1.1.4

- Discover mods loads popular mods as soon as the tab opens (and when the instance or provider changes); Enter searches even while another task runs, and scrolling to the end loads more results.
- Custom theme: choose your own accent and background colours in Settings → Theme → Custom.
- The mouse wheel scrolls the page: drop-down lists (like the language) only change after you click them.
- The launcher no longer turns grey behind an open dialog; it just ignores clicks until the dialog closes.
- Changing the theme is instant: only the colours are reapplied instead of rebuilding every page.
- Mods show their own icons in the mods window, for every loader (Fabric, Quilt, Forge, NeoForge, old mcmod.info); mods without one get their Modrinth icon.
- Skins shows the skin you wear: the Microsoft account's current skin, or for profiles without premium the default skin Minecraft gives them.
- Lost versions use Minecraft textures too (grass block, jack o'lantern for horror versions, crafting table).
- New instance: Custom shows the Minecraft grass block and Import a clean import icon.
- Servers on the instance page: an Edit button, full server names (Minecraft colour codes removed) and the Servers and Screenshots titles stay at the top when the lists are empty.

# 1.1.3

- Themes: EBALIA (green), Deep Dark (teal), Nether (red), End (purple), Ocean (blue) and Cherry (pink), in Settings, applied at once.
- Questions & answers in Guide & tutorial: importing packs, crashes, buying Minecraft, mods, Java, virtual machines, where worlds are, updates, portable use, icons and pictures, lost versions and antivirus warnings.
- EBALIA Mods covers on Home use dark artwork (Nether, Deep Dark, mineshaft).

# 1.1.2

- Graphics driver crashes and missing OpenGL show your graphics card (dedicated or built into the processor) and open its driver: the exact driver page for NVIDIA GeForce and AMD Radeon RX cards, Intel's pages for Arc and 7th–10th generation graphics, and the maker's automatic detection tool (NVIDIA App, AMD Auto-Detect, Intel Driver & Support Assistant) when the exact page is not found. Laptops and branded PCs also get their maker's support page (Dell, HP, Lenovo, ASUS, Acer, MSI, GIGABYTE, Samsung).
- Bedrock: the launcher installs the Microsoft Visual C++ runtime the Bedrock manager needs (it stopped with "You need VC++ Runtime 14.14…"), and once the manager is installed a Play button starts it.
- Instance icons use Minecraft's own textures (diamond pickaxe, TNT, crafting table, creeper…), read from the game the launcher downloaded.
- Choose your own profile picture from the account menu (and go back to the Microsoft one at any time).
- Home shows EBALIA Mods with covers, above the news.
- A cleaner Patreon page: membership card, tidy posts with readable dates and "Show more", in Community and News.
- Removed the Patreon service address setting; no empty strip under the pages; "Support & community" shows its "&".

# 1.1.1

- Virtual machines and computers without a graphics driver: when Minecraft closes because there is no OpenGL, the launcher offers to start it with software graphics (OpenGL drawn by the processor; Mesa on Windows, the system Mesa on Linux). It can also be turned on in the instance settings. Windows uses it for Minecraft 1.13 and later.
- Faster game downloads: connections to Mojang's servers are reused and libraries download in parallel. A fresh Minecraft 1.21.1 (3,900 files, 850 MB) went from 143 to 28 seconds in our test.
- Instance backgrounds: choose any launcher picture or one from your computer, from the instance page or its settings.
- The instance icon can be changed from the instance settings too.
- When Minecraft closes with an error, a summary says what failed with a button to fix it: a missing mod (installed from Modrinth for the instance's version and loader), a graphics driver crash (link to the AMD, NVIDIA or Intel driver, or software graphics), no OpenGL, the wrong Java or too little memory. The full log is one click away.
- Import .minecraft folders: the official Minecraft Launcher's, and version folders of launchers that use its format (TLauncher, SKLauncher…), zipped or not. The Minecraft version and loader come from the version profile or, when there is none, from the mods themselves. These folders also appear in the list of instances found on the computer, ready to import. Account files are never copied.
- Instance folders carry the instance name instead of a random number; renaming an instance renames its folder. Instances from earlier versions are renamed on start and keep working (a folder in use keeps its old name).
- Paste instances of other launchers straight into the instances folder (Prism Launcher, MultiMC, CurseForge app, FTB App, ATLauncher, GDLauncher, Modrinth App, TLauncher or .minecraft version folders, .zip and .mrpack files): the launcher turns them into instances with their Minecraft version and loader, also while it is open. The original goes to the instance trash only when that worked.
- Every import checks that its mods match the pack's Minecraft version and loader and names the ones that do not.

# 1.1.0

- Microsoft sign-in is enabled with the EBALIA public application ID (`microsoft-client-id.txt`).
- Microsoft accounts now show their profile picture (the Xbox gamerpic of the account; the face of the Minecraft skin when there is none) in the sidebar, the Home player card, the account menu and Settings. Pictures are stored in `mc/avatars`.
- Silence, the EBALIA mascot, accompanies the interactive tour and the Guide & tutorial page, animated: floating, breathing, blinking, glowing eyes, a swaying knife, a hop on every step and a lean toward the highlighted control.
- Portable mode: a `portable.txt` next to the launcher (next to the AppImage or the `.app` bundle) keeps instances, accounts and settings in an `ebalia-data` folder beside it.
- Silence now has a pixel-art sprite sheet with seven poses (idle, happy, surprised, wink, pointing, cheering, thinking), each with talking and blinking frames; every tour step uses its own pose.
- New hand-drawn 16×16 instance icons. Any picture from your computer can be used as an instance icon, when creating the instance or later from its page.
- The instance page shows its screenshots (click to open) and its multiplayer server list, where servers can be added, copied and removed; unknown `servers.dat` fields are preserved.
- Translation pass: the interactive tour, Patreon and creations pages, Lost Versions uninstall, Microsoft sign-in messages and the launcher updater are translated into all ten languages.
- Website: installer and portable downloads side by side, SHA-256 of every package with a copy button and the command to verify it, and a reworked phone layout where every link stays visible.
- New packages: Linux AppImage for any glibc 2.35+ distribution (Qt, libarchive and OpenSSL bundled; tested on Debian, Fedora, Arch and openSUSE in CI) with in-app updates, portable Windows `.zip`, Linux portable `.tar.gz`, and macOS Intel builds alongside Apple Silicon, each with a portable `.zip`.
- Fixed Forge and NeoForge instances: loader libraries are merged by artifact and classifier, so Forge keeps its universal jar ("Failed to find system mod: forge"). Verified in game with Forge 26.3, 1.20.1, 1.16.5, 1.12.2, 1.8.9 and 1.7.10, NeoForge 26.3 and 1.21.1, Fabric and Quilt 26.3.
- Forge, NeoForge, Fabric and Quilt instances installed with an earlier launcher are prepared again the next time you press Play (downloaded files are reused).
- NeoForge 26.x and NeoForge for 1.20.1 are available; Fabric and Quilt pick the newest stable loader; Forge 1.7.10–1.12.2 installs through its legacy installer.
- Lost Versions start with Java directly on Windows, macOS and Linux (no Wine): a1.1.1, Alpha 1.0.16, 1.1 Patch 1, Ghost, 554e, NSSS and every other archived version reach their menus. Archives are checked with SHA-256 and each version keeps its saves inside its own folder.
- Modpack import from `.mrpack`, CurseForge `.zip`, Prism Launcher / MultiMC / PolyMC, CurseForge app, FTB App, ATLauncher, GDLauncher, Modrinth App and Technic, with Forge, NeoForge, Fabric, Quilt or vanilla. ZIPs wrapped in folders or made on macOS and Windows, file names in any language, Modrinth mirror URLs and optional files, and CurseForge resource packs and shaders are handled.
- When a new release is available an "Update to …" button appears in the sidebar; it downloads, verifies and installs the update without visiting the website.
- Added an experimental external Bedrock manager with background installation. Windows uses BedrockLauncher; Linux/macOS use the Android backend and require Google Play ownership. Windows ownership does not transfer to Android. Real gameplay remains unverified across platforms.
- Added Lost Version uninstall with confirmation and move-to-trash behavior.
- Moved operation progress to the bottom status bar.
- Bundled the Windows Visual C++ runtime and added a desktop shortcut to the per-user installer.
- Added Update and restart inside the launcher. Release packages are verified with SHA-256, staged and startup-tested before replacing installed program files. Previous launcher files and temporary downloads are removed after successful startup. NixOS builds the release with Nix; shared store paths remain subject to Nix garbage collection.
- The updater never removes instances, Lost Versions, worlds, mods or downloaded user content. Updates are blocked while games or installations are active. Failed replacement restores the previous program files.
- Windows setup detects the existing installation directory, including the previous installer registry format, and updates it in place.
- Added a 14-step first-run interactive tour, with highlighted controls, Back/Next/Skip, and replay from Guide & tutorial.
- Existing 1.0.0 users need to install 1.1.0 once to obtain the in-app updater; future compatible releases update from within the launcher.
- Preserved the existing application data location.

The launcher does not include Minecraft, accounts, or game licenses. Windows/macOS packages are unsigned. Linux binary targets Ubuntu 24.04 x64; other distributions can build from source, including NixOS via the flake.
