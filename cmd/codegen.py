#!/usr/bin/env python3
import sys, os
# sys.path.insert(1, os.path.join(os.path.dirname(__file__), '..', 'klipper'))
GENERATED = os.path.join(os.path.dirname(__file__), '..', 'klipper', 'imonk', 'generated')

result = []
with open(os.path.join(os.path.dirname(__file__), '..', 'src', 'Display', 'Util.cpp'), 'r') as f:
    for line in f.readlines():
        line = line.strip()
        if line.startswith('fnt_if'):
            result.append(line.split('"')[1])

with open(os.path.join(GENERATED, 'font.py'), 'w') as f:
    f.write(f'KNOWN_FONTS = {repr(tuple(result))}\n')
