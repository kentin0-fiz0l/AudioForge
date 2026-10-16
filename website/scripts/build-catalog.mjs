#!/usr/bin/env node
// Build the website's plugin catalog from the plugins themselves, so it cannot drift:
//
//   plugins/<Name>/CMakeLists.txt   display name, type (instrument, effect, MIDI) and formats
//   plugins/<Name>/Source/*.cpp     parameter names (and ranges, where they are plain numbers)
//   plugins/<Name>/plugin.json      the written part: tagline, category, description, features
//   website/public/demos/<id>.mp3   an audio demo, if tools/render-demos.sh made one
//
// Writes website/src/lib/catalog.json. Every plugin folder must have a plugin.json.
//   node website/scripts/build-catalog.mjs           write the catalog
//   node website/scripts/build-catalog.mjs --check   fail if the committed catalog is out of date (CI)

import { readFileSync, readdirSync, writeFileSync, existsSync, statSync } from 'node:fs';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const pluginsDir = join(root, 'plugins');
const outFile = join(root, 'website', 'src', 'lib', 'catalog.json');
const CATEGORIES = ['mixing', 'creative', 'synthesis', 'utility', 'midi'];

function cmakeValue(cmake, key) {
  const m = cmake.match(new RegExp(`\\b${key}\\s+("([^"]*)"|[^\\s)]+)`));
  return m ? (m[2] ?? m[1]) : undefined;
}

// The arguments of a call starting at `open` (the index of its '('), split at top-level commas
function callArgs(src, open) {
  const args = [];
  let depth = 0, start = open + 1, inString = false;
  for (let i = open; i < src.length; i++) {
    const c = src[i];
    if (inString) {
      if (c === '\\') i++;
      else if (c === '"') inString = false;
      continue;
    }
    if (c === '"') inString = true;
    else if ('({['.includes(c)) depth++;
    else if (')}]'.includes(c)) {
      depth--;
      if (depth === 0) { args.push(src.slice(start, i).trim()); return args; }
    } else if (c === ',' && depth === 1) { args.push(src.slice(start, i).trim()); start = i + 1; }
  }
  return args;
}

// C++ source without its comments, leaving string literals alone
function stripComments(src) {
  let out = '', i = 0;
  while (i < src.length) {
    const c = src[i], next = src[i + 1];
    if (c === '"') {
      let j = i + 1;
      while (j < src.length && src[j] !== '"') j += src[j] === '\\' ? 2 : 1;
      out += src.slice(i, j + 1); i = j + 1;
    } else if (c === '/' && next === '/') {
      while (i < src.length && src[i] !== '\n') i++;
    } else if (c === '/' && next === '*') {
      const end = src.indexOf('*/', i + 2); i = end < 0 ? src.length : end + 2;
    } else { out += c; i++; }
  }
  return out;
}

