import os
import platform
import sys
import os.path


def is_active():
    return True


def get_name():
    return "Vita"


def can_build():
    # Check the minimal dependencies
    vitasdk = os.environ.get("VITASDK", "/usr/local/vitasdk")
    if not os.path.exists(vitasdk):
        print("VITASDK not defined in environment.. vita disabled.")
        return False
    return True


def get_opts():

    from SCons.Variables import BoolVariable, EnumVariable

    return [
        BoolVariable("use_sanitizer", "Use LLVM compiler address sanitizer", False),
        BoolVariable("use_leak_sanitizer", "Use LLVM compiler memory leaks sanitizer (implies use_sanitizer)", False),
        EnumVariable("debug_symbols", "Add debugging symbols to release builds", "no", ("yes", "no", "full")),
        BoolVariable("separate_debug_symbols", "Create a separate file containing debugging symbols", False),
        BoolVariable("touch", "Enable touch events", True),
    ]


def get_flags():
    return [
        ("tools", False),
        ("builtin_bullet", True),
        ("builtin_enet", True),  # Not in VitaSDK.
        ("builtin_libogg", False),
        ("builtin_libvpx", False),
        ("builtin_libwebsockets", True),  # Not in VitaSDK.
        ("builtin_mbedtls", False),  # VitaSDK package isn't the correct version btw
        ("builtin_miniupnpc", False),
        ("builtin_opus", False),
        ("builtin_pcre2", False),
        ("builtin_pcre2_with_jit", False),
        ("builtin_squish", True),  # Not in VitaSDK.
        ("builtin_zstd", False),
        ("module_mbedtls_enabled", False),
        ("module_upnp_enabled", False),
        ("module_enet_enabled", False),
        ("module_gdnative_enabled", True),
        ("module_regex_enabled", True),
        ("module_webm_enabled", False),
        ("module_mobile_vr_enabled", False),
    ]


def configure(env):
    vita_sdk_path = os.environ.get("VITASDK", "/usr/local/vitasdk")

    env["CC"] = vita_sdk_path + "/bin/arm-vita-eabi-gcc"
    env["CXX"] = vita_sdk_path + "/bin/arm-vita-eabi-g++"
    env["LD"] = vita_sdk_path + "/bin/arm-vita-eabi-ld"
    env["AR"] = vita_sdk_path + "/bin/arm-vita-eabi-ar"
    env["STRIP"] = vita_sdk_path + "/bin/arm-vita-eabi-strip"
    env["RANLIB"] = vita_sdk_path + "/bin/arm-vita-eabi-ranlib"
    ## Build type

    pkg_config_path = "{}/arm-vita-eabi/lib/pkgconfig/pkgconfig"

    os.environ["PKG_CONFIG_PATH"] = pkg_config_path
    env["ENV"]["PKG_CONFIG_PATH"] = pkg_config_path

    env["VITASDK_BIN"] = "{}/bin/".format(vita_sdk_path)

    env.Prepend(CPPPATH=["{}/arm-vita-eabi/include".format(vita_sdk_path)])
    env.Prepend(CPPPATH=["{}/arm-vita-eabi/include/freetype2".format(vita_sdk_path)])
    env.Prepend(CPPPATH=["{}/share/gcc-arm-vita-eabi/samples/common".format(vita_sdk_path)])
    env.Append(LIBPATH=["{}/arm-vita-eabi/lib".format(vita_sdk_path)])
    env.Append(LINKFLAGS=["-Wl,-q,-whole-archive", "-lpthread", "-Wl,-q,-no-whole-archive"])
    print(env.get("CCFLAGS"))

    env.Prepend(
        CCFLAGS=[
            "-Wl,-q",
            "-D_POSIX_TIMERS",
            "-DPTHREAD_ENABLED",
            "-DVITA_ENABLED",
            "-DPOSH_COMPILER_GCC",
            "-DPOSH_OS_VITA",
            '-DPOSH_OS_STRING=\\"vita\\"',
            "-D__psp2__",
#            "-DPVR_PSP2",
        ]
    )

    if env["target"] == "release":
        # -O3 -ffast-math is identical to -Ofast. We need to split it out so we can selectively disable
        # -ffast-math in code for which it generates wrong results.
        if env["optimize"] == "speed":  # optimize for speed (default)
            env.Prepend(CCFLAGS=["-O2", "-ffast-math"])
        else:  # optimize for size
            env.Prepend(CCFLAGS=["-Os"])

        if env["debug_symbols"] == "yes":
            env.Prepend(CCFLAGS=["-g1"])
        if env["debug_symbols"] == "full":
            env.Prepend(CCFLAGS=["-g2"])

    elif env["target"] == "release_debug":
        if env["optimize"] == "speed":  # optimize for speed (default)
            env.Prepend(CCFLAGS=["-O2", "-ffast-math", "-DDEBUG_ENABLED"])
        else:  # optimize for size
            env.Prepend(CCFLAGS=["-Os", "-DDEBUG_ENABLED"])

        if env["debug_symbols"] == "yes":
            env.Prepend(CCFLAGS=["-g1"])
        if env["debug_symbols"] == "full":
            env.Prepend(CCFLAGS=["-g2"])

    elif env["target"] == "debug":
        env.Prepend(CCFLAGS=["-g3", "-DDEBUG_ENABLED", "-DDEBUG_MEMORY_ENABLED"])
        # env.Append(LINKFLAGS=['-rdynamic'])

        # env.Append(LINKFLAGS=['-rdynamic'])

    ## Architecture

    env["bits"] = "32"

    ## Flags

    # Linkflags below this line should typically stay the last ones
    # if not env['builtin_zlib']:
    #    env.ParseConfig('aarch64-none-elf-pkg-config zlib --cflags --libs')

    env.Append(CPPPATH=["#platform/vita"])
    env.Append(CPPFLAGS=["-DLIBC_FILEIO_ENABLED", "-DGLES_ENABLED", "-DGL_GLEXT_PROTOTYPES"])
    env.Append(CPPFLAGS=["-DPTHREAD_NO_RENAME"])
    env.Append(CCFLAGS=["-mtune=cortex-a9", "-mfpu=neon", "-fpermissive", "-ftree-vectorize", "-Wno-attributes"])
    env.Append(
        LIBS=[
            "dl",
            "vitaGL",
            "vitashark",
            "SceShaccCgExt",
            "SceShaccCg_stub",
            "SceGxm_stub",
            "mathneon",
            "SceKernelDmacMgr_stub",
            "SceRazorCapture_stub",
            "SceRazorHud_stub",
            "SceCommonDialog_stub",
            "taihen_stub",
            "SceLibKernel_stub",
            "SceKernelThreadMgr_stub",
            "SceSblSsMgr_stub",
            "SceAppMgr_stub",
            "SceIofilemgr_stub",
            "SceSysmodule_stub",
            "SceDisplay_stub",
            "SceFios2_stub",
            "SceCtrl_stub",
            "SceMotion_stub",
            "SceTouch_stub",
            "SceIme_stub",
            "SceAudio_stub",
            "ScePower_stub",
            "jpeg",
            "png",
            "freetype",
            "opus",
            "vorbis",
            "ogg",
            "z",
            "zstd",
            "pcre2-32",
            "theora",
            #"-llibgpu_es4_ext_stub.a",
            #"-llibIMGEGL_stub.a",
            #"-llibGLESv2_stub.a",
        ]
    )
    print(env.get("LIBS"))
