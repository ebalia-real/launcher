// EBALIA's site: numbers in each language, the VHS screen (counter, channel changes, effects switch) and the mod filters.
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
    const target = Number(element.dataset.count);
    if (calm.matches || !effects()) { show(element, target); return; }
    const start = performance.now(), length = 1400;
    const step = now => {
      const t = Math.min(1, (now - start) / length);
      show(element, target * (1 - Math.pow(1 - t, 3)));
      if (t < 1) requestAnimationFrame(step); else show(element, target);
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

  // Mod filters: every tape, or only those on one store.
  const filters = [...document.querySelectorAll('.filter')];
  const tapes = [...document.querySelectorAll('.tape')];
  filters.forEach(filter => filter.addEventListener('click', () => {
    const store = filter.dataset.filter;
    filters.forEach(other => other.setAttribute('aria-pressed', String(other === filter)));
    tapes.forEach(tape => { tape.hidden = store !== 'all' && !tape.dataset.stores.split(' ').includes(store); });
    flash();
  }));
})();
