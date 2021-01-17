# Password Vault

Have your Wio Terminal type your passwords.

![Type password to log into Amazon](promo/amazon-login.jpg)

This sketch for the Seeed Wio Terminal reads a list of passwords from a file "passwd.txt" on the SD card and displays them on the screen of a Wio Terminal. 

You can select an entry from the list by filtering and browsing with the 5-way joystick of the Wio Terminal.

![Filtering and browsing](promo/password-filtering.jpg)

Once you have selected an entry by pression the 5-way joystick, the password is typed to your computer by emulating a keyboard.

You switch between filter and list views by pressing the leftmost button.

There are many commercial password managers. However, they are expensive, have tiny screens and too few buttons to be comfortably usable. This PasswordVault is open source, quite usable already and it uses open, powerful hardware. The Wio Terminal by Seeed is inexpensive, has a big screen, 5-way joystick and additional buttons. In even has WiFi and Bluetooth, so there is plenty room for more powerful features in the future.

![Wio Terminal](promo/wioterminal.jpg)

ATTENTION: At the moment, the passwords are expected to be in clear text. This is of course highly insecure. If you insist on using this software in its current state, please make sure that nobody has access to the SD card containing the password file. 

LICENSE: GNU GENERAL PUBLIC LICENSE, Version 3
