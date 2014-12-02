#!/usr/bin/env python

import sys, os, getopt
import subprocess


def get_command_output(args):
    proc = subprocess.Popen(args, stdout=subprocess.PIPE)
    (stdout, stderr) = proc.communicate()
    return (stdout, proc.returncode)


def get_chrome_version(chrome_src):
    tool = os.path.join(chrome_src, 'build/util/version.py')
    file = os.path.join(chrome_src, 'chrome/VERSION')

    (stdout, retval) = get_command_output([tool, '-f', file, '-t', '@MAJOR@.@MINOR@.@BUILD@'])

    if not retval:
        return stdout.rstrip()
    else:
        print("Failed to determine chromium version: " + stdout)
        return "0.0.0"


def gen_pkgconfig_file(path, version, defines):
    pc_dir = os.path.join(path, 'pkgconfig')
    pc_path = os.path.join(pc_dir, 'chromium-efl.pc')

    print(" * Generating GBS chromium-efl.pc")

    if not os.path.exists(pc_dir):
        os.makedirs(pc_dir)

    out = open(pc_path, "w");

    out.write("prefix=/usr\n")
    out.write("exec_prefix=${prefix}\n")
    out.write("libdir=${prefix}/lib\n")
    out.write("includedir=${prefix}/include\n\n")

    out.write("Name: chromium-efl\n")
    out.write("Description: Chromium EFL port\n")
    out.write("Version: " + version + "\n\n")
    out.write("Libs: -L${libdir} -Wl,-rpath-link=${libdir} -lchromium-efl -lchromium-ewk\n")
    out.write("Cflags: -I${includedir}/chromium-ewk -I/usr/include/v8 " + defines + "\n")

    out.close()


def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "o:d:c:", ["out-dir=", "defines=", "chrome-src="])
    except getopt.GetoptError as err:
        print (str(err))
        usage()
        sys.exit(2)

    outdir = None
    defines = None
    chrome_src = None
    for o, a in opts:
        if o in ("-o", "--out-dir"):
            outdir = a
        elif o in ("-d", "--defines"):
            defines = a
        elif o in ("-c", "--chrome-src"):
            chrome_src = a
        else:
            assert False, "unhandled option"

    if not (outdir and defines and chrome_src):
        print("Invalid arguments specified!")
        sys.exit(2)

    gen_pkgconfig_file(outdir, get_chrome_version(chrome_src), defines)

    sys.exit(0)


if __name__ == "__main__":
    main()