// A parameter name: a string literal, or literals joined with values built in a loop
// ("Map " + idx + " Enable" becomes "Map N Enable")
function nameOf(arg) {
  if (!arg) return undefined;
  const parts = arg.split(/\s*\+\s*/);
  if (!parts.some((x) => /^"/.test(x))) return undefined;
  return parts.map((x) => (/^"/.test(x) ? x.slice(1, -1) : 'N')).join('').replace(/\s+/g, ' ').trim();
}
const number = (s) => {
  const m = s?.trim().match(/^-?\d+(\.\d+)?(e-?\d+)?f?$/i);
  return m ? Number(s.trim().replace(/f$/i, '')) : undefined;
};
const fmt = (n) => (Number.isInteger(n) ? String(n) : String(Number(n.toPrecision(4))));

function rangeOf(kind, args) {
  if (kind === 'Bool') return 'on / off';
  if (kind === 'Choice') {
    const list = args[2]?.match(/"((?:[^"\\]|\\.)*)"/g);
    return list ? list.map((s) => s.slice(1, -1)).join(', ') : '';
  }
  // Float or Int: (id, name, min, max, default) or (id, name, NormalisableRange(min, max, ...), default)
  let lo = number(args[2]), hi = number(args[3]);
  const range = args[2]?.match(/NormalisableRange\s*<[^>]*>\s*[({]\s*([^,]+),\s*([^,)}]+)/);
  if (range) { lo = number(range[1]); hi = number(range[2]); }
  return lo !== undefined && hi !== undefined ? `${fmt(lo)} to ${fmt(hi)}` : '';
}

function parametersOf(folder) {
  const sourceDir = join(folder, 'Source');
  if (!existsSync(sourceDir)) return [];
  const params = [];
  const seen = new Set();
  const files = readdirSync(sourceDir).filter((f) => f.endsWith('.cpp') || f.endsWith('.h')).sort();
  for (const file of files) {
    const src = stripComments(readFileSync(join(sourceDir, file), 'utf8'));
    const re = /AudioParameter(Float|Int|Bool|Choice)\s*>?\s*\(/g;
    let m;
    while ((m = re.exec(src))) {
      const args = callArgs(src, m.index + m[0].length - 1);
      const name = nameOf(args[1]);
      if (!name || seen.has(name)) continue;
      seen.add(name);
      params.push({ name, range: rangeOf(m[1], args) });
    }
  }
  return params;
}

function typeOf(cmake) {
  if (cmakeValue(cmake, 'IS_MIDI_EFFECT') === 'TRUE') return 'midi';
  if (cmakeValue(cmake, 'IS_SYNTH') === 'TRUE') return 'instrument';
  return 'effect';
}

function entryFor(name) {
  const folder = join(pluginsDir, name);
  const cmake = readFileSync(join(folder, 'CMakeLists.txt'), 'utf8');
  const jsonPath = join(folder, 'plugin.json');
  if (!existsSync(jsonPath)) throw new Error(`plugins/${name}/plugin.json is missing`);
  const written = JSON.parse(readFileSync(jsonPath, 'utf8'));
  for (const key of ['tagline', 'category', 'description', 'features']) {
    if (!written[key]) throw new Error(`plugins/${name}/plugin.json has no "${key}"`);
  }
  if (!CATEGORIES.includes(written.category)) {
    throw new Error(`plugins/${name}/plugin.json: category must be one of ${CATEGORIES.join(', ')}`);
  }

  const id = name.toLowerCase();
  const formats = (cmake.match(/FORMATS\s+([^\n)]*)/)?.[1] ?? '').split(/\s+/).filter(Boolean);
  return {
    id,
    folder: name,
    name: cmakeValue(cmake, 'PRODUCT_NAME') ?? name,
    type: typeOf(cmake),
    formats: ['VST3', 'AU', 'Standalone'].filter((f) => formats.includes(f)),
    parameters: parametersOf(folder),
    ...written,
    ...(existsSync(join(root, 'website', 'public', 'demos', `${id}.mp3`)) ? { demo: `/demos/${id}.mp3` } : {}),
  };
}

const names = readdirSync(pluginsDir)
  .filter((n) => /^[A-Za-z0-9_-]+$/.test(n) && statSync(join(pluginsDir, n)).isDirectory()
    && existsSync(join(pluginsDir, n, 'CMakeLists.txt')))
  .sort((a, b) => a.localeCompare(b));
const catalog = names.map(entryFor);
const json = JSON.stringify(catalog, null, 2) + '\n';

if (process.argv.includes('--check')) {
  const current = existsSync(outFile) ? readFileSync(outFile, 'utf8') : '';
  if (current !== json) {
    console.error('website/src/lib/catalog.json is out of date: run node website/scripts/build-catalog.mjs');
    process.exit(1);
  }
  console.log(`catalog.json is up to date (${catalog.length} plugins)`);
} else {
  writeFileSync(outFile, json);
  const noParams = catalog.filter((p) => p.parameters.length === 0).map((p) => p.folder);
  console.log(`Wrote ${catalog.length} plugins to website/src/lib/catalog.json`
    + (noParams.length ? `; no parameters found for: ${noParams.join(', ')}` : ''));
}
