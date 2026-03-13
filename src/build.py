import sys
import re
import os.path as path

def print_help():
    print("usage: python build.py --macro <macro> --pub <files")
    return

def parse_files(args):
    files = []

    for arg in args:
        if arg.startswith("--"):
            break
        
        if not path.exists(arg):
            print(f"'{arg}' does not exist.")
            exit(1)
        elif path.isdir(arg):
            print(f"'{arg}' is a directory. Expected a file name.")
            exit(1)
        else:
            files.append(arg)
    
    return files

def omit_includes(code, files):
    for file in files:
        fname = path.basename(file)
        if file.endswith(".h"):
            code = code.replace(f"#include \"{fname}\"\n", "")
            code = code.replace(f"#include <{fname}>\n", "")
    return code

def omit_extern(code):
    return re.sub(r"^\s*extern\b[^;]*;\s*\n?", "", code, flags=re.MULTILINE)

# Main start
# ==========

if len(sys.argv) < 2:
    print_help()
    exit(1)

priv1_files = []
priv2_files = []
pub_files = []
cur_arg = 1
macro = ""
file_name =""

# Parse args
# ----------

while cur_arg < len(sys.argv):
    match sys.argv[cur_arg]:
        case "--help":
            print_help()
            exit(1)
        case "--macro":
            cur_arg += 1
            macro = sys.argv[cur_arg]
        case "--pub":
            cur_arg += 1
            pub_files = parse_files(sys.argv[cur_arg:])

            while cur_arg < len(sys.argv):
                if sys.argv[cur_arg].startswith("--"):
                    break
                cur_arg += 1
            continue
        case "--priv1":
            cur_arg += 1
            priv1_files = parse_files(sys.argv[cur_arg:])

            while cur_arg < len(sys.argv):
                if sys.argv[cur_arg].startswith("--"):
                    break
                cur_arg += 1
            continue
        case "--priv2":
            cur_arg += 1
            priv2_files = parse_files(sys.argv[cur_arg:])

            while cur_arg < len(sys.argv):
                if sys.argv[cur_arg].startswith("--"):
                    break
                cur_arg += 1
            continue
        case "--file_path":
            cur_arg += 1
            file_name = sys.argv[cur_arg]
        case _:
            print(f"Unknown argument '{sys.argv[cur_arg]}'")
    
    cur_arg += 1

# Assert the variables
# --------------------

if macro == "":
    macro = "SINGLE_HEADER"
    
macro = macro.upper()

if file_name == "":
    file_name = "single_header.h"

if path.exists(file_name):
    if path.isfile(file_name):
        print(f"The file '{file_name}' already exists.")
        resposta = input("Do you want to overwrite? (y/n): ")
        if resposta.lower() != "y":
            base, ext = path.splitext(file_name)
            i = 1
            new_file_name = f"{base}{i}{ext}"
            while path.exists(new_file_name):
                i += 1
                new_file_name = f"{base}{i}{ext}"
            file_name = new_file_name

# Generate the single header
# --------------------------

with open(file_name, "w") as out:
    for pub in pub_files:
        with open(pub, "r") as src:
            out.write(src.read())
        out.write("\n\n")
    
    out.write(f"#ifdef {macro}_IMPLEMENTATION\n")

    for priv1 in priv1_files:
        with open(priv1, "r") as src:
            code = omit_includes(src.read(),
                                    pub_files + priv1_files + priv2_files)
            code = omit_extern(code)
            out.write(code)
        out.write("\n")

    for priv2 in priv2_files:
        with open(priv2, "r") as src:
            out.write(omit_includes(src.read(),
                                    pub_files + priv1_files + priv2_files))
        out.write("\n")

    out.write(f"#endif // {macro}_IMPLEMENTATION\n")