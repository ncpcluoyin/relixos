
#imports

import os
from os import path
from pathlib import Path
import json

#imports end

#options

target = "x86_64-elf"

cc = target + "-gcc"
cxx = target + "-g++"

ld = target + "-ld"

rustc = target + "-gccrs"

make = "make"

commonflags = "-O3 -ffreestanding"

cflags = commonflags
cxxflags = commonflags

all_build_tools = [cc,cxx,ld,make,rustc]

#options_end

#check function

def __check_tools__func(tool_name:str):
    if os.system(tool_name + " --version") != True:
        print(tool_name + " not found in your system")

def check_tools(tools:list):
    for tool_name__ in tools:
        __check_tools__func(tool_name__)

#check function end


#directories create function

pre_create_dirs_abs_ = list()

def dir_create(_dirs_:list):
    for dir__ in _dirs_:
        os.system("mkdir -p -v " + dir__)

def add_pre_create_dir(__dir_name_____:str):
    pre_create_dirs_abs_.append(path.abspath(__dir_name_____))

#directories create function end


#gen makefile function


makefile_contains = str()

def gen_makefile(_source_path,_build_dir):
    os.chdir(_source_path)

    #add _build_dir into pre_create_dirs
    add_pre_create_dir(_build_dir)

    file_list = listdir()
    dir_list = list()
    configuration_json = dict()
    c_cxx_includes = list()
    if "configuration.json" in file_list:#read the configuration.json
        file_list.remove("configuration.json")
        configuration_json = json.loads(Path("configuration.json").read_text())
        if "ignore" in list(configuration_json.keys()):#ignore files and directories
            ignore_file_dir_list = configuration_json["ignore"]
            if not isinstance(ignore_file_dir_list,list):
                print("in " + path.abspath("configuration.json") + " \"ignore\" should be an array")
                print("exit")
                exit()
            for ignore_file_dir in ignore_file_dir_list:
                if ignore_file_dir in file_list:
                    file_list.remove(ignore_file_dir)
                else:
                    print(os.getcwd() + "/" + ignore_file_dir + " not found")
        if "c_cxx_includes" in list(configuration_json.keys()):#load c / cxx / asm includes
            includes__ = configuration_json["c_cxx_includes"]
            if not isinstance(includes__,list):
                print("in " + path.abspath("configuration.json") + " \"c_cxx_includes\" should be an array")
                print("exit")
                exit()
            c_cxx_includes = includes__
    file_list_back = file_list
    for file_dir_name__ in file_list_back:
        if path.isdir(file_dir_name__):
            file_list.remove(file_dir_name__)
            dir_list.append(file_dir_name__)
    for source_file in file_list:#add header files into c_cxx_includes
        if path.splitext(source_file)[-1] == ".h":
            c_cxx_includes.append(source_file)
        if path.splitext(source_file)[-1] == ".hxx":
            c_cxx_includes.append(source_file)
        if path.splitext(source_file)[-1] == ".hpp":
            c_cxx_includes.append(source_file)
        if path.splitext(source_file)[-1] == ".hh":
            c_cxx_includes.append(source_file)
    for source_file in file_list:#gen makefile
        if path.splitext(source_file)[-1] == ".c":
            c_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + ".o", c_cxx_includes)
        if path.splitext(source_file)[-1] == ".cxx":
            cxx_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + ".o", c_cxx_includes)
        if path.splitext(source_file)[-1] == ".cc":
            cxx_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + ".o", c_cxx_includes)
        if path.splitext(source_file)[-1] == ".cpp":
            cxx_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + ".o", c_cxx_includes)
        if path.splitext(source_file)[-1] == ".s":
            asm_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + ".o", c_cxx_includes)
        if path.splitext(source_file)[-1] == ".asm":
            asm_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + ".o", c_cxx_includes)
        if path.splitext(source_file)[-1] == ".S":
            asm_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + ".o", c_cxx_includes)
    for source_dir in dir_list:
        sp = _source_path
        bd = _build_dir
        gen_makefile(sp + "/" + source_dir,bd + "/" + source_dir)


def add_line(_T:str):
    makefile_contains = makefile_contains + _T + "\n"

