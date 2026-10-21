// EBALIA's site: numbers in each language, the VHS screen (counter, channel changes, effects switch), the mod filters and
// the mods' live downloads, names and pictures.
(() => {
  const root = document.documentElement;
  const calm = window.matchMedia('(prefers-reduced-motion: reduce)');

  // Effects switch (scanlines, grain, tracking, flicker); remembered on this browser only.
  const button = document.querySelector('#effects');
  const state = document.querySelector('#effects-state');
  const effects = () => !root.classList.contains('no-vhs');
  const setEffects = on => {
    root.classList.toggle('no-vhs', !on);
    button.setAttribute('aria-pressed', String(on));
    state.textContent = on ? 'ON' : 'OFF';
    try { localStorage.setItem('ebalia-vhs', on ? 'on' : 'off'); } catch {}
  };
  let saved = null;
  try { saved = localStorage.getItem('ebalia-vhs'); } catch {}
  setEffects(saved ? saved === 'on' : !calm.matches);
  button.addEventListener('click', () => setEffects(!effects()));

  // Download counts written the way each language writes numbers (3.2M, 3,2 M, 3,2 Mio.).
  const formatter = () => new Intl.NumberFormat(root.lang || 'en', { notation: 'compact', maximumFractionDigits: 1 });
  const show = (element, value) => {
    element.textContent = element.hasAttribute('data-plain') ? String(Math.round(value)) : formatter().format(value);
  };
  const counted = new WeakSet();
  const formatAll = () => {
    for (const element of document.querySelectorAll('[data-count]')) {
      if (element.hasAttribute('data-animate') && !counted.has(element)) continue;
      show(element, Number(element.dataset.count));
    }
  };
  window.addEventListener('ebalia-language-change', formatAll);
  formatAll();

  // The hero numbers count up the first time they come into view, like a tape counter.
  const countUp = element => {
    counted.add(element);
    const target = () => Number(element.dataset.count); // read each frame: the live numbers may arrive while it rolls
    if (calm.matches || !effects()) { show(element, target()); return; }
    const start = performance.now(), length = 1400;
    const step = now => {
      const t = Math.min(1, (now - start) / length);
      show(element, target() * (1 - Math.pow(1 - t, 3)));
      if (t < 1) requestAnimationFrame(step); else show(element, target());
    };
    requestAnimationFrame(step);
  };
  const animated = [...document.querySelectorAll('[data-animate]')];
  if ('IntersectionObserver' in window) {
    const observer = new IntersectionObserver(entries => entries.forEach(entry => {
      if (entry.isIntersecting) { observer.unobserve(entry.target); countUp(entry.target); }
    }), { threshold: .4 });
    animated.forEach(element => { element.textContent = '0'; observer.observe(element); });
  } else animated.forEach(countUp);

  // The recorder counter in the corner: time since the page opened.
  const clock = document.querySelector('#osd-clock');
  const opened = Date.now();
  const tick = () => {
    const seconds = Math.floor((Date.now() - opened) / 1000);
    clock.textContent = [Math.floor(seconds / 3600), Math.floor(seconds / 60) % 60, seconds % 60].map(n => String(n).padStart(2, '0')).join(':');
  };
  tick(); setInterval(tick, 1000);

  // Changing channel: a short burst of static before jumping to the section.
  const flash = () => {
    if (!effects() || calm.matches) return;
    root.classList.remove('switching'); void root.offsetWidth; root.classList.add('switching');
    setTimeout(() => root.classList.remove('switching'), 260);
  };
  for (const link of document.querySelectorAll('a[href^="#"]')) {
    link.addEventListener('click', event => {
      const target = link.getAttribute('href').length > 1 ? document.querySelector(link.getAttribute('href')) : null;
      if (!target && link.getAttribute('href') !== '#') return;
      event.preventDefault(); flash();
      setTimeout(() => {
        (target || document.body).scrollIntoView({ behavior: calm.matches || !effects() ? 'auto' : 'smooth' });
        if (target) history.replaceState(null, '', link.getAttribute('href'));
      }, effects() && !calm.matches ? 140 : 0);
    });
  }
  // Rewind: back to the top, with the tape running backwards.
  document.querySelector('#rewind').addEventListener('click', () => {
    flash(); root.classList.add('rewinding');
    window.scrollTo({ top: 0, behavior: calm.matches || !effects() ? 'auto' : 'smooth' });
    setTimeout(() => root.classList.remove('rewinding'), 900);
  });

  // The launcher's release date, in the visitor's language; downloads.js announces a newer release.
  const dates = () => {
    for (const element of document.querySelectorAll('[data-release-date]')) {
      const date = new Date(element.dataset.date);
      element.textContent = isNaN(date) ? '' : new Intl.DateTimeFormat(root.lang || 'en', { dateStyle: 'long' }).format(date);
    }
  };
  window.addEventListener('ebalia-release', dates);
  window.addEventListener('ebalia-language-change', dates);
  dates();

  // The big download button: this computer's system, straight to its packages. Phones and tablets see every system.
  const hero = document.querySelector('#hero-download');
  const agent = `${navigator.userAgentData?.platform || ''} ${navigator.platform || ''} ${navigator.userAgent}`.toLowerCase();
  const mobile = /android|iphone|ipad|ipod|mobile/.test(agent) || (navigator.maxTouchPoints > 1 && /mac/.test(agent) && !/windows/.test(agent) && screen.width < 1100);
  const system = mobile ? '' : /win/.test(agent) ? 'windows' : /mac/.test(agent) ? 'macos' : /linux|x11|cros/.test(agent) ? 'linux' : '';
  const systemNames = { windows: 'Windows', macos: 'macOS', linux: 'Linux' };
  const label = () => {
    const { t } = window.ebaliaI18n;
    hero.textContent = system ? t('downloadFor', { system: systemNames[system] }) : t('viewDownloads');
  };
  window.addEventListener('ebalia-language-change', label);
  label();
  hero.addEventListener('click', () => {
    const button = system && document.querySelector(`#descargas [data-platform="${system}"]`);
    if (button) button.click();
    else document.querySelector('#descargas').scrollIntoView({ behavior: calm.matches || !effects() ? 'auto' : 'smooth' });
  });

  // Links to share: each language's address shows its own preview card in Discord, WhatsApp or Telegram.
  for (const button of document.querySelectorAll('[data-share]')) {
    button.addEventListener('click', async () => {
      const label = button.textContent;
      try { await navigator.clipboard.writeText(button.dataset.share); button.textContent = '✓'; }
      catch { window.prompt('', button.dataset.share); }
      setTimeout(() => { button.textContent = label; }, 1400);
    });
  }

  // Mod filters: every tape, or only those on one store (also the tapes that came in live).
  const filters = [...document.querySelectorAll('.filter')];
  let shownStore = 'all';
  const filterTapes = () => {
    document.querySelectorAll('#mods .tape').forEach(tape => { tape.hidden = shownStore !== 'all' && !tape.dataset.stores.split(' ').includes(shownStore); });
    document.querySelectorAll('#mods .group').forEach(group => { group.hidden = !group.querySelector('.tape:not([hidden])'); });
  };
  filters.forEach(filter => filter.addEventListener('click', () => {
    shownStore = filter.dataset.filter;
    filters.forEach(other => other.setAttribute('aria-pressed', String(other === filter)));
    filterTapes();
    flash();
  }));

  // EBALIA's mods as they are now. site-src/live.py reads CurseForge and Modrinth every hour onto the repository's live
  // branch; the page takes the downloads, names and pictures from there when it opens and every ten minutes, and a mod
  // that comes out (or is new) joins the shelf. Only plain text comes in, and only https addresses on the stores' hosts.
  const live = 'https://raw.githubusercontent.com/ebalia-real/launcher/live/mods.json';
  const hosts = { picture: ['media.forgecdn.net', 'cdn.modrinth.com'], curseforge: ['www.curseforge.com', 'curseforge.com'], modrinth: ['modrinth.com'] };
  const safe = (address, allowed) => {
    try { const url = new URL(address); return url.protocol === 'https:' && allowed.includes(url.hostname) ? url.href : ''; } catch { return ''; }
  };
  const say = key => (window.ebaliaI18n ? window.ebaliaI18n.t(key) : '');
  const translated = (tag, key) => { const element = document.createElement(tag); element.dataset.i18n = key; element.textContent = say(key); return element; };
  const setCount = (element, value) => {
    element.dataset.count = String(value);
    if (!element.hasAttribute('data-animate') || counted.has(element)) show(element, value); // a counter still to roll starts from the new number
  };
  const shelfFor = mod => document.querySelectorAll('.mods-panel .shelf')[mod.group === 'own' ? 0 : 1];
  const groupFor = mod => document.querySelectorAll('#mods .group .tapes')[mod.group === 'own' ? 0 : 1];
  const newTape = mod => {
    const group = groupFor(mod), model = document.querySelector('#mods .tape:not(.soon-card)');
    if (!group || !model) return null;
    const tape = model.cloneNode(true);
    tape.id = 'mod-' + mod.id; tape.classList.remove('soon-card', 'collab'); tape.hidden = false;
    tape.dataset.icon = ''; tape.querySelector('.tape-window img').removeAttribute('src');
    // EBALIA alone, or the other authors of a collaboration ("with PKFL · TingKH"), as the stores list them.
    const credit = tape.querySelector('.credit');
    const [key, names] = Array.isArray(mod.credit) ? mod.credit : [];
    if (['collabWith', 'remakeBy'].includes(key) && typeof names === 'string') {
      credit.className = 'credit'; credit.replaceChildren(translated('span', key), ' ' + names); tape.classList.add('collab');
    } else { credit.className = 'credit solo'; credit.textContent = 'EBALIA'; }
    const about = tape.querySelector('.tape-label p'); about.removeAttribute('data-i18n'); about.textContent = typeof mod.summary === 'string' ? mod.summary : '';
    group.insertBefore(tape, group.querySelector('.soon-card'));
    const shelf = shelfFor(mod), mini = document.querySelector('.mods-panel .mini');
    if (shelf && mini) {
      const copy = mini.cloneNode(true);
      copy.dataset.mod = mod.id; copy.href = '#mod-' + mod.id; copy.querySelector('img').removeAttribute('src');
      shelf.insertBefore(copy, [...shelf.querySelectorAll('.mini')].find(other => document.getElementById('mod-' + other.dataset.mod)?.classList.contains('soon-card')) || null);
    }
    return tape;
  };
  const update = mod => {
    if (!mod || typeof mod.id !== 'string' || typeof mod.name !== 'string') return;
    const tape = document.getElementById('mod-' + mod.id) || (mod.soon ? null : newTape(mod));
    if (!tape) return;
    const mini = document.querySelector(`.mini[data-mod="${CSS.escape(mod.id)}"]`);
    tape.querySelector('h3').textContent = mod.name;
    if (mini) { mini.title = mod.name; mini.querySelector('span').textContent = mod.name.split(' · ')[0]; }
    const picture = safe(mod.icon || '', hosts.picture);
    if (picture && picture !== tape.dataset.icon) { // the store has another picture now
      tape.dataset.icon = picture; tape.querySelector('.tape-window img').src = picture;
      if (mini) mini.querySelector('img').src = picture;
    }
    const stores = ['curseforge', 'modrinth'].filter(store => mod[store] && safe(mod[store].url, hosts[store]));
    tape.dataset.stores = stores.join(' ');
    tape.querySelector('.stores').replaceChildren(...stores.map(store => {
      const link = document.createElement('a');
      link.className = 'store ' + store; link.rel = 'noopener'; link.href = safe(mod[store].url, hosts[store]);
      link.textContent = (store === 'curseforge' ? 'CurseForge' : 'Modrinth') + ' ↗';
      return link;
    }));
    tape.classList.toggle('soon-card', Boolean(mod.soon));
    const meta = tape.querySelector('.count, .soon');
    if (mod.soon) { if (meta && !meta.classList.contains('soon')) { const soon = translated('span', 'soon'); soon.className = 'soon'; meta.replaceWith(soon); } return; }
    let count = meta && meta.classList.contains('count') ? meta : null;
    if (!count) { // out at last
      count = document.createElement('span'); count.className = 'count';
      count.append(document.createElement('b'), ' ', translated('span', 'downloadsLabel'));
      if (meta) meta.replaceWith(count); else tape.querySelector('.stores').before(count);
    }
    setCount(count.querySelector('b'), Math.max(0, Number(mod.downloads) || 0));
  };
  const refresh = () => fetch(live, { cache: 'no-cache' })
    .then(response => (response.ok ? response.json() : Promise.reject(response.status)))
    .then(data => {
      if (!data || data.format !== 1 || !Array.isArray(data.mods)) return;
      data.mods.forEach(update);
      document.querySelectorAll('#mods .tape-number').forEach((number, index) => { number.textContent = 'T-' + String(index + 1).padStart(2, '0'); });
      const [total, published] = document.querySelectorAll('.mods-stat b');
      if (total && Number(data.downloads) > 0) setCount(total, Number(data.downloads));
      if (published && Number(data.published) > 0) setCount(published, Number(data.published));
      filterTapes();
    })
    .catch(() => {}); // offline or not there yet: the page keeps the numbers it was built with
  refresh();
  setInterval(refresh, 10 * 60 * 1000);
})();
