import json
from difflib import ndiff
from config import Scene

if __name__ == "__main__":
    for n in range(3):
        with open(f'test{n+1}.json', 'r', encoding='ascii') as f:
            data = json.load(f)
            data_str = json.dumps(data)
            scene = Scene(**data)

            print(scene)
            print('\n'.join(ndiff([data_str], [json.dumps(scene.dump())])))
