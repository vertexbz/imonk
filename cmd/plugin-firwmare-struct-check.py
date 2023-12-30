#!/usr/bin/env python3
import sys, os
sys.path.insert(1, os.path.join(os.path.dirname(__file__), '..', 'klipper'))

import json
import os.path
import re
import sys
from ast import literal_eval
from dataclasses import Field as DCField, MISSING
from difflib import ndiff
from imonk.resource.view.config.widget import WIDGETS
from imonk.resource.view.config.field.base import BaseField
from imonk.resource.view.config.types import Color, Font, Align


class Field:
    def __init__(self, f: DCField):
        self.type = f.type

        default = f.default_factory() if f.default_factory not in (MISSING, BaseField.DEFAULT) else f.default
        if default in (MISSING, BaseField.DEFAULT):
            self.default = None
            self.required = True
        else:
            self.default = default
            self.required = False

    def meta(self):
        meta = {
            'required': self.required,
            'type': self.type if isinstance(self.type, str) else self.type.__name__
        }

        if not self.required:
            meta['default'] = self.default

        return meta

    def __repr__(self):
        return str(self.meta())


class CodeField:
    def __init__(self, typ: type, default):
        self.type = typ
        self.default = default

    def meta(self):
        meta = {
            'required': self.default is None,
            'type': self.type.__name__
        }

        if self.default is not None:
            meta['default'] = self.default

        return meta

    def __repr__(self):
        return str(self.meta())


def get_ext(cls, k):
    default = cls.__dict__.get(k, None)
    if isinstance(default, BaseField):
        return default
    return None


def get_py_fields(cls):
    return {k: Field(get_ext(cls, k) or f) for k, f in cls.__dataclass_fields__.items()}


def get_fields(cls):
    field_map = cls.__data_field_map_out__()
    return {field_map.get(k, k): f for k, f in get_py_fields(cls).items()}


def factory_lines_per_type():
    per_type = {}
    with open(os.path.join(os.path.dirname(__file__), '..', 'src', 'Widget', 'factory.hpp')) as f:
        lines = f.readlines()
        s = False

        typ = None

        for l in lines:
            if l.strip() == "":
                continue
            l = l.rstrip()
            if 'build_widget(' in l:
                s = True
                continue
            if 'return nullptr' in l:
                break

            if 'obj["type"]' in l and 'strcmp' in l:
                typ = l.strip().split(',')[1].split('"')[1]
                per_type[typ] = []
                continue

            if typ:
                per_type[typ].append(l.replace('Display::Util::', ''))

            if 'return' in l:
                typ = None
                continue

    clean = {}
    for k, v, in per_type.items():
        v = ''.join(map(lambda l: l.strip(), v))
        v = v[v.find('('):].split(';')
        first = v[0][0:-1].strip()
        rest = v[1:-1]
        v = [first, *rest]
        clean[k] = '\n'.join(v)

    return clean


PROP_CATCHER = re.compile(r'(?:strto(rgb|font|align)\s*\()?obj\["([^"]+)"](?:\.as<([^>]+)>)?(?:\s*\|\s*("?[\w\.]+"?))?')


def get_props_from_code(code: str):
    result = {}
    for typ, name, typ_as, default in PROP_CATCHER.findall(code):
        if not default:
            default = None
        else:
            default = literal_eval(default)

        if not typ:
            if typ_as in ('const char *', 'char *', 'const char*', 'char*'):
                typ = str
            elif typ_as in ('uint8_t', 'int8_t', 'uint16_t', 'int16_t', 'uint32_t', 'int32_t', 'Display::Unit'):
                typ = int
            elif default is not None:
                typ = type(default)
            else:
                raise TypeError(f'Unknown type {typ_as}')
        else:
            if typ == 'font':
                typ = Font
            elif typ == 'rgb':
                typ = Color
            elif typ == 'align':
                typ = Align
            else:
                raise TypeError(f'Unknown type {typ}')

        result[name] = CodeField(typ, default)
    return result


def get_code_fields() -> dict[str, dict[str, CodeField]]:
    return {typ: get_props_from_code(code) for typ, code in factory_lines_per_type().items()}


errors = 0
def eprint(*args, sep=' ', end='\n', count=True):
    if count:
        global errors
        errors = errors + 1
    file = sys.stdout
    file.write("\033[31m")
    file.write(sep.join(args))
    file.write('\033[0m')
    file.write(end)


warnings = 0
def wprint(*args, sep=' ', end='\n', count=True):
    if count:
        global warnings
        warnings = warnings + 1
    file = sys.stdout
    file.write("\033[33m")
    file.write(sep.join(args))
    file.write('\033[0m')
    file.write(end)


if __name__ == "__main__":
    firmware_fields = get_code_fields()

    for Widget in WIDGETS:
        fields = get_fields(Widget)
        firmware = firmware_fields[Widget.KIND]

        for key, field in fields.items():
            if key != 'id' and key not in firmware:
                eprint(f'`{Widget.KIND}` field `{key}` not defined in Firmware! {field}')

        for key, field in firmware.items():
            if key not in fields:
                eprint(f'`{Widget.KIND}` field `{key}` not defined in plugin! {field}')

        for key, field in firmware.items():
            if key not in fields:
                continue

            kl_meta = fields[key].meta()
            kl_str = json.dumps(kl_meta).strip()
            fw_meta = field.meta()
            fw_str = json.dumps(fw_meta).strip()

            if kl_str != fw_str:
                if fw_meta['required'] and not kl_meta['required'] and 'default' in kl_meta and 'default' not in fw_meta:
                    del fw_meta['required'], kl_meta['required'], kl_meta['default']
                    if fw_meta == kl_meta:
                        wprint(f'`{Widget.KIND}` field `{key}` default value is ensured only by plugin!')
                        continue

                eprint(f'`{Widget.KIND}` field `{key}` differs! Plugin -, Firmware +')
                eprint('    '+'\n    '.join(ndiff(kl_str.splitlines(keepends=True), fw_str.splitlines(keepends=True))), count=False)

    if errors:
        print()
        eprint(f'Found {errors} errors and {warnings} warnings', count=False)
    elif warnings:
        print()
        wprint(f'Found {warnings} warnings', count=False)

    sys.exit(errors)


