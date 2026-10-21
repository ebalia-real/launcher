#!/usr/bin/env python3
"""Draws the sidebar icons: original 16x16 pixel art in a block-game style, made for EBALIA.

Run it from ebalia-launcher/ to rewrite resources/icons/navigation/*.png. Only the standard library is needed.
The pickaxe is not drawn here: it is Minetest Game's (see resources/icons/navigation/LICENSE-minetest.txt).
"""
import os
import struct
import sys
import zlib

SIZE = 16


def rgb(value, alpha=255):
    value = value.lstrip('#')
    return (int(value[0:2], 16), int(value[2:4], 16), int(value[4:6], 16), alpha)


def shade(colour, factor):
    return tuple(max(0, min(255, round(c * factor))) for c in colour[:3]) + (colour[3],)


def blank():
    return [[None] * SIZE for _ in range(SIZE)]


def noise(x, y, salt=0):
    """A fixed pattern, so the textures come out the same every time."""
    n = (x * 374761393 + y * 668265263 + salt * 2147483647) & 0xffffffff
    n = ((n ^ (n >> 13)) * 1274126177) & 0xffffffff
    return (n ^ (n >> 16)) % 100


def grid(rows, palette, top=0, left=0):
    """Pixels from rows of characters; '.' and spaces stay empty."""
    image = blank()
    for y, row in enumerate(rows):
        for x, key in enumerate(row):
            if key not in '. ':
                image[top + y][left + x] = palette[key]
    return image


