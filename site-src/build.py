# python3 site-src/build.py — builds EBALIA's site: index.html (preview card in English) and es/ en/ pt/ fr/ de/ (preview card in that language).
# The page text always follows the visitor's language; the folders only choose the language of the shared-link card.
import json,base64,html,os,urllib.request
SP=os.path.dirname(os.path.abspath(__file__))+'/'
SITE=os.path.normpath(SP+'../site')+'/'
BASE='https://ebalia.gitgud.site/'
# GitGud's Cloudflare refuses image files on gitgud.site, so the card is served from the GitHub repository.
CARD='https://raw.githubusercontent.com/ebalia-real/launcher/main/site/og/ebalia-{}.jpg'
data=json.load(open(SP+'mods/mods.json'))
def uri(path,kind): return f'data:image/{kind};base64,'+base64.b64encode(open(path,'rb').read()).decode()
logo=uri(SP+'logo.png','png');preview=uri(SP+'preview.webp','webp')
release=json.load(urllib.request.urlopen(urllib.request.Request('https://api.github.com/repos/ebalia-real/launcher/releases/latest',headers={'User-Agent':'EBALIA-site-build'})))
VERSION=release['tag_name'].lstrip('v');DATE=release['published_at']
# Own mods first, then collaborations (from the authors on CurseForge and the team on Modrinth).
OWN=[('in-your-world','In Your World','modInYourWorld'),('ooo-jar','ooo.jar · 000.jar','modOoo'),('secret-01-proyect','Secret 01','modSecret')]
COLLAB=[('disturbed','DISTURBED','modDisturbed',('collabWith','LUNAR_CORE · VantablackDEV · ClouddyVR · monty14zx')),
        ('backrooms-no-exit','Backrooms: No Exit','modBackrooms',('collabWith','PKFL · TingKH')),
        ('twodoors-jar','.2doors.jar','modTwoDoors',('collabWith','PKFL · LookOut')),
        ('salvation-archives-lobotomy-remake','Salvation Archives: Lobotomy Remake','modSalvation',('remakeBy','FLYover67')),
        ('the-world-is-alive','My World is Alive','modAlive',('collabWith','PKFL'))]
def count(slug): m=data[slug];return (m.get('cfDownloads') or 0)+(m.get('mrDownloads') or 0)
TOTAL=sum(count(s) for s,*_ in OWN+COLLAB if s!='secret-01-proyect');PUBLISHED=len(OWN)+len(COLLAB)-1
def tape(n,slug,name,key,credit=None):
    m=data[slug];soon=slug=='secret-01-proyect'
    stores=[s for s,k in (('curseforge','cf'),('modrinth','mr')) if m.get(k)]
    links=''.join(([f'<a class="store curseforge" href="{html.escape(m["cf"])}" rel="noopener">CurseForge ↗</a>'] if m.get('cf') else [])+
                  ([f'<a class="store modrinth" href="{html.escape(m["mr"])}" rel="noopener">Modrinth ↗</a>'] if m.get('mr') else []))
    meta=('<span class="soon" data-i18n="soon">Próximamente</span>' if soon else
          f'<span class="count"><b data-count="{count(slug)}"></b> <span data-i18n="downloadsLabel">descargas</span></span>')
    who=f'<span class="credit"><span data-i18n="{credit[0]}"></span> {html.escape(credit[1])}</span>' if credit else '<span class="credit solo">EBALIA</span>'
    return (f'<article class="tape{" soon-card" if soon else ""}{" collab" if credit else ""}" id="mod-{slug}" data-stores="{" ".join(stores)}">'
            f'<div class="tape-window"><img src="{uri(SP+"mods/"+slug+".webp","webp")}" alt="" width="96" height="96" loading="lazy"><span class="tape-play" aria-hidden="true">▶</span></div>'
            f'<div class="tape-label"><span class="tape-number">T-{n:02d}</span><h3>{html.escape(name)}</h3>{who}<p data-i18n="{key}"></p>{meta}<div class="stores">{links}</div></div></article>')
