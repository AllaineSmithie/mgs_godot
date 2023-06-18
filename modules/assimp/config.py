def can_build(env, platform):
    if not env.editor_build:
        return False
    return not env["disable_3d"]


def configure(env):
    pass
