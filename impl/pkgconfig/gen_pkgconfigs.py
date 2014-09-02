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


def write_common(file, outdir, defines, chrome_src):
    # extract "out.{target_host}/{build_type}/gen" out of a full path 'outdir'
    index = outdir.find("out.")
    relativeOutdir = outdir[index:] + "/gen "

    file.write("Name: chromium-efl\n")
    file.write("Description: Chromium EFL port\n")
    file.write("Version: " + get_chrome_version(chrome_src) + "\n\n")
    file.write("Libs: -L${libdir} -Wl,-rpath-link=${libdir} -lchromium-efl\n")
    file.write("Cflags: -I${includedir}/impl -I${includedir}/src -I${includedir}/src/skia/config "
                   "-I/usr/include/v8 " + "-I${includedir}/" + relativeOutdir + defines + "\n")


def gen_gbs_pkgconfig_file(path, chrome_src, defines):
    pc_dir = os.path.join(path, 'pkgconfig')
    pc_path = os.path.join(pc_dir, 'chromium-efl.pc')

    print(" * Generating GBS chromium-efl.pc")

    if not os.path.exists(pc_dir):
        os.makedirs(pc_dir)

    out = open(pc_path, "w");

    out.write("prefix=/usr\n")
    out.write("exec_prefix=${prefix}\n")
    out.write("libdir=${prefix}/lib\n")
    out.write("includedir=../chromium\n\n")

    write_common(out, path, defines, chrome_src)

    out.close()


def gen_desktop_pkgconfig_file(path, chrome_src, defines):
    pc_dir = os.path.join(path, 'pkgconfig', 'desktop')
    pc_path = os.path.join(pc_dir , "chromium-efl.pc")
    top_dir = os.path.join(chrome_src, "..")

    print(" * Generating desktop chromium-efl.pc")

    if not os.path.exists(pc_dir):
        os.makedirs(pc_dir)

    out = open(pc_path, "w");

    out.write("prefix=" + path + "\n")
    out.write("exec_prefix=${prefix}\n")
    out.write("libdir=${prefix}/lib\n")
    out.write("includedir=" + top_dir + "\n\n")

    write_common(out, path, defines, chrome_src)

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

    gen_gbs_pkgconfig_file(outdir, chrome_src, defines)
    gen_desktop_pkgconfig_file(outdir, chrome_src, defines)

    sys.exit(0)


if __name__ == "__main__":
    main()