def mini(slug,name):
    return f'<a class="mini" href="#mod-{slug}" title="{html.escape(name)}"><img src="{uri(SP+"mods/"+slug+".webp","webp")}" alt="" width="64" height="64"><span>{html.escape(name.split(" · ")[0])}</span></a>'
own=''.join(tape(n,*m) for n,m in enumerate(OWN,1));collab=''.join(tape(n,*m) for n,m in enumerate(COLLAB,len(OWN)+1))
shelf_own=''.join(mini(s,n) for s,n,*_ in OWN);shelf_collab=''.join(mini(s,n) for s,n,*_ in COLLAB)
CARDS={
 'en':('EBALIA · Minecraft horror mods and launcher','Minecraft horror mods (DISTURBED, In Your World, ooo.jar…) on CurseForge and Modrinth, and the EBALIA launcher for Windows, macOS and Linux.','en_US','EBALIA: Minecraft horror mods and its own launcher'),
 'es':('EBALIA · Mods de terror y launcher para Minecraft','Mods de terror para Minecraft (DISTURBED, In Your World, ooo.jar…) en CurseForge y Modrinth, y el launcher de EBALIA para Windows, macOS y Linux.','es_ES','EBALIA: mods de terror para Minecraft y un launcher propio'),
 'pt':('EBALIA · Mods de terror e launcher para Minecraft','Mods de terror para Minecraft (DISTURBED, In Your World, ooo.jar…) no CurseForge e no Modrinth, e o launcher da EBALIA para Windows, macOS e Linux.','pt_BR','EBALIA: mods de terror para Minecraft e um launcher próprio'),
 'fr':('EBALIA · Mods d’horreur et launcher pour Minecraft','Des mods d’horreur pour Minecraft (DISTURBED, In Your World, ooo.jar…) sur CurseForge et Modrinth, et le launcher d’EBALIA pour Windows, macOS et Linux.','fr_FR','EBALIA : mods d’horreur pour Minecraft et son propre launcher'),
 'de':('EBALIA · Horror-Mods und Launcher für Minecraft','Horror-Mods für Minecraft (DISTURBED, In Your World, ooo.jar …) auf CurseForge und Modrinth und der EBALIA-Launcher für Windows, macOS und Linux.','de_DE','EBALIA: Horror-Mods für Minecraft und ein eigener Launcher'),
}
def page(lang,folder):
    title,desc,locale,alt=CARDS[lang];up='../' if folder else '';url=BASE+(lang+'/' if folder else '')
    e=html.escape
    alternates=''.join(f'<link rel="alternate" hreflang="{l}" href="{BASE}{l}/">' for l in CARDS)+f'<link rel="alternate" hreflang="x-default" href="{BASE}">'
    locales=''.join(f'<meta property="og:locale:alternate" content="{CARDS[l][2]}">' for l in CARDS if l!=lang)
    head=(f'<!doctype html><html lang="{lang}"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">'
      f'<title>{e(title)}</title><meta name="description" content="{e(desc)}"><link rel="canonical" href="{url}">{alternates}'
      f'<meta property="og:type" content="website"><meta property="og:site_name" content="EBALIA"><meta property="og:title" content="{e(title)}"><meta property="og:description" content="{e(desc)}">'
      f'<meta property="og:url" content="{url}"><meta property="og:image" content="{CARD.format(lang)}"><meta property="og:image:type" content="image/jpeg"><meta property="og:image:width" content="1200"><meta property="og:image:height" content="630"><meta property="og:image:alt" content="{e(alt)}">'
      f'<meta property="og:locale" content="{locale}">{locales}<meta name="twitter:card" content="summary_large_image"><meta name="twitter:title" content="{e(title)}"><meta name="twitter:description" content="{e(desc)}"><meta name="twitter:image" content="{CARD.format(lang)}">'
      f'<link rel="icon" type="image/png" href="{logo}"><link rel="apple-touch-icon" href="{logo}"><meta name="theme-color" content="#b5322a">'
      f'<link rel="preconnect" href="https://fonts.googleapis.com"><link rel="preconnect" href="https://fonts.gstatic.com" crossorigin><link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=VT323&display=swap">'
      f'<link rel="stylesheet" href="{up}style.css?v=ebalia-3"><link rel="stylesheet" href="{up}vhs.css?v=ebalia-3"></head><body>')
    return head+f'''
<div class="vhs-layer scanlines" aria-hidden="true"></div><div class="vhs-layer grain" aria-hidden="true"></div><div class="vhs-layer tracking" aria-hidden="true"></div><div class="vhs-layer static" aria-hidden="true"></div>
<header><a class="brand" href="#"><img src="{logo}" alt="">EBALIA</a><div class="osd" aria-hidden="true"><span class="osd-play">▶ PLAY</span><span>SP</span><span id="osd-clock">00:00:00</span></div><nav aria-label="Main navigation"><a href="#launcher"><span class="channel">CH1</span><span data-i18n="navLauncher">Launcher</span></a><a href="#mods"><span class="channel">CH2</span><span data-i18n="navMods">Mods</span></a><a href="#comunidad"><span class="channel">CH3</span><span data-i18n="navCommunity">Comunidad</span></a><div class="language-picker"><button id="language-toggle" class="language-toggle" type="button" aria-expanded="false" aria-controls="language-menu" aria-label="Idioma"><svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="1.6" aria-hidden="true"><circle cx="12" cy="12" r="9"/><ellipse cx="12" cy="12" rx="4" ry="9"/><path d="M3 12h18M5 6.5h14M5 17.5h14"/></svg><span id="language-current">ES</span><svg class="language-chevron" viewBox="0 0 16 16" width="14" height="14" fill="none" stroke="currentColor" stroke-width="1.6" aria-hidden="true"><path d="m4 6 4 4 4-4"/></svg></button><div id="language-menu" class="language-menu" hidden><div class="language-heading" data-i18n="language">Idioma</div><div class="language-options"><button type="button" data-language="auto" aria-pressed="true"><span data-i18n="automatic">Automático (sistema)</span><span class="language-check" aria-hidden="true">✓</span></button><button type="button" data-language="es" aria-pressed="false"><span>Español</span><span class="language-check" aria-hidden="true">✓</span></button><button type="button" data-language="en" aria-pressed="false"><span>English</span><span class="language-check" aria-hidden="true">✓</span></button><button type="button" data-language="pt" aria-pressed="false"><span>Português</span><span class="language-check" aria-hidden="true">✓</span></button><button type="button" data-language="fr" aria-pressed="false"><span>Français</span><span class="language-check" aria-hidden="true">✓</span></button><button type="button" data-language="de" aria-pressed="false"><span>Deutsch</span><span class="language-check" aria-hidden="true">✓</span></button></div></div></div></nav></header>
<main><section class="hero"><div class="hero-head"><div class="eyebrow"><span class="rec">● REC</span> EBALIA · MINECRAFT</div><h1 class="glitch" data-i18n="hero"></h1><p data-i18n="intro"></p></div>
<div class="hero-grid">
<div class="screen panel launcher-panel"><span class="eyebrow">CH1 · LAUNCHER</span><h2 data-i18n="launcherHeading">El launcher de EBALIA</h2>
<p class="version-line"><span class="version-badge">v<span data-release-version>{VERSION}</span></span><span class="version-date"><span data-i18n="updatedOn">Actualizado el</span> <span data-release-date data-date="{DATE}"></span></span></p>
<p class="panel-body" data-i18n="launcherShort"></p>
<div class="panel-actions"><button type="button" id="hero-download" class="primary" data-system="windows"></button><a class="secondary" href="#descargas" data-i18n="allDownloads">Todas las descargas ↓</a></div>
<a class="mini-tv" href="#launcher" aria-hidden="true" tabindex="-1"><img src="{preview}" alt="" width="1400" height="788"></a></div>
<div class="screen panel mods-panel"><span class="eyebrow">CH2 · MODS</span><h2 data-i18n="modsHeading">Los mods de EBALIA</h2>
<p class="mods-stat"><b data-count="{TOTAL}" data-animate></b> <span data-i18n="statDownloads">descargas</span> · <b data-count="{PUBLISHED}" data-plain data-animate></b> <span data-i18n="statMods">mods</span></p>
<h3 class="shelf-title" data-i18n="ownMods">Individuales</h3><div class="shelf">{shelf_own}</div>
<h3 class="shelf-title" data-i18n="collabs">Colaboraciones</h3><div class="shelf">{shelf_collab}</div>
<a class="secondary" href="#mods" data-i18n="allMods">Ver todos los mods ↓</a></div>
</div></section>
<section id="launcher"><div class="sectionhead"><div><span class="eyebrow">CH1 · <span data-i18n="launcherEyebrow">EL LAUNCHER</span></span><h2 data-i18n="launcherHeading">El launcher de EBALIA</h2><p class="lead" data-i18n="launcherBody"></p></div></div><div class="tv"><div class="preview"><div class="windowbar"><i></i><i></i><i></i><span data-i18n="home">EBALIA / INICIO</span></div><img src="{preview}" alt="" width="1400" height="788" loading="lazy"></div><span class="tv-osd" aria-hidden="true">CH1 ▶</span></div></section>
<section class="features"><article><b data-i18n="feature1">01 / A TU MANERA</b><h2 data-i18n="worlds">Un lugar para tus mundos</h2><p data-i18n="worldsBody"></p></article><article><b data-i18n="feature2">02 / MÁS POR DESCUBRIR</b><h2>Lost Versions</h2><p data-i18n="lostBody"></p></article><article><b data-i18n="feature3">03 / CERCA DEL DESARROLLO</b><h2 data-i18n="connected">La comunidad, conectada</h2><p data-i18n="connectedBody"></p></article></section>
<section id="descargas"><div class="sectionhead"><div><span class="eyebrow" data-i18n="start">EMPEZÁ ACÁ</span><h2 data-i18n="where">Elegí dónde jugar.</h2></div><a href="#descargas" data-release-notes data-i18n="notes">Notas de la versión ↗</a></div><p id="release-status" role="status" data-i18n="checking">Consultando los paquetes publicados…</p><div class="downloads"><article><span class="platform">⊞</span><h3>Windows</h3><p>Windows 10 / 11 · x64</p><button type="button" data-platform="windows" class="download">Ver descarga</button><small data-i18n="windowsBody"></small></article><article><span class="platform">⌘</span><h3>macOS</h3><p>Apple Silicon · Intel</p><button type="button" data-platform="macos" class="download">Ver descarga</button><small data-i18n="macBody"></small></article><article><span class="platform">&gt;_</span><h3>Linux</h3><p data-i18n="linuxArch">x64</p><button type="button" data-platform="linux" class="download">Ver descarga</button><small data-i18n="linuxBody"></small></article></div><details><summary data-i18n="requirements">Requisitos y otras distribuciones de Linux</summary><p data-i18n="requirementsBody"></p></details><p class="fine" data-i18n="fine"></p></section>
<section id="mods"><div class="sectionhead"><div><span class="eyebrow">CH2 · <span data-i18n="modsEyebrow">MODS</span></span><h2 data-i18n="modsHeading">Los mods de EBALIA</h2><p class="lead" data-i18n="modsBody"></p></div><div class="filters" role="group" aria-label="CurseForge / Modrinth"><button type="button" class="filter" data-filter="all" aria-pressed="true" data-i18n="filterAll">Todos</button><button type="button" class="filter" data-filter="curseforge" aria-pressed="false">CurseForge</button><button type="button" class="filter" data-filter="modrinth" aria-pressed="false">Modrinth</button></div></div>
<div class="group"><h3 class="group-title"><span data-i18n="ownMods">Individuales</span></h3><p class="group-body" data-i18n="ownModsBody"></p><div class="tapes">{own}</div></div>
<div class="group"><h3 class="group-title"><span data-i18n="collabs">Colaboraciones</span></h3><p class="group-body" data-i18n="collabsBody"></p><div class="tapes">{collab}</div></div></section>
<section id="comunidad"><div class="sectionhead"><div><span class="eyebrow">CH3 · <span data-i18n="made">COMUNIDAD</span></span><h2 data-i18n="follow">Seguí lo que viene.</h2><p class="lead" data-i18n="followBody"></p></div></div><div class="links">
<article class="patreon"><h3>Patreon</h3><p data-i18n="patreonBody"></p><div class="stores"><a class="primary" href="https://www.patreon.com/c/ebalia/membership" rel="noopener" data-i18n="patreonMembership">Membresías ↗</a><a class="store" href="https://www.patreon.com/c/ebalia/posts" rel="noopener" data-i18n="patreonPosts">Publicaciones ↗</a><a class="store" href="https://www.patreon.com/c/ebalia/shop" rel="noopener" data-i18n="patreonShop">Tienda ↗</a></div></article>
<article><h3>Discord</h3><p data-i18n="discordBody"></p><div class="stores"><a class="store" href="https://discord.gg/ZrgMwfdwZB" rel="noopener" data-i18n="discordJoin">Unirse ↗</a></div></article>
<article><h3>YouTube</h3><p data-i18n="youtubeBody"></p><div class="stores"><a class="store" href="https://www.youtube.com/@EBALIA" rel="noopener" data-i18n="youtubeOpen">Ver el canal ↗</a></div></article>
<article><h3 data-i18n="codeHeading">Código</h3><p data-i18n="codeBody"></p><div class="stores"><a class="store" href="https://github.com/ebalia-real/launcher" rel="noopener">GitHub ↗</a><a class="store" href="https://gitgud.io/castigarse/launcher" rel="noopener">GitGud ↗</a></div></article>
</div></section></main>
<footer><span>EBALIA · <span data-i18n="footerLauncher">launcher</span> v<span data-release-version>{VERSION}</span></span><div><span class="share"><span data-i18n="shareLink">Copiar enlace</span>:{"".join(f'<button type="button" class="footer-button" data-share="{BASE}{l}/">{l.upper()}</button>' for l in CARDS)}</span><button type="button" id="rewind" class="footer-button">◀◀ <span data-i18n="rewind">Rebobinar</span></button><button type="button" id="effects" class="footer-button" aria-pressed="true"><span data-i18n="effects">Efectos VHS</span>: <b id="effects-state">ON</b></button><a href="https://www.curseforge.com/members/ebalia/projects" rel="noopener">CurseForge</a><a href="https://modrinth.com/user/EBALIA" rel="noopener">Modrinth</a><a href="https://github.com/ebalia-real/launcher/issues" data-i18n="issue">Reportar un problema</a></div><small data-i18n="independent">Proyecto independiente. No afiliado a Mojang ni Microsoft.</small></footer><dialog id="download-dialog" aria-labelledby="download-title"><button id="download-close" type="button" aria-label="Close">×</button><span class="eyebrow">EBALIA · LAUNCHER v<span data-release-version>{VERSION}</span></span><h2 id="download-title"></h2><p id="download-message" role="status"></p><div id="download-options"></div><div id="download-hash" hidden><p id="download-hash-help"></p><pre id="download-hash-command"></pre><a id="download-hash-all" target="_blank" rel="noopener">SHA256SUMS.txt ↗</a></div><section id="linux-guide" hidden aria-live="polite"></section><button id="download-retry" class="download" type="button"></button></dialog><script src="{up}i18n.js?v=ebalia-3"></script><script src="{up}linux-guide.js?v=ebalia-3"></script><script src="{up}downloads.js?v=ebalia-3"></script><script src="{up}site.js?v=ebalia-3"></script></body></html>
'''
open(SITE+'index.html','w',encoding='utf-8').write(page('en',False))
for lang in CARDS:
    os.makedirs(SITE+lang,exist_ok=True);open(SITE+lang+'/index.html','w',encoding='utf-8').write(page(lang,True))
print('version',VERSION,DATE,'total',TOTAL,'published mods',PUBLISHED,'page bytes',os.path.getsize(SITE+'index.html'))
