# Password Vault

Have your Wio Terminal type your passwords.

![Type password to log into Amazon](promo/amazon-login.jpg)

This sketch for the Seeed Wio Terminal reads a list of passwords from a file "/olav.txt" on the SD card and displays them on the screen of a Wio Terminal. This is in preparation to manage multiple password files for different users.

You can select an entry from the list by filtering and browsing with the 5-way joystick of the Wio Terminal.

![Filtering and browsing](promo/password-filtering.jpg)

Once you have selected an entry by pression the 5-way joystick, the password is typed to your computer by emulating a keyboard.

You switch between filter and list views by pressing the leftmost button.

## How to prepare the password file

I use https://www.passwordstore.org/ to manage my passwords. There is a simple Python script to export these passwords into a file that PasswordVault can read. 

ATTENTION: Currently, this file contains your passwords in cleartext. Do not keep this file lying around!

To export your passwords, first install the dependencies into a virtual Python environment, then simply run it:

````
python3 -m venv env
source env/bin/activate
pip install -r requirements.txt
python3 passwords.py > olav.txt
````
## Other options

There are many commercial password managers. However, they are expensive, have tiny screens and too few buttons to be comfortably usable. Here are some examples:

* [Password Vault](https://hackaday.io/project/18763-password-vault) by mwahid manages upt to four passwords 
* [Hardware password manager](https://hackaday.io/project/5588-hardware-password-manager) by Voja Antonic manages up to eight passwords
* [CQKey - hardware password keeper](https://hackaday.io/project/11190-cqkey-hardware-password-keeper) has a small display and manages up to 10 passwords
* [Password Vault by John N Hansen (affiliate link)](https://amzn.to/3sAGlZQ) with small display and full keyboard
* [Mooltipass](https://www.themooltipass.com/) with small screen and NFC card reader
 
There is even [a paper notebook (affiliate link)](https://amzn.to/3qnLjHu), specifically to keep passwords. Don't loose it :)

Here are [more examples](https://hackaday.io/search?term=password+vault) on Hackaday.


This PasswordVault is open source, quite usable already and it uses open, powerful hardware. The Wio Terminal by Seeed is inexpensive, has a big screen, 5-way joystick and additional buttons. In even has WiFi and Bluetooth, so there is plenty room for more powerful features in the future.

![Wio Terminal](promo/wioterminal.jpg)

ATTENTION: At the moment, the passwords are expected to be in clear text. This is of course highly insecure. If you insist on using this software in its current state, please make sure that nobody has access to the SD card containing the password file. 

LICENSE: GNU GENERAL PUBLIC LICENSE, Version 3
