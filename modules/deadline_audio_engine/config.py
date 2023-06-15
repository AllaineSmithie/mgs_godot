def can_build(env, platform):
    return True

def get_doc_path():
    return "doc_classes"

def get_doc_classes():
    return [
        "DeadlineAudioEngine"
        "AudioStreamPRO"
    ]


def configure(env):
    # sfizz
    # SConscript("sfizz/SCsub")
    # if env.dev_build:
    #    env.Append(LIBPATH=["sfizz/library/lib/Debug"])
    #else:
    #    env.Append(LIBPATH=["sfizz/library/lib/Release"])
    #
    # env.Append(LIBS=["absl_bad_optional_access", "absl_bad_variant_access", "absl_base", "absl_city", "absl_civil_time", "absl_cord", "absl_cord_internal"])
    # env.Append(LIBS=["absl_cordz_functions", "absl_cordz_handle", "absl_cordz_info", "absl_crc_cord_state", "absl_crc_cpu_detect", "absl_crc_internal"])
    # env.Append(LIBS=["absl_crc32c", "absl_debugging_internal", "absl_demangle_internal", "absl_exponential_biased", "absl_graphcycles_internal", "absl_hash"])
    # env.Append(LIBS=["absl_hashtablez_sampler", "absl_int128", "absl_log_severity", "absl_low_level_hash", "absl_malloc_internal", "absl_raw_hash_set"])
    # env.Append(LIBS=["absl_raw_logging_internal", "absl_spinlock_wait", "absl_stacktrace", "absl_strings", "absl_strings_internal", "absl_symbolize", "absl_synchronization"])
    # env.Append(LIBS=["absl_throw_delegate", "absl_time", "absl_time_zone", "aiff", "libwavpack", "sfizz", "sfizz_cephes", "sfizz_cpuid"])
    # env.Append(LIBS=["sfizz_filesystem_impl", "sfizz_fmidi", "sfizz_hiir_polyphase_iir2designer", "sfizz_import", "sfizz_internal", "sfizz_kissfft", "sfizz_messaging"])
    # env.Append(LIBS=["sfizz_parser", "sfizz_pugixml", "sfizz_spin_mutex", "sfizz_spline", "sfizz_static", "sfizz_tunings", "st_audiofile", "st_audiofile_formats"])
    
    # elif env["platform"] == "x11":
    #     env.Append(LIBS=["fmod", "fmodstudio"])
    #     if env["bits"] == "32":
    #         env.Append(
    #             LIBPATH=["#modules/fmod/api/core/lib/x86/",
    #                      "#modules/fmod/api/studio/lib/x86/"])
    #     else:
    #         env.Append(
    #             LIBPATH=["#modules/fmod/api/core/lib/x86_64/",
    #                      "#modules/fmod/api/studio/lib/x86_64/"])
    # 
    # elif env["platform"] == "osx":
    #     env.Append(LIBS=["fmod", "fmodstudio"])
    #     env.Append(
    #         LIBPATH=["#modules/fmod/api/core/lib/", "#modules/fmod/api/studio/lib/"])
    
    # elif env["platform"] == "android":
    #     if env["android_arch"] == "arm64v8":
    #         env.Append(LIBPATH=["#modules/fmod/api/core/lib/arm64-v8a",
    #                             "#modules/fmod/api/studio/lib/arm64-v8a"])
    #     else:
    #         env.Append(LIBPATH=["#modules/fmod/api/core/lib/armeabi-v7a",
    #                             "#modules/fmod/api/studio/lib/armeabi-v7a"])
    #     env.Append(LIBS=["fmod", "fmodstudio"])
    # 
    # elif env["platform"] == "iphone":
    #     env.Append(LIBPATH=["#modules/fmod/api/core/lib/",
    #                         "#modules/fmod/api/studio/lib/"])
    #     env.Append(LIBS=["libfmod_iphoneos.a", "libfmodstudio_iphoneos.a"])
    pass
