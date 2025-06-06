import os

with open("test.txt", "w") as f:
    f.write("Hello, World!")

print("File created: test.txt")    

os.system("ls -l /proc/self/fd")
