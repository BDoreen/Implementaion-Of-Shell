# Implementation Of Shell
Second year C project. Project demands are to implemen a shell that supports regular commands (ls, mkdir..) and couple of other functions (listed below).  

# Function Description
**int encryptFile(char * from, int code, char * to)**  
Function encrypts the file "from" by adding "code" to the ascii value of the characters in the file.

**int decryptFile(char * from, int code, char * to)**  
Function decrypts the file "from" by decreasing "code" from the ascii value of the characters in the file.

**int letterFreq(char * from)**  
Function scans the file 'from' and finds three ABC letters that occur most of the time.  
Then, prints their frequency, and if the order of those three letters is a-e-o  
than it prints "Good Letter Frequency".  

**int uppercaseByIndex(char * from, char * to, int index)**  
Function scans each word in file 'from' and transforms word[index] to uppercase.  

**int lowercaseByIndex(char * rFile, char * wFile, int index)**  
Function scans each word in file from and transforms word[index] to lowercase.  

**int randomFile(int num, char * wFile)**  
Function writes "num" random ABC letters to 'wFile'  

**int compressFile(char * pathOpen, char * pathSave)**  
Function receives two paths, it scans the file of 'pathOpen',  
and compresses the words which occur more than 4 times in a row.  

**int lockCmdForTime(char * cmdToLock, int lockDuration, Node ** listLockedCmds)**  
Functions prevents the use of a given command by using linked list of locked commands.

# Files Describtion
**super_shell**
This file can be ran through Unix Shell by typing ./super_shell in the terminal.

**super_shell.c**
Contains int main() of the program.

**modules.c**
Contains implementation of the different functions in the program.

**myEnc.txt, myDec.txt, myUpperTxt.txt, myCompTxt.txt**
These are output txt files. 

**myTxt.txt, myTxt2.txt, good_letter_freq.txt**
These files contain required txt to demonstrate the different program functions.

![alt text](https://user-images.githubusercontent.com/90141260/133922724-5a298a1e-749d-4842-827b-0b11f18c5add.png)
![alt text](https://user-images.githubusercontent.com/90141260/133922725-75cdb4c0-345b-4a56-a10f-be134e64b927.png)
