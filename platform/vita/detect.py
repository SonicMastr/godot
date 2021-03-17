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
    if "VITASDK" not in os.environ:
        print("VITASDK not defined in environment.. vita diabled.")
        return False
    return True

def get_opts():
    
    from SCons.Variables import BoolVariable, EnumVariable

    return [
        BoolVariable('use_sanitizer', 'Use LLVM compiler address sanitizer', False),
        BoolVariable('use_leak_sanitizer', 'Use LLVM compiler memory leaks sanitizer (implies use_sanitizer)', False),
        EnumVariable('debug_symbols', 'Add debugging symbols to release builds', 'yes', ('yes', 'no', 'full')),
        BoolVariable('separate_debug_symbols', 'Create a separate file containing debugging symbols', False),
        BoolVariable('touch', 'Enable touch events', True),
        ]

def get_flags():
    return [
        ("tools", False),
        ('builtin_bullet', True),
        ('builtin_enet', True), # Not in portlibs.
        ('builtin_freetype', True),
        ('builtin_libogg', True),
        ('builtin_libpng', True),
        ('builtin_libtheora', True),
        ('builtin_libvorbis', True),
        ('builtin_libvpx', True),
        ('builtin_libwebp', True),
        ('builtin_libwebsockets', True), # Not in portlibs.
        ('builtin_mbedtls', False),
        ('builtin_miniupnpc', False),
        ('builtin_opus', True),
        ('builtin_pcre2', False),
        ('builtin_squish', True), # Not in portlibs.
        ('builtin_zlib', True),
        ('builtin_zstd', True), # Not in portlibs.
        ('module_websocket_enabled', False),
        ('module_mbedtls_enabled', False),
        ('module_upnp_enabled', False),
        ('module_enet_enabled', False),
        ('module_gdnative_enabled', False),
        ('module_regex_enabled', False),
        ('module_webm_enabled', False)
        ]


def configure(env):
    env["CC"] = "arm-vita-eabi-gcc"
    env["CXX"] = "arm-vita-eabi-g++"
    env["LD"] = "arm-vita-eabi-ld"
    env["AR"] = "arm-vita-eabi-ar"
    env["STRIP"] = "arm-vita-eabi-strip"
    env["RANLIB"] = "arm-vita-eabi-ranlib"
    ## Build type

    vita_sdk_path = os.environ.get("VITASDK")

    pkg_config_path = "{}/arm-vita-eabi/lib/pkgconfig/pkgconfig"

    os.environ["PKG_CONFIG_PATH"] = pkg_config_path
    env['ENV']['PKG_CONFIG_PATH'] = pkg_config_path


    env.Prepend(CPPPATH=['{}/arm-vita-eabi/include'.format(os.environ.get("VITASDK"))])
    env.Prepend(CPPPATH=['{}/arm-vita-eabi/include/freetype2'.format(os.environ.get("VITASDK"))])
    env.Prepend(CPPPATH=['{}/share/gcc-arm-vita-eabi/samples/common'.format(os.environ.get("VITASDK"))])
    env.Append(LIBPATH=['{}/arm-vita-eabi/lib'.format(os.environ.get("VITASDK"))])
    env.Prepend(LINKFLAGS=["-Wl,-q", "-unsafe", "-Wl,-z,nocopyreloc"])
    print(env.get("CPPPATH"))

    env.Prepend(CPPFLAGS=['-Wl,-q', '-unsafe', '-D_POSIX_TIMERS', '-DNO_THREADS', '-DUNIX_SOCKET_UNAVAILABLE', '-DVITA_ENABLED', '-DPOSH_COMPILER_GCC', '-DPOSH_OS_VITA', '-DPOSH_OS_STRING=\\"vita\\"'])


    if (env["target"] == "release"):
        # -O3 -ffast-math is identical to -Ofast. We need to split it out so we can selectively disable
        # -ffast-math in code for which it generates wrong results.
        if (env["optimize"] == "speed"): #optimize for speed (default)
            env.Prepend(CCFLAGS=['-O3', '-ffast-math'])
        else: #optimize for size
            env.Prepend(CCFLAGS=['-Os'])
     
        if (env["debug_symbols"] == "yes"):
            env.Prepend(CCFLAGS=['-g1'])
        if (env["debug_symbols"] == "full"):
            env.Prepend(CCFLAGS=['-g2'])

    elif (env["target"] == "release_debug"):
        if (env["optimize"] == "speed"): #optimize for speed (default)
            env.Prepend(CCFLAGS=['-O2', '-ffast-math', '-DDEBUG_ENABLED'])
        else: #optimize for size
            env.Prepend(CCFLAGS=['-Os', '-DDEBUG_ENABLED'])

        if (env["debug_symbols"] == "yes"):
            env.Prepend(CCFLAGS=['-g1'])
        if (env["debug_symbols"] == "full"):
            env.Prepend(CCFLAGS=['-g2'])

    elif (env["target"] == "debug"):
        env.Prepend(CCFLAGS=['-g3', '-DDEBUG_ENABLED', '-DDEBUG_MEMORY_ENABLED'])
        #env.Append(LINKFLAGS=['-rdynamic'])

        #env.Append(LINKFLAGS=['-rdynamic'])

    ## Architecture

    env["bits"] = "32"

    ## Flags

    # Linkflags below this line should typically stay the last ones
    #if not env['builtin_zlib']:
    #    env.ParseConfig('aarch64-none-elf-pkg-config zlib --cflags --libs')

    env.Append(CPPPATH=['#platform/vita'])
    env.Append(CPPFLAGS=['-DGLFW_INCLUDE_ES2', '-DLIBC_FILEIO_ENABLED', '-DOPENGL_ENABLED', '-DGLES_ENABLED'])
    env.Append(CPPFLAGS=['-DPTHREAD_NO_RENAME'])
    env.Append(LIBS=[
        "pib",
        "SceLibKernel_stub",
        "SceKernelThreadMgr_stub",
        "SceSysmodule_stub",
        "SceIofilemgr_stub",
        "freetype",
        "SceCommonDialog_stub",
        "SceGxm_stub",
        "SceDisplay_stub",
        "SceCtrl_stub",
        "SceTouch_stub",
        "liblibScePiglet_stub",
        "SceIncomingDialog_stub"
    ])
    print(env.get("LIBS"))

"""
        "libpib",
        "SceLibKernel_stub",
        "ScePvf_stub",
        "SceAppMgr_stub",
        "SceAppUtil_stub",
        "ScePgf_stub",
        "jpeg",
        "SceCommonDialog_stub",
        "SceGxm_stub",
        "SceDisplay_stub",
        "SceSysmodule_stub",
        "vitashark",
        "SceShaccCg_stub",
        "pthread"
"""
#-lglad -lEGL -lglapi -ldrm_nouveau 
