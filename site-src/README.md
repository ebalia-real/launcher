# EBALIA site sources

- `build.py` writes `site/index.html` and `site/{es,en,pt,fr,de}/index.html` from `mods/mods.json` (the mods with their
  CurseForge and Modrinth links and download counts), the icons in `mods/`, `logo.png` and `preview.webp`. It reads the
  latest launcher release from GitHub for the version written in the page; the page itself refreshes it live.
- `card.py` draws the link preview card `site/og/ebalia-<lang>.jpg` (needs ImageMagick 7 and Liberation Sans).
  The card is served from `raw.githubusercontent.com` because gitgud.site refuses image files.
- To add a mod: add it to `mods/mods.json` and its icon to `mods/`, add it to `OWN` or `COLLAB` in `build.py`, add its
  description key in `site/i18n.js` for the five languages, then run both scripts.
