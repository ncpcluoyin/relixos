#!/usr/bin/python3
#imports

import os
from os import path
from pathlib import Path
import json
import copy

#imports end

#options

options = json.loads(Path(path.dirname(path.abspath(__file__)) + "/configuration.json").read_text())

target = options["target"]

cc = target + "-gcc"
cxx = target + "-g++"

rustc = target + "-gccrs"

make = "make"

cflags = options["cflags"]
cxxflags = options["cxxflags"]
rustflags = options["rustflags"]
ldflags = options["ldflags"]

#force flags

commonflags_force = "-Wall -Wextra"

cflags_force = commonflags_force + " -ffreestanding"
cxxflags_force = commonflags_force + " -ffreestanding"
rustflags_force = commonflags_force + " -frust-incomplete-and-experimental-compiler-do-not-use"

#force flags end

all_build_tools = [cc,cxx,make,rustc]

#options_end

#check function

def __check_tools__func(tool_name:str):
    if os.system(tool_name + " --version") != 0:
        print(tool_name + " not found in your system")
        exit(1)
    else:
        print(tool_name + " found")

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

final_makefile = str()

all_object_files = list()

images = dict()

def _join():
    final_make(makefile_contains)


def gen_headers(files:list,links:dict):
    final_make("#created by configure.py")
    final_make("")
    a = str()
    for file in files:
        a = a + " " + file
    for link in list(links.values()):
        a = a + " " + link
    final_make("all:" + a)
    final_make("")

def final_make(_T:str):
    global final_makefile
    final_makefile = final_makefile + _T + "\n"

def gen_images(__images___:dict,objects:list):
    a = str()
    for file in objects:
        a = a + " " + file
    for image in list(__images___.keys()):
        add_line(__images___[image] + ":" + image + a)
        add_line("\t" + "cd " + path.dirname(__images___[image]) + " && " + cc + " -T " + image + " -o " + __images___[image] + a + " " + ldflags)

def gen_functions():
    final_make("clean:")
    final_make("\trm -rf build")
    for dirs in pre_create_dirs_abs_:
        final_make("\tmkdir -p -v " + dirs)
    final_make("")
    final_make("delete:")
    final_make("\trm -rf build Makefile")

def gen_makefile(_source_path,_build_dir):

    global cflags
    global cxxflags
    global rustflags

    os.chdir(_source_path)
    print("entering " + _source_path)

    #add _build_dir into pre_create_dirs
    add_pre_create_dir(_build_dir)

    #backup the flags
    bak_cflags = cflags
    bak_cxxflags = cxxflags
    bak_rustflags = rustflags

    file_list = os.listdir()
    dir_list = list()
    configuration_json = dict()
    c_cxx_includes = list()
    if "configuration.json" in file_list:#read the configuration.json
        print("configuration.json found")
        file_list.remove("configuration.json")
        configuration_json = json.loads(Path("configuration.json").read_text())
        if ("select" in list(configuration_json.keys())) and ("ignore" in list(configuration_json.keys())):
            print("There are both \"select\" and \"ignore\" in the " + path.abspath("configuration.json"))
            print("you should delete one of them")
            print("exit")
            exit(1)
        if "select" in list(configuration_json.keys()):
            select_list = configuration_json["select"]
            if not isinstance(select_list,list):
                print("in " + path.abspath("configuration.json") + " \"select\" should be an array")
                print("exit")
                exit(1)
            select_list_back = copy.deepcopy(select_list)
            for contains in select_list_back:
                if not path.exists(contains):
                    select_list.remove(contains)
            file_list = select_list
        if "ignore" in list(configuration_json.keys()):#ignore files and directories
            ignore_file_dir_list = configuration_json["ignore"]
            if not isinstance(ignore_file_dir_list,list):
                print("in " + path.abspath("configuration.json") + " \"ignore\" should be an array")
                print("exit")
                exit(1)
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
                exit(1)
            c_cxx_includes = includes__
        if "cflags" in list(configuration_json.keys()):
            if not isinstance(configuration_json["cflags"],str):
                print("in " + path.abspath("configuration.json") + " \"cflags\" should be a string")
                print("exit")
                exit(1)
            cflags = configuration_json["cflags"]
        if "cxxflags" in list(configuration_json.keys()):
            if not isinstance(configuration_json["cxxflags"],str):
                print("in " + path.abspath("configuration.json") + " \"cxxflags\" should be a string")
                print("exit")
                exit(1)
            cxxflags = configuration_json["cxxflags"]
        if "rustflags" in list(configuration_json.keys()):
            if not isinstance(configuration_json["rustflags"],str):
                print("in " + path.abspath("configuration.json") + " \"rustflags\" should be a string")
                print("exit")
                exit(1)
            rustflags = configuration_json["rustflags"]
    file_list_back = copy.deepcopy(file_list)
    for file_dir_name__ in file_list_back:
        if path.isdir(file_dir_name__):
            file_list.remove(file_dir_name__)
            dir_list.append(file_dir_name__)
    for source_file in file_list:#add header files into c_cxx_includes
        if path.splitext(source_file)[-1] == ".h":
            c_cxx_includes.append(source_file)
            print(source_file)
        if path.splitext(source_file)[-1] == ".hxx":
            c_cxx_includes.append(source_file)
            print(source_file)
        if path.splitext(source_file)[-1] == ".hpp":
            c_cxx_includes.append(source_file)
            print(source_file)
        if path.splitext(source_file)[-1] == ".hh":
            c_cxx_includes.append(source_file)
            print(source_file)
        if path.splitext(source_file)[-1] == ".h++":
            c_cxx_includes.append(source_file)
            print(source_file)
    for source_file in file_list:#gen makefile
        if path.splitext(source_file)[-1] == ".c":
            c_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_c.o", c_cxx_includes,cflags)
            print(source_file + " : " + path.splitext(source_file)[0] + "_c.o")
        if path.splitext(source_file)[-1] == ".cxx":
            cxx_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_cxx.o", c_cxx_includes,cxxflags)
            print(source_file + " : " + path.splitext(source_file)[0] + "_cxx.o")
        if path.splitext(source_file)[-1] == ".cc":
            cxx_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_cc.o", c_cxx_includes,cxxflags)
            print(source_file + " : " + path.splitext(source_file)[0] + "_cc.o")
        if path.splitext(source_file)[-1] == ".cpp":
            cxx_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_cpp.o", c_cxx_includes,cxxflags)
            print(source_file + " : " + path.splitext(source_file)[0] + "_cpp.o")
        if path.splitext(source_file)[-1] == ".c++":
            cxx_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_c++.o", c_cxx_includes,cxxflags)
            print(source_file + " : " + path.splitext(source_file)[0] + "_c++.o")
        if path.splitext(source_file)[-1] == ".s":
            asm_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_s.o", c_cxx_includes,cflags)
            print(source_file + " : " + path.splitext(source_file)[0] + "_s.o")
        if path.splitext(source_file)[-1] == ".asm":
            asm_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_asm.o", c_cxx_includes,cflags)
            print(source_file + " : " + path.splitext(source_file)[0] + "_asm.o")
        if path.splitext(source_file)[-1] == ".S":
            asm_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_S.o", c_cxx_includes,cflags)
            print(source_file + " : " + path.splitext(source_file)[0] + "_S.o")
        if path.splitext(source_file)[-1] == ".lds":
            images[path.abspath(source_file)] = path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_lds.bin"
            print(source_file + " : " + path.splitext(source_file)[0] + "_lds.bin")
        if path.splitext(source_file)[-1] == ".ld":
            images[path.abspath(source_file)] = path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_ld.bin"
            print(source_file + " : " + path.splitext(source_file)[0] + "_ld.bin")
        if path.splitext(source_file)[-1] == ".rs":
            rust_file_com(path.abspath(source_file),path.abspath(_build_dir) + "/" + path.splitext(source_file)[0] + "_rs.o",rustflags)
            print(source_file + " : " + path.splitext(source_file)[0] + "_rs.o")

    #reset flags
    cflags = bak_cflags
    cxxflags = bak_cxxflags
    rustflags = bak_rustflags

    for source_dir in dir_list:
        sp = _source_path
        bd = _build_dir
        gen_makefile(sp + "/" + source_dir,bd + "/" + source_dir)


