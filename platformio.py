import time
import requests
Import('env')

printer_host: str = env.GetProjectOption('printer_host')
otm_binary = '$BUILD_DIR/${PROGNAME}.bin'

if 'upload' not in env.DumpTargets():
    env.AddCustomTarget(
        name='upload',
        title='Upload',
        dependencies=None,
        actions=[],
    )


def otm_callback(*args, **kwargs):
    firmware_path = env.subst(otm_binary)
    update_url = printer_host.rstrip('/') + '/machine/imonk/firmware'

    print('Initiating update over the Moonraker')
    print(f'Update URL: {update_url}')
    print(f'Firmware: {firmware_path}')

    print(f'Uploading firmware...')
    try:
        start = time.time()
        response = requests.request('POST', update_url, files={'file': open(firmware_path, 'rb')})
        end = time.time()
        print(response.text)
        print(f'Upload took: {end - start}')
    except Exception as e:
        print(e)


env.AddCustomTarget(
    name='otm',
    dependencies=[otm_binary],
    actions=[otm_callback],
    title='Over the Moon(raker) update',
    description="Show PlatformIO Core and Python versions"
)