def c_file_com(_I:str,_O:str,_R:list):#gen c language makefile
    relays = str()
    relays_dir_list = list()
    relays_file_list = list()
    for relay_file in _R:
        if not path.exists(path.abspath(relay_file)):
            print(path.abspath(relay_file) + " doesn't exist")
            print("but " + _I + " needs it")
            print("exit")
            exit()
        if path.isdir(relay_file):
            if not path.abspath(relay_file) in relays_dir_list:
                relays_dir_list.append(path.abspath(relay_file))
            for files in os.listdir(path.abspath(relay_file)):
                if path.isdir(files):
                    continue
                if not path.abspath(files) in relays_file_list:
                    relays_file_list.append(path.abspath(files))
        if path.isfile(relay_file):
            if not path.abspath(path.dirname(path.abspath(relay_file))) in relays_dir_list:
                relays_dir_list.append(path.abspath(path.dirname(path.abspath(relay_file))))
            if not path.abspath(relay_file) in relays_file_list:
                relays_file_list.append(path.abspath(relay_file))
    cc_includes = str()
    for includes in relays_dir_list:
        cc_includes = "-I" + includes + " " + cc_includes
    for includes in relays_file_list:
        relays = relays + " " + includes
    add_line(_O + ":" + relays + " " + _I)
    add_line("\t" + cc + " " + cc_includes + " " + cflags + " " + _I + " -o " + _O)

def cxx_file_com(_I:str,_O:str,_R:list):#gen c++ language makefile
    relays = str()
    relays_dir_list = list()
    relays_file_list = list()
    for relay_file in _R:
        if not path.exists(path.abspath(relay_file)):
            print(path.abspath(relay_file) + " doesn't exist")
            print("but " + _I + " needs it")
            print("exit")
            exit()
        if path.isdir(relay_file):
            if not path.abspath(relay_file) in relays_dir_list:
                relays_dir_list.append(path.abspath(relay_file))
            for files in os.listdir(path.abspath(relay_file)):
                if path.isdir(files):
                    continue
                if not path.abspath(files) in relays_file_list:
                    relays_file_list.append(path.abspath(files))
        if path.isfile(relay_file):
            if not path.abspath(path.dirname(path.abspath(relay_file))) in relays_dir_list:
                relays_dir_list.append(path.abspath(path.dirname(path.abspath(relay_file))))
            if not path.abspath(relay_file) in relays_file_list:
                relays_file_list.append(path.abspath(relay_file))
    cxx_includes = str()
    for includes in relays_dir_list:
        cxx_includes = "-I" + includes + " " + cxx_includes
    for includes in relays_file_list:
        relays = relays + " " + includes
    add_line(_O + ":" + relays + " " + _I)
    add_line("\t" + cxx + " " + cxx_includes + " " + cxxflags + " " + _I + " -o " + _O)

def asm_file_com(_I:str,_O:str,_R:list):#gen c language makefile
    relays = str()
    relays_dir_list = list()
    relays_file_list = list()
    for relay_file in _R:
        if not path.exists(path.abspath(relay_file)):
            print(path.abspath(relay_file) + " doesn't exist")
            print("but " + _I + " needs it")
            print("exit")
            exit()
        if path.isdir(relay_file):
            if not path.abspath(relay_file) in relays_dir_list:
                relays_dir_list.append(path.abspath(relay_file))
            for files in os.listdir(path.abspath(relay_file)):
                if path.isdir(files):
                    continue
                if not path.abspath(files) in relays_file_list:
                    relays_file_list.append(path.abspath(files))
        if path.isfile(relay_file):
            if not path.abspath(path.dirname(path.abspath(relay_file))) in relays_dir_list:
                relays_dir_list.append(path.abspath(path.dirname(path.abspath(relay_file))))
            if not path.abspath(relay_file) in relays_file_list:
                relays_file_list.append(path.abspath(relay_file))
    cc_includes = str()
    for includes in relays_dir_list:
        cc_includes = "-I" + includes + " " + cc_includes
    for includes in relays_file_list:
        relays = relays + " " + includes
    add_line(_O + ":" + relays + " " + _I)
    add_line("\t" + cc + " " + cc_includes + " " + cflags + " -c " + _I + " -o " + _O)

#gen makefile function end


#main function

project_build_dir = path.abspath(os.getcwd())
project_source_dir = path.abspath(path.dirname(__file__))


if __name__ == "__main__":
    check_tools(all_build_tools)
    gen_makefile(project_source_dir, project_build_dir)
    dir_create(pre_create_dirs_abs_)


#main function end