def outline(image, colour):
    """A one-pixel border around the shape, touching it side by side (not at the corners)."""
    out = [row[:] for row in image]
    for y in range(SIZE):
        for x in range(SIZE):
            if image[y][x] is None and any(0 <= y + dy < SIZE and 0 <= x + dx < SIZE and image[y + dy][x + dx] is not None
                                           for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                out[y][x] = colour
    return out


def layer(*images):
    """Later images are drawn over earlier ones."""
    out = blank()
    for image in images:
        for y in range(SIZE):
            for x in range(SIZE):
                if image[y][x] is not None:
                    out[y][x] = image[y][x]
    return out


def cube(top, left, right):
    """A block seen from above, like in an inventory: top, left and right faces, 16 pixels wide."""
    image = blank()
    for x in range(SIZE):
        k = (x if x < 8 else 15 - x) // 2
        for y in range(4 - k, 5 + k):
            image[y][x] = top(x, y)
        for depth, y in enumerate(range(5 + k, 13 + k)):
            image[y][x] = (left if x < 8 else right)(x, depth)
    return image


def circle(rim_light, rim_dark, face):
    """A round item 12 pixels across with a rim lit from the top left."""
    widths = [4, 8, 10, 10, 12, 12, 12, 12, 10, 10, 8, 4]
    image = blank()
    for row, width in enumerate(widths):
        y = 2 + row
        for x in range(8 - width // 2, 8 + width // 2):
            image[y][x] = face(x, y)
    for y in range(SIZE):
        for x in range(SIZE):
            if image[y][x] is not None and any(not (0 <= y + dy < SIZE and 0 <= x + dx < SIZE) or image[y + dy][x + dx] is None
                                               for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                image[y][x] = rim_light if (x - 7.5) + (y - 7.5) < 0 else rim_dark
    return image


# Home: a grass block.
def grass_block():
    greens = [rgb('5a9a33'), rgb('6db33f'), rgb('6db33f'), rgb('86c94f')]
    dirt = [rgb('6f4a2f'), rgb('8f6240'), rgb('8f6240'), rgb('a8774f')]
    side_green = [rgb('4f8a2d'), rgb('5f9e37')]

    def top(x, y):
        return greens[noise(x, y) % 4]

    def side(x, depth, factor):
        fringe = 2 + (1 if noise(x, 7) < 45 else 0)
        colour = side_green[noise(x, depth, 1) % 2] if depth < fringe else dirt[noise(x, depth, 2) % 4]
        return shade(colour, factor)
    return cube(top, lambda x, d: side(x, d, 0.92), lambda x, d: side(x, d, 0.7))


# Discover mods: a compass.
def compass():
    face = rgb('5f5f68')
    image = circle(rgb('d6d6dc'), rgb('8c8c96'), lambda x, y: face if noise(x, y, 3) > 15 else shade(face, 1.12))
    for x, y in ((8, 7), (9, 6), (10, 5), (9, 5), (8, 6)):
        image[y][x] = rgb('e5392f')
    for x, y in ((7, 8), (6, 9), (5, 10), (6, 10), (7, 9)):
        image[y][x] = rgb('e9e9ee')
    image[7][7] = image[8][8] = rgb('2c2c32')
    return outline(image, rgb('1f1f24'))


# Lost versions: a ghost.
def ghost():
    rows = [
        '....WWWW....',
        '..WWWWWWWW..',
        '.WWWWWWWWWS.',
        '.WWWWWWWWWS.',
        'WWWEEWWEEWWS',
        'WWWEEWWEEWWS',
        'WWWWWWWWWWWS',
        'WWWWWWWWWWWS',
        'WWWWWWWWWWWS',
        'WWWWWWWWWWWS',
        'WWWSWWWSWWSS',
        'WW.WWW.WWW.S',
    ]
    palette = {'W': rgb('ecebf5'), 'S': rgb('b8b6cc'), 'E': rgb('2a2840')}
    return outline(grid(rows, palette, top=2, left=2), rgb('24222f'))


# My packs: a chest.
def chest():
    rows = [
        'LLLLLLLLLLLL',
        'LMMMMMMMMMMD',
        'LMMMMMMMMMMD',
        'BBBBBggBBBBB',
        'LMMMMGGMMMMD',
        'LMMMMBBMMMMD',
        'LMMMMMMMMMMD',
        'DDDDDDDDDDDD',
        'LMMMMMMMMMMD',
        'LMMMMMMMMMMD',
        'DDDDDDDDDDDD',
    ]
    palette = {'L': rgb('c48a43'), 'M': rgb('9c6530'), 'D': rgb('74481f'), 'B': rgb('3e2611'),
               'g': rgb('dcdcdc'), 'G': rgb('9a9a9a')}
    return outline(grid(rows, palette, top=3, left=2), rgb('2b1a0b'))


# Skins: a block-game player.
def player():
    rows = [
        '..hhhhhh..',
        '..hhhhhh..',
        '..hsssss..',
        '..sessesS.',
        '..sssssS..',
        '..ssmssS..',
        'cccccccccc',
        'ccccccccCC',
        'sscccccCSS',
        'sscccccCSS',
        'ssbbbbbbSS',
        '..pppPPP..',
        '..pppPPP..',
        '..ggg.GG..',
    ]
    palette = {'h': rgb('2b2b33'), 's': rgb('d9a57c'), 'S': rgb('b8875f'), 'e': rgb('2a2a2a'), 'm': rgb('9c5c45'),
               'c': rgb('5fb83d'), 'C': rgb('46902c'), 'b': rgb('6b4423'), 'p': rgb('6b6f7c'), 'P': rgb('51545f'),
               'g': rgb('3d3d44'), 'G': rgb('2c2c32')}
    return outline(grid(rows, palette, top=1, left=3), rgb('1d1a22'))


# News: a sheet of paper.
def paper():
    rows = [
        'pppppppf..',
        'pppppppff.',
        'plllllpfff',
        'pppppppppP',
        'plllllllpP',
        'pppppppppP',
        'pllllllppP',
        'pppppppppP',
        'plllllllpP',
        'pppppppppP',
        'pllllppppP',
        'pppppppppP',
        'PPPPPPPPPP',
    ]
    palette = {'p': rgb('efe6cf'), 'P': rgb('cdbf98'), 'l': rgb('8c7a57'), 'f': rgb('c4b48a')}
    return outline(grid(rows, palette, top=2, left=3), rgb('4a3b22'))


# Community: two players side by side.
def players():
    back = grid([
        '.yyyyyy.',
        '.yyyyyy.',
        '.yttttt.',
        '.tetteT.',
        '.ttmmtT.',
        '.ttttTT.',
        'rrrrrrrr',
        'rrrrrrRR',
        'rrrrrrRR',
    ], {'y': rgb('e0b54a'), 't': rgb('8d5a3b'), 'T': rgb('74472d'), 'e': rgb('1e1e1e'), 'm': rgb('5a3220'),
        'r': rgb('d9573f'), 'R': rgb('b04430')}, top=1, left=7)
    front = grid([
        '.hhhhhh.',
        '.hhhhhh.',
        '.hsssss.',
        '.sessesS',
        '.ssmmsS.',
        '.ssssSS.',
        'cccccccc',
        'ccccccCC',
        'ccccccCC',
    ], {'h': rgb('2b2b33'), 's': rgb('d9a57c'), 'S': rgb('b8875f'), 'e': rgb('2a2a2a'), 'm': rgb('9c5c45'),
        'c': rgb('5fb83d'), 'C': rgb('46902c')}, top=6, left=1)
    return layer(outline(back, rgb('1d1a22')), outline(front, rgb('1d1a22')))


# Guide: a book.
def book():
    rows = [
        'bLLLLLLLLp',
        'bBBBBBBBBp',
        'bByyyyyBBp',
        'bBBBBBBBBp',
        'bBByyyBBBp',
        'bBBBBBBBBp',
        'bBBBBBBBBp',
        'bBBBBBBBBp',
        'bBBBBBBBBp',
        'bBBBBBBBBp',
        'bBBBBBBBBp',
        'bbbbbbbbbP',
    ]
    palette = {'b': rgb('5e2f15'), 'B': rgb('8a4b26'), 'L': rgb('a8623a'), 'y': rgb('e8c34a'),
               'p': rgb('f2ead2'), 'P': rgb('cfc3a0')}
    image = grid(rows, palette, top=2, left=3)
    for y in (12, 13):
        image[y][9] = rgb('d23b32')  # the bookmark
    return outline(image, rgb('2a160a'))


# Bedrock: a bedrock block.
def bedrock_block():
    greys = [rgb('1f1f23'), rgb('3a3a40'), rgb('55555c'), rgb('55555c'), rgb('75757d'), rgb('9b9ba3')]

    def face(x, y, factor):
        return shade(greys[noise(x, y, 5) % 6], factor)
    return cube(lambda x, y: face(x, y, 1.2), lambda x, d: face(x, d + 20, 0.82), lambda x, d: face(x, d + 40, 0.56))


# Changelog: a clock.
def clock():
    def face(x, y):
        if y < 8:
            return rgb('ffd94a') if (x, y) in ((5, 5), (6, 5), (5, 6), (6, 6)) else rgb('74c4f2')
        return rgb('f2f2f2') if (x, y) in ((9, 10), (6, 11), (10, 9)) else rgb('23305f')
    image = circle(rgb('f6dc5a'), rgb('c49a22'), face)
    return outline(image, rgb('3a2a10'))


# Settings: an anvil.
def anvil():
    rows = [
        '..LLLLLLLLLLL',
        'AAAAAAAAAAAAD',
        '..AAAAAAAAAAD',
        '....DDDDDDD..',
        '.....AAAAD...',
        '.....AAAAD...',
        '....AAAAAAD..',
        '...AAAAAAAAD.',
        '..AAAAAAAAAAD',
        '..DDDDDDDDDDD',
    ]
    palette = {'L': rgb('a6a6ae'), 'A': rgb('6e6e76'), 'D': rgb('48484f')}
    return outline(grid(rows, palette, top=3, left=1), rgb('1b1b1f'))


ICONS = {
    'grass-block': grass_block, 'compass': compass, 'ghost': ghost, 'chest': chest,
    'player': player, 'paper': paper, 'players': players, 'book': book, 'bedrock': bedrock_block,
    'clock': clock, 'anvil': anvil,
}


def png(image, scale=1, background=None):
    width = height = SIZE * scale
    raw = bytearray()
    for y in range(height):
        raw.append(0)
        for x in range(width):
            pixel = image[y // scale][x // scale] or background or (0, 0, 0, 0)
            raw.extend(pixel)
    chunk = lambda kind, data: struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data) & 0xffffffff)
    return (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0))
            + chunk(b'IDAT', zlib.compress(bytes(raw), 9)) + chunk(b'IEND', b''))


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else 'resources/icons/navigation'
    os.makedirs(out, exist_ok=True)
    for name, draw in ICONS.items():
        with open(os.path.join(out, name + '.png'), 'wb') as f:
            f.write(png(draw()))


if __name__ == '__main__':
    main()
