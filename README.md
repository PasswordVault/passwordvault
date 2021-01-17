# Password Vault

Have your Wio Terminal type your passwords.
This sketch for the Seeed Wio Terminal reads a list of passwords from a file "passwd.txt" on the SD card and displays them on the screen of a Wio Terminal. 

You can select an entry from the list by filtering and browsing with the 5-way joystick of the Wio Terminal.

Once you have selected an entry by pression the 5-way joystick, the password is typed to your computer by emulating a keyboard.

You switch between filter and list views by pressing the leftmost button.

ATTENTION: At the moment, the passwords are expected to be in clear text. This is of course highly insecure. If you insist on using this software in its current state, please make sure that nobody has access to the SD card containing the password file. 

LICENSE: GNU GENERAL PUBLIC LICENSE, Version 3
