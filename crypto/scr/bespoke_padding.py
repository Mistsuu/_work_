import telnetlib
import json

HOST = "socket.cryptohack.org"
PORT = 13386

def readline():
    return tn.read_until(b"\n")

def json_recv():
    line = readline()
    return json.loads(line.decode())

def json_send(hsh):
    request = json.dumps(hsh).encode()
    tn.write(request)

def get_data():
    opening = readline()
    json_send({
        "option" : "get_flag"
    })
    data = json_recv()
    return data

tn = telnetlib.Telnet(HOST, PORT)
data = get_data()

e    = int(data["encrypted_flag"])
m    = int(data["modulus"])
a, b = int(data["padding"][0]), int(data["padding"][1])