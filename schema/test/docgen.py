from dataclasses import Field as DCField, MISSING
from config import View
from config.widget import WIDGETS
from config.field.base import BaseField


class Field:
    def __init__(self, f: DCField, cls):
        self.type = f.type

        default = f.default_factory() if f.default_factory not in (MISSING, BaseField.DEFAULT) else f.default
        if default in (MISSING, BaseField.DEFAULT):
            self.default = None
            self.required = True
        else:
            self.default = default
            self.required = False

        if f.metadata and 'example' in f.metadata:
            self.example = f.metadata['example']
        elif self.default is not None:
            self.example = self.default
        else:
            raise ValueError(f'{cls.__name__}.{f.name} missing example')

        if f.metadata and 'description' in f.metadata:
            self.description = f.metadata['description']
        else:
            raise ValueError(f'{cls.KIND}.{f.name} missing description')

    def meta(self):
        meta = {
            'required': self.required,
            'type': self.type if isinstance(self.type, str) else self.type.__name__
        }

        if not self.required:
            meta['default'] = self.default

        return meta

    def __repr__(self):
        return str({
            **self.meta(),
            'example': self.example,
            'description': self.description
        })


def get_ext(cls, k):
    default = cls.__dict__.get(k, None)
    if isinstance(default, BaseField):
        setattr(default, 'name', k)
        return default
    return None


def get_py_fields(cls):
    return {k: Field(get_ext(cls, k) or f, cls) for k, f in cls.__dataclass_fields__.items()}


def get_fields(cls):
    field_map = cls.__data_field_map_out__()
    return {field_map.get(k, k): f for k, f in get_py_fields(cls).items()}


def lookahead(iterable):
    """Pass through all values from the given iterable, augmented by the
    information if there are more values to come after the current one
    (True), or if it is the last value (False).
    """
    # Get an iterator and pull the first value.
    it = iter(iterable)
    last = next(it)
    # Run the iterator to exhaustion (starting from the second value).
    for val in it:
        # Report the *previous* value (more to come).
        yield last, True
        last = val
    # Report the last value.
    yield last, False


if __name__ == "__main__":
    print('## View')
    print('```ini')
    print('[imonk view ...]')
    for k, f in get_fields(View).items():
        example = str(f.example).split('\n')
        print(f'{k}: {example.pop(0)}  # {f.description}')
        for l in example:
            print(l)
    print('```')

    print()
    print()
    print('## Widgets')
    for Widget in WIDGETS:
        print()
        print(f'### `{Widget.KIND}`')
        print('```json')
        print('    {')
        print(f'        "type": "{Widget.KIND}",')
        for (k, f), has_more in lookahead(get_fields(Widget).items()):
            example = f.example
            if isinstance(example, str):
                example = '"' + example + '"'

            default = f.default
            if isinstance(default, str):
                default = '"' + default + '"'

            print(f'        "{k}": {example}{"," if has_more else ""}  # ', end='')
            if not f.required:
                print('(optional) ', end='')
            print(f.description, end='')
            if not f.required:
                print(f' (default: {default}) ', end='')
            print()

        print('    }')
        print('```')

