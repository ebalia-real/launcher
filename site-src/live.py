# python3 site-src/live.py [out.json] — EBALIA's mods as they are now: downloads, name, picture and summary from CurseForge
# and Modrinth, with the mods by EBALIA that catalog.json does not list yet. The site and the launcher read the result,
# which a workflow refreshes every hour on the repository's live branch (.github/workflows/live-data.yml); build.py
# writes the page from the copy in mods/live.json. CurseForge needs a key: CURSEFORGE_API_KEY, or
# ebalia-launcher/curseforge-api-key.txt.
import datetime,json,os,re,sys,urllib.request
SP=os.path.dirname(os.path.abspath(__file__))+'/'
OUT=sys.argv[1] if len(sys.argv)>1 else SP+'mods/live.json'

def get(url,key=None):
    headers={'User-Agent':'EBALIA-live-data (github.com/ebalia-real/launcher)','Accept':'application/json'}
    if key:headers['x-api-key']=key
    with urllib.request.urlopen(urllib.request.Request(url,headers=headers),timeout=30) as response:return json.load(response)

def simple(name):return re.sub(r'[^a-z0-9]','',(name or '').lower())

def curseforge(author,key):
    """EBALIA's public mods on CurseForge, by slug."""
    found={};index=0
    while True:
        page=get(f'https://api.curseforge.com/v1/mods/search?gameId=432&classId=6&authorId={author}&pageSize=50&index={index}',key)['data']
        for mod in page:
            if mod.get('isAvailable') and mod.get('status')==4: # approved and public
                found[mod['slug']]={'name':mod['name'],'summary':mod.get('summary',''),'downloads':int(mod.get('downloadCount',0)),
                                    'authors':[a.get('name','') for a in mod.get('authors',[]) if a.get('id')!=author],
                                    'icon':(mod.get('logo') or {}).get('thumbnailUrl',''),'url':(mod.get('links') or {}).get('websiteUrl') or f'https://www.curseforge.com/minecraft/mc-mods/{mod["slug"]}',
                                    'released':(mod.get('dateReleased') or '')[:10]}
        if len(page)<50:return found
        index+=50

def modrinth(user):
    """The mods EBALIA is part of on Modrinth, by slug, with the other members of each team."""
    found={}
    for project in get(f'https://api.modrinth.com/v2/user/{user}/projects'):
        if project.get('project_type')=='mod' and project.get('status') in ('approved','archived'):
            others=[m['user']['username'] for m in get(f"https://api.modrinth.com/v2/project/{project['id']}/members") if m['user']['id']!=user]
            found[project['slug']]={'name':project['title'],'authors':others,'summary':project.get('description',''),'downloads':int(project.get('downloads',0)),
                                    'icon':project.get('icon_url') or '','url':f'https://modrinth.com/mod/{project["slug"]}','released':(project.get('published') or '')[:10]}
    return found

def entry(base,cf,mr):
    store=cf or mr
    soon=bool(base.get('soon')) and not store
    name=base.get('name') or (store or {}).get('name')
    if store and store['name']!=base.get('storeName'):name=store['name'] # renamed on the store, or out at last
    mod={'id':base['id'],'group':base.get('group','own'),'name':name,'soon':soon}
    for field in ('text','credit'):
        if base.get(field):mod[field]=base[field]
    if store:
        mod['summary']=store['summary'];mod['icon']=(cf or {}).get('icon') or (mr or {}).get('icon','')
        mod['released']=min(x['released'] for x in (cf,mr) if x and x['released']) if any(x and x['released'] for x in (cf,mr)) else ''
    for store_name,data in (('curseforge',cf),('modrinth',mr)):
        if data:mod[store_name]={'url':data['url'],'downloads':data['downloads']}
    mod['downloads']=sum(data['downloads'] for data in (cf,mr) if data)
    return mod

def collect(key):
    catalog=json.load(open(SP+'mods/catalog.json',encoding='utf-8'))
    cf=curseforge(catalog['curseforgeAuthor'],key);mr=modrinth(catalog['modrinthUser'])
    used_cf,used_mr,mods=set(),set(),[]
    for base in catalog['mods']:
        c=base.get('curseforge') if base.get('curseforge') in cf else None
        m=base.get('modrinth') if base.get('modrinth') in mr else None
        if base.get('match'): # a mod still to come, found by its name once it is out
            c=c or next((slug for slug,data in cf.items() if slug not in used_cf and base['match'] in simple(data['name'])),None)
            m=m or next((slug for slug,data in mr.items() if slug not in used_mr and base['match'] in simple(data['name'])),None)
        used_cf.add(c);used_mr.add(m);mods.append(entry(base,cf.get(c),mr.get(m)))
    # New mods by EBALIA: the same mod on both stores goes together. EBALIA alone made it: one of its own; with other
    # authors: a collaboration, credited to them (as the catalog does by hand).
    def new(slug,c,m):
        others=list(dict.fromkeys((c or {}).get('authors',[])+(m or {}).get('authors',[])))
        return entry({'id':slug,'group':'collab' if others else 'own','credit':['collabWith',' · '.join(others)] if others else None},c,m)
    fresh=[]
    for slug,data in cf.items():
        if slug in used_cf:continue
        twin=next((s for s,d in mr.items() if s not in used_mr and simple(d['name'])==simple(data['name'])),None);used_mr.add(twin)
        fresh.append(new(slug,data,mr.get(twin)))
    for slug,data in mr.items():
        if slug not in used_mr:fresh.append(new(slug,None,data))
    own=[m for m in mods if m['group']=='own'];collab=[m for m in mods if m['group']!='own']
    soon=[m for m in own if m['soon']]
    mods=[m for m in own if not m['soon']]+[m for m in fresh if m['group']=='own']+soon+collab+[m for m in fresh if m['group']!='own']
    published=[m for m in mods if not m['soon']]
    return {'format':1,'updated':datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ'),
            'downloads':sum(m['downloads'] for m in published),'published':len(published),'mods':mods}

if __name__=='__main__':
    key=os.environ.get('CURSEFORGE_API_KEY','').strip()
    if not key and os.path.exists(SP+'../ebalia-launcher/curseforge-api-key.txt'):key=open(SP+'../ebalia-launcher/curseforge-api-key.txt').read().strip()
    if not key:sys.exit('A CurseForge API key is needed: CURSEFORGE_API_KEY or ebalia-launcher/curseforge-api-key.txt.')
    data=collect(key)
    if not data['mods'] or data['downloads']<=0:sys.exit('No mods came back; the previous data stays.')
    os.makedirs(os.path.dirname(os.path.abspath(OUT)),exist_ok=True)
    with open(OUT,'w',encoding='utf-8') as out:json.dump(data,out,ensure_ascii=False,indent=1);out.write('\n')
    print(f"{data['published']} mods, {data['downloads']} downloads -> {OUT}")
