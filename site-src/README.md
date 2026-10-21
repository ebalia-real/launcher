# EBALIA site sources

- `mods/catalog.json` lists EBALIA's mods: own or collaboration, the name shown, its description key in `site/i18n.js`,
  the credit, and where each one is on CurseForge and Modrinth. A mod still to come has `"soon": true` and a `match`
  that finds it by name once it is out.
- `live.py` asks CurseForge (the projects of EBALIA's author account) and Modrinth (EBALIA's projects) how the mods are
  now: downloads, name, picture and summary, and finds mods by EBALIA that the catalog does not list yet. It writes
  `mods/live.json`; it needs a CurseForge key (`CURSEFORGE_API_KEY`, or `ebalia-launcher/curseforge-api-key.txt`).
  `.github/workflows/live-data.yml` runs it every hour onto the repository's `live` branch, and from
  `raw.githubusercontent.com/ebalia-real/launcher/live/mods.json` the site (`site/site.js`) and the launcher take the
  downloads, names and pictures every time they open: a renamed mod, a new picture or a mod that comes out shows there
  without rebuilding anything.
- `build.py` writes `site/index.html` and `site/{es,en,pt,fr,de}/index.html` from `mods/live.json` and the catalog, with
  the small pictures in `mods/` while the stores keep the picture each was made from (`pictureFrom`), `logo.png` and
  `preview.webp`. It reads the latest launcher release from GitHub for the version written in the page; the page itself
  refreshes it live.
- `card.py` draws the link preview card `site/og/ebalia-<lang>.jpg` with the downloads in `mods/live.json` (needs
  ImageMagick 7, Liberation Sans and, for the launcher's Japanese, Korean and Chinese cards, Noto Sans CJK) and copies it
  to `ebalia-launcher/resources/website/`, where the launcher shows it on Home; the launcher's other languages get their
  card there only. The card is served from `raw.githubusercontent.com` because gitgud.site refuses image files.
- To refresh the site: `python3 site-src/live.py && python3 site-src/build.py && python3 site-src/card.py`.
- To add a mod by hand (a collaboration on another account, or a mod still to come): add it to `mods/catalog.json` and
  its picture to `mods/`, add its description key in `site/i18n.js` for the five languages, then refresh the site.
  Mods published on EBALIA's own accounts join by themselves.
