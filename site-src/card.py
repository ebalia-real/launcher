# python3 site-src/card.py — the link preview card (1200x630, site/og/ebalia-<lang>.jpg) in each language, with ImageMagick.
# The launcher shows the same card on Home, so it is copied to ebalia-launcher/resources/website/ too, and the launcher's
# other languages get theirs there only. `python3 site-src/card.py it ru` draws just those languages.
import json,subprocess,os,shutil,sys
SP=os.path.dirname(os.path.abspath(__file__))+'/'
SITE=os.path.normpath(SP+'../site')+'/'
LAUNCHER=os.path.normpath(SP+'../ebalia-launcher/resources/website')+'/'
M=shutil.which('magick') or 'magick'
# Liberation Sans Bold from the system (fc-match), VT323 from fonts/ (SIL Open Font License, fonts/OFL.txt).
VT=SP+'fonts/VT323-Regular.ttf';BOLD=subprocess.run(['fc-match','-f','%{file}','Liberation Sans:bold'],capture_output=True,text=True).stdout
icons=['disturbed','in-your-world','ooo-jar','backrooms-no-exit','twodoors-jar','salvation-archives-lobotomy-remake','the-world-is-alive']
T={
 'es':('Mods de terror para Minecraft','y un launcher propio'),
 'en':('Minecraft horror mods','and its own launcher'),
 'pt':('Mods de terror para Minecraft','e um launcher próprio'),
 'fr':('Mods d’horreur pour Minecraft','et son propre launcher'),
 'de':('Horror-Mods für Minecraft','und ein eigener Launcher'),
}
# Languages the launcher has and the site does not. Cyrillic is drawn with Liberation Sans, Japanese, Korean and Chinese
# with Noto Sans CJK; VT323 has neither.
LAUNCHER_ONLY={
 'it':('Mod horror per Minecraft','e un launcher tutto suo'),
 'ru':('Хоррор-моды для Minecraft','и собственный лаунчер'),
 'ja':('Minecraft のホラー Mod','そして独自のランチャー'),
 'ko':('Minecraft 공포 모드','그리고 자체 런처'),
 'zh':('Minecraft 恐怖模组','还有自家的启动器'),
}
# Every mod's downloads together, from site-src/live.py's last look at the stores (mods/live.json), as each language
# writes it: millions with one decimal (7M, 6,9 M), or tens of thousands in Japanese, Korean and Chinese (700万).
TOTAL=json.load(open(SP+'mods/live.json',encoding='utf-8'))['downloads']
STAT={'es':'{} M DESCARGAS','en':'{}M DOWNLOADS','pt':'{} MI DOWNLOADS','fr':'{} M TÉLÉCHARGEMENTS','de':'{} MIO. DOWNLOADS',
      'it':'{} MLN DI DOWNLOAD','ru':'{} МЛН СКАЧИВАНИЙ','ja':'{}万ダウンロード','ko':'{}만 다운로드','zh':'{} 万次下载'}
def downloads(lang):
    if lang in ('ja','ko','zh'):return STAT[lang].format(round(TOTAL/1e5)*10)
    millions=f'{round(TOTAL/1e5)/10:.1f}'.removesuffix('.0')
    return STAT[lang].format(millions if lang=='en' else millions.replace('.',','))
CJK={'ja':'Noto-Sans-CJK-JP-Bold','ko':'Noto-Sans-CJK-KR-Bold','zh':'Noto-Sans-CJK-SC-Bold'}
STORES='CURSEFORGE · MODRINTH · WINDOWS · MACOS · LINUX'
def text(font,size,color,x,y,s):
    return ['-font',font,'-pointsize',str(size),'-fill',color,'-annotate',f'+{x}+{y}',s]
def aberrated(font,size,x,y,s,shift):
    return text(font,size,'#ff4a3d99',x+shift,y,s)+text(font,size,'#7cc6ff99',x-shift,y,s)+text(font,size,'#ece6da',x,y,s)
chosen=sys.argv[1:] or [*T,*LAUNCHER_ONLY]
for lang,(line1,line2) in {**T,**LAUNCHER_ONLY}.items():
    stat=downloads(lang)
    if lang not in chosen:continue
    os.makedirs(LAUNCHER,exist_ok=True);out=SITE+f'og/ebalia-{lang}.jpg' if lang in T else LAUNCHER+f'ebalia-{lang}.jpg'
    tagline=CJK.get(lang,BOLD);numbers=VT if lang not in CJK and lang!='ru' else CJK.get(lang,BOLD);numberSize=52 if numbers==VT else 44
    a=[M,'-size','1200x630','-define','gradient:center=380,260','-define','gradient:radii=820,560','radial-gradient:#3b1717-#0b0a0c']
    # Tracking band across the card.
    a+=['(','-size','1200x30','gradient:#ffffff00-#ffffff1a','(','-size','1200x30','gradient:#ffffff1a-#ffffff00',')','-append','-resize','1200x34!',')','-geometry','+0+392','-composite']
    # Recorder display: a green play triangle, PLAY SP counter; REC with a red dot.
    a+=['-fill','#b8f08c','-draw','polygon 58,40 58,70 82,55']+text(VT,44,'#b8f08c',92,70,'PLAY')+text(VT,44,'#ece6da',190,70,'SP  00:13:37')
    a+=['-fill','#ff4a3d','-draw','circle 1046,55 1056,55']+text(VT,44,'#ff4a3d',1066,70,'REC')
    # Logo and name with colour bleed.
    a+=['(',SP+'logo.png','-filter','point','-resize','104x104',')','-geometry','+56+128','-composite']
    a+=aberrated(VT,168,186,236,'EBALIA',5)
    # Tagline, numbers and stores.
    a+=text(tagline,48,'#ece6da',60,330,line1)+text(tagline,48,'#ff4a3d',60,390,line2)
    a+=text(numbers,numberSize,'#ffffff',60,478,stat)+text(VT,30,'#a59c93',60,588,STORES)
    # The mods as small tapes on the right: two columns, the last one centred.
    x0,y0,s,g=880,92,112,12
    for n,icon in enumerate(icons):
        col,row=n%2,n//2
        x=x0+col*(s+g) if n<6 else x0+(s+g)//2
        y=y0+row*(s+g)
        a+=['-fill','#3a333f','-draw',f'roundrectangle {x-3},{y-3} {x+s+2},{y+s+2} 10,10']
        a+=['(',SP+f'mods/{icon}.webp','-filter','point','-resize',f'{s}x{s}!',')','-geometry',f'+{x}+{y}','-composite']
    # Scanlines and a vignette over everything.
    a+=['(','-size','1x3','xc:none','-fill','#00000055','-draw','point 0,2','-write','mpr:line','+delete','-size','1200x630','tile:mpr:line',')','-composite']
    a+=['(','-size','1200x630','radial-gradient:#00000000-#00000088',')','-composite']
    a+=['-strip','-interlace','JPEG','-sampling-factor','4:2:0','-quality','86',out]
    subprocess.run(a,check=True)
    if lang in T:shutil.copyfile(out,LAUNCHER+f'ebalia-{lang}.jpg')
    print(lang,os.path.getsize(out))
