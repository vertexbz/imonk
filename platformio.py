Import('env')
if 'upload' not in env.DumpTargets():
    env.AddCustomTarget(
        name='upload',
        title='Upload',
        dependencies=None,
        actions=[],
    )
