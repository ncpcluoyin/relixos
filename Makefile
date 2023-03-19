mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

all:
	cd $(mkfile_dir) && python3 all.py
clean:
	cd $(mkfile_dir) && python3 clean.py