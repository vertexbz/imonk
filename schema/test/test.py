import json
from difflib import ndiff
from config import View

if __name__ == "__main__":
    for n in range(5):
        with open(f'test{n+1}.json', 'r', encoding='ascii') as f:
            data = json.load(f)
            data_str = json.dumps(data)
            scene = View(**data)

            print(scene)
            print('\n'.join(ndiff([data_str], [json.dumps(scene.dump())])))
