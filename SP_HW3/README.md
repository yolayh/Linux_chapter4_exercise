
#### Open my_shell
```
./my_shell
```

##### Function:

1. parse the sentence
```
# Input
myshell -> system program

# Output
[0] system
[1] program
```

2. echo
```
# Input
myshell -> echo -n 2 one two three

# Output
[0] echo
[1] -n
[2] 2
[3] one
[4] two
[5] three
two
```

3. terminate my_shell by input "exit", "quit", "logout" or "bye"
```
# Input
myshell -> quit

# Output
[0] quit
```

4. run command
```
# Input
myshell -> ls -l

# Output
[0] ls
[1] -l
總用量 68
-rw-rw-r-- 1 yolayh yolayh  2391  9月 14 01:06 builtin.c
-rw-rw-r-- 1 yolayh yolayh  3136  9月 17 18:36 builtin.o
...
```

5. run command at backgroud by adding "&"
```
# Input
myshell -> sleep 10 &    # check by using command "ps"
```

