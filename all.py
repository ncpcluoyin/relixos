import os
os.chdir("relix_core")
os.system("cargo xbuild --target x86_64-unknown-none.json --release")