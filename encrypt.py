import os
import sys
from dotenv import load_dotenv
load_dotenv()

import xxtea

def encrypt(fname):
    with open(fname) as f:
        lines = f.readlines()

    key = os.getenv("PASSWD").encode().ljust(16, b'\0')

    for l in lines:
        name, passwd = l.split('\t', 1)
        passwd = passwd.rstrip()
        enc = xxtea.encrypt_hex(passwd, key)

        print(name + '\t' + enc.decode("ascii"))

if __name__ == '__main__':
    if len(sys.argv) > 1:
        encrypt(sys.argv[1])
    else:
        encrypt('passwd')