def add_line(_T:str):
    global makefile_contains
    makefile_contains = makefile_contains + _T + "\n"

def c_file_com(_I:str,_O:str,_R:list,flags:str):#gen c language makefile
    relays = str()
    relays_dir_list = list()
    relays_file_list = list()
    for relay_file in _R:
        if not path.exists(path.abspath(relay_file)):
            print(path.abspath(relay_file) + " doesn't exist")
            print("but " + _I + " needs it")
            print("exit")
            exit(1)
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
    add_line("\t" + "cd " + path.dirname(_O) + " && " + cc + " " + cc_includes + " " + flags + " " + cflags_force + " -c " + _I + " -o " + _O)
    all_object_files.append(_O)

def cxx_file_com(_I:str,_O:str,_R:list,flags:str):#gen c++ language makefile
    relays = str()
    relays_dir_list = list()
    relays_file_list = list()
    for relay_file in _R:
        if not path.exists(path.abspath(relay_file)):
            print(path.abspath(relay_file) + " doesn't exist")
            print("but " + _I + " needs it")
            print("exit")
            exit(1)
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
    add_line("\t" + "cd " + path.dirname(_O) + " && " + cxx + " " + cxx_includes + " " + flags + " " + cxxflags_force + " -c " + _I + " -o " + _O)
    all_object_files.append(_O)

def asm_file_com(_I:str,_O:str,_R:list,flags:str):#gen asm language makefile
    relays = str()
    relays_dir_list = list()
    relays_file_list = list()
    for relay_file in _R:
        if not path.exists(path.abspath(relay_file)):
            print(path.abspath(relay_file) + " doesn't exist")
            print("but " + _I + " needs it")
            print("exit")
            exit(1)
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
    add_line("\t" + "cd " + path.dirname(_O) + " && " + cc + " " + cc_includes + " " + flags  + " " + cflags_force + " -c " + _I + " -o " + _O)
    all_object_files.append(_O)

def rust_file_com(_I:str,_O:str,flags:str):#gen rust language makefile
    add_line(_O + ": " + _I)
    add_line("\t" +  "cd " + path.dirname(_O) + " && " + rustc + " " + flags + " " + rustflags_force + " -c " + _I + " -o " + _O)
    all_object_files.append(_O)

#gen makefile function end


#main function

project_build_dir = path.abspath(os.getcwd())
project_source_dir = path.abspath(path.dirname(__file__))


if __name__ == "__main__":
    #preparation
    check_tools(all_build_tools)
    gen_makefile(project_source_dir, project_build_dir + "/build")
    gen_images(images,all_object_files)
    gen_headers(all_object_files,images)
    _join()
    gen_functions()

    #gen makefile and create directories
    dir_create(pre_create_dirs_abs_)
    makefile_output = open(project_build_dir + "/Makefile","w")
    makefile_output.writelines(final_makefile)
    makefile_output.close()

    #end
    print("done")
    exit(0)

#main function end