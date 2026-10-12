# python3 site-src/card.py — the link preview card (1200x630, site/og/ebalia-<lang>.jpg) in each language, with ImageMagick.
import subprocess,os,shutil
SP=os.path.dirname(os.path.abspath(__file__))+'/'
SITE=os.path.normpath(SP+'../site')+'/'
M=shutil.which('magick') or 'magick'
# Liberation Sans Bold from the system (fc-match), VT323 from fonts/ (SIL Open Font License, fonts/OFL.txt).
VT=SP+'fonts/VT323-Regular.ttf';BOLD=subprocess.run(['fc-match','-f','%{file}','Liberation Sans:bold'],capture_output=True,text=True).stdout
icons=['disturbed','in-your-world','ooo-jar','backrooms-no-exit','twodoors-jar','salvation-archives-lobotomy-remake','the-world-is-alive']
T={
 'es':('Mods de terror para Minecraft','y un launcher propio','6,9 M DESCARGAS'),
 'en':('Minecraft horror mods','and its own launcher','6.9M DOWNLOADS'),
 'pt':('Mods de terror para Minecraft','e um launcher próprio','6,9 MI DOWNLOADS'),
 'fr':('Mods d’horreur pour Minecraft','et son propre launcher','6,9 M TÉLÉCHARGEMENTS'),
 'de':('Horror-Mods für Minecraft','und ein eigener Launcher','6,9 MIO. DOWNLOADS'),
}
STORES='CURSEFORGE · MODRINTH · WINDOWS · MACOS · LINUX'
def text(font,size,color,x,y,s):
    return ['-font',font,'-pointsize',str(size),'-fill',color,'-annotate',f'+{x}+{y}',s]
def aberrated(font,size,x,y,s,shift):
    return text(font,size,'#ff4a3d99',x+shift,y,s)+text(font,size,'#7cc6ff99',x-shift,y,s)+text(font,size,'#ece6da',x,y,s)
for lang,(line1,line2,stat) in T.items():
    out=SITE+f'og/ebalia-{lang}.jpg'
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
    a+=text(BOLD,48,'#ece6da',60,330,line1)+text(BOLD,48,'#ff4a3d',60,390,line2)
    a+=text(VT,52,'#ffffff',60,478,stat)+text(VT,30,'#a59c93',60,588,STORES)
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
    print(lang,os.path.getsize(out))
