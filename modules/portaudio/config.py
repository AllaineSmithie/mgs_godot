def can_build(env, platform):
    if platform == "web" or platform == "android":
        return False
    # temporarily disable linux build as buildpipeline
    # seems to break
    if platform == "linuxbsd":
        return False
    return True


def configure(env):
    pass


def get_doc_classes():
    return [
        "PortAudio",
        "PortAudioNode",
    ]


def get_doc_path():
    return "doc_classes"
