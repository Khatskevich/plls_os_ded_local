import sys
import os
import subprocess
import commands

store_dir_path = sys.argv[1]

subprocess.call(["make","clean"])
subprocess.call(["make"])

simple_size = [0]
dedublicated_size = [0]

for file in os.listdir(store_dir_path):
    filepath = store_dir_path + file
    if os.path.isfile(filepath):
        print filepath
        subprocess.call(['./a.out','store', filepath])
        simple_size.append(os.stat(filepath).st_size + simple_size[-1])
        dedublicated_size.append(commands.getoutput('du -shb '+"./base_dir").split()[0])
for file in os.listdir(store_dir_path):
    filepath = store_dir_path + file
    if os.path.isfile(filepath):
        temp_file_path = '/tmp/' + file
        subprocess.call(['./a.out','restore', file, temp_file_path])
        temp_hash = commands.getoutput('md5hash '+ temp_file_path).split()[0]
        file_hash = commands.getoutput('md5hash '+ filepath).split()[0]
        os.remove(temp_file_path)
        if temp_hash != file_hash:
            print "Unsuccessfull file restore : " + filepath
            exit(1)
        else:
            print filepath + " Ok!"

print "Simple Dedublicated"
for i in range(0, len(simple_size)-1):
    print str(simple_size[i]) + " " + str(dedublicated_size[i])

