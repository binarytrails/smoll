#!/usr/bin/python

lines = None

with open('ftp-betterdefaultpasslist.txt', 'r') as f: lines=f.readlines()

for line in lines: 
    user,pwd = line.split(':') 
    print(user, pwd) 

for line in lines: 
    user,pwd = line.split(':') 
    with open('users.txt', 'w') as f: 
        f.write(user.strip() + '\n') 
    with open('pwds.txt', 'w') as f: 
        f.write(pwd.strip() + '\n') 
            
