# Bedrock · Beta

EBALIA plays the player's own copy of Minecraft Bedrock. It never downloads, unlocks or imports the game itself.

## Windows
- Detects Minecraft for Windows (`Microsoft.MinecraftUWP`) and Minecraft Preview (`Microsoft.MinecraftWindowsBeta`) with
  `Get-AppxPackage`, plus Microsoft Gaming Services, which the game needs to start.
- Play opens the chosen edition through `minecraft:` / `minecraft-preview:`.
- Getting the game: the Microsoft Store page (product 9NBLGGH2JHXJ) or the Xbox app (Game Pass). The Store keeps it updated.
- Uninstall first copies `%APPDATA%\Minecraft Bedrock` (or `Minecraft Bedrock Preview`) and the old package `LocalState`
  worlds to EBALIA's data folder, then removes the package.
- Virtual machines without a GPU get a warning: Bedrock uses DirectX.

## Linux and macOS
The Android edition through Minecraft Linux (https://minecraft-linux.github.io/).
- Linux: the Flathub app `io.mrarm.mcpelauncher` (user or system installation; every flatpak call names the scope).
- macOS: the `Minecraft.Bedrock.Launcher.dmg` of the newest stable minecraft-linux/macos-builder release, verified with
  its sha256 digest and kept in EBALIA's application data. Needs Apple Silicon and macOS 15 or newer.
- Minecraft Linux's own window signs in to Google Play, checks the purchase, downloads versions and its compatibility
  module. EBALIA lists the downloaded versions (`versions/`), writes an `EBALIA` profile in `profiles/profiles.ini`
  (the version plus the mods of the profile the player uses) and starts it with Minecraft Linux's profile mode
  (`flatpak run … io.mrarm.mcpelauncher -p EBALIA` / `mcpelauncher-ui-qt --profile EBALIA`). That mode keeps the
  purchase check, so EBALIA never starts the game client directly.
- Starting newer versions, Minecraft Linux may show its own window asking to approve the Google credentials.
- Updates: `flatpak remote-ls --updates` / the releases of macos-builder, applied from the page.
- The game log is written to `bedrock/last-game.log`; the end of it tells a missing purchase, an incompatible version or
  missing components apart.

Microsoft / Xbox sign-in happens inside the game (Linux, macOS) or through the Xbox app (Windows).
