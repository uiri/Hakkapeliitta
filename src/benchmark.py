#!/usr/bin/env python2

import subprocess
import os
import fcntl
import time

def setNonBlocking(fd):
    flags = fcntl.fcntl(fd, fcntl.F_GETFL) | os.O_NONBLOCK
    fcntl.fcntl(fd, fcntl.F_SETFL, flags)

def get_stdout(p):
    count = 0
    while True:
        try:
            next_byte = p.stdout.read()
            if next_byte:
                return str(next_byte)
            if p.poll() != None:
                return ""
        except IOError:
            if p.poll() != None:
                return ""
            continue

p = subprocess.Popen(["./Hakkapeliitta"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize = 1)
setNonBlocking(p.stdin)
setNonBlocking(p.stdout)

line = get_stdout(p)
lcount = 0
posstr = "position startpos"
while line:
    if lcount == 1:
        p.stdin.write("uci\n")
    elif lcount == 2:
        p.stdin.write("isready\n")
    elif lcount == 3:
        p.stdin.write(posstr + "\n")
        print(posstr)
        posstr += " moves"
        p.stdin.write("go\n")
    elif "bestmove" in line:
        posstr += " "
        posstr += line.split("bestmove")[1].strip().split(" ")[0]
        p.stdin.write(posstr + "\n")
        print(posstr)
        p.stdin.write("go\n")
    line = get_stdout(p)
    lcount += 1
