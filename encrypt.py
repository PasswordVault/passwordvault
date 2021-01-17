import os
from dotenv import load_dotenv
load_dotenv()

import xxtea

with open('passwd') as f:
    lines = f.readlines()

key = os.getenv("PASSWD").encode().ljust(16, b'\0')

for l in lines:
    name, passwd = l.split('\t', 1)
    passwd = passwd.rstrip()
    enc = xxtea.encrypt_hex(passwd, key)

    print(name + '\t' + enc.decode("ascii"))

