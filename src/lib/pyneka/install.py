#!/usr/bin/env python3
import sys, os, shutil

so = "build/libcnkpyneka/pyneka.so"
if not os.path.exists(so):
    raise ValueError("Error pyneka module not found, looking at {}, did it build correctly?", so)

print("Choose a location to place the module in one of the values of \x1b[1m;sys.path\x1b[22m\n");
for i, path in enumerate(sys.path):
    print("  [{}] {}".format(i, path))

print("\nEnter number of the desired choice above: ", end="");
choice = input()
path = os.path.join(sys.path[int(choice)], os.path.basename(so)) 
print("Moving to {}".format(path));

shutil.copy2(so, os.path.join(sys.path[int(choice)], path))
