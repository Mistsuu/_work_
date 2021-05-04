import subprocess
import time
import string
import random
import os
import signal

# Global variable
noClients = 950
cmds      = ["/list", "/quit", "/chat"]

def getRandomUsernames():
    return ["".join(random.choices(string.ascii_lowercase + string.ascii_uppercase + string.digits, k=random.randint(8, 15))) for _ in range(noClients)]

def getRandomUsername():
    return "".join(random.choices(string.ascii_lowercase + string.ascii_uppercase + string.digits, k=random.randint(8, 15)))

def getRandomString(l, r):
    return "".join(random.choices(string.ascii_lowercase + string.ascii_uppercase + string.digits, k=random.randint(l, r)))

def spawnClients():
    return [subprocess.Popen(["../c", "127.0.0.1"], stdin=subprocess.PIPE, stdout=subprocess.DEVNULL) for _ in range(noClients)]

def sendTo(process, msg):
    process.stdin.write((msg+"\n").encode())
    process.stdin.flush()

# init vars
processes = spawnClients()
usernames = getRandomUsernames()
isChating = [False]*noClients

def shutDown(sig, frame):
    print("[*] Shutting down...")
    for process in processes:
        try:
            sendTo(process, "/quit")
            time.sleep(0.1)     
            sendTo(process, "\n")
            time.sleep(0.1)
            sendTo(process, "\n")
            time.sleep(0.1)
            sendTo(process, "\n")
        except:
            pass

# send usernames
for process, username in zip(processes, usernames):
    sendTo(process, username)

# set sigint handler
signal.signal(signal.SIGINT, shutDown)

# Simulate 1000 time steps
print("[*] Simulating...")
for i in range(1000):
    for i in range(len(processes)):
        if random.random() < 0.3:
            cmd = random.choice(cmds)
            if cmd == "/list":
                sendTo(processes[i], "/list")
            elif cmd == "/quit" and isChating[i]:
                sendTo(processes[i], "/quit")
                isChating[i] = False
            elif cmd == "/chat" and not isChating[i]:
                if random.random() < 0.5:
                    sendTo(processes[i], "/chat " + random.choice(usernames))
                else:
                    sendTo(processes[i], "/chat " + getRandomUsername())
                isChating[i] = True
            time.sleep(random.random() * 0.5)
        else:
            sendTo(processes[i], getRandomString(1000, 2000))
            time.sleep(random.random() * 5 / noClients)

print("[*] Shutting down...")
for process in processes:
    try:
        sendTo(process, "/quit")
        time.sleep(0.1)     
        sendTo(process, "\n")
        time.sleep(0.1)
        sendTo(process, "\n")
        time.sleep(0.1)
        sendTo(process, "\n")
    except:
        pass
