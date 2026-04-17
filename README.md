
# When did we use AI?

prompt 1. explain this c syntax: bind(server_fd, (struct sockaddr*)&address, sizeof(address))

what we learned:

python analogy ~ bind(socket, (HOST,PORT)), so the ugly part, which is (struct sockaddr*)&address, just means to
take the memory address of "address" (because its a big structure, probably living on the heap), and then we cast it
to a type of (struct sockaddr*), a pointer basically. This means that C just expects a pointer, so we pass it the pointer of address.

struct sockaddr * <-> pointer of type struct sockaddr. Actual memory layout is struct sockaddr_in, but we need struct sockaddr *. We have to get its memory location &sockaddr_in, and then cast it as a sockaddr, i.e, (struct sockaddr *)

# Utils

Format Specifiers in C - Geeksforgeeks

C: perror, stdout output order - https://stackoverflow.com/questions/8773996

# Parsing stuff

Command Line Arguments in C - Geeksforgeeks

Convert String to int in C - Geeksforgeeks

Get a Substring in C - Geeksforgeeks

C string strlen() Function - W3Schools

C strcmp() - Geeksforgeeks

Opendir function() in C use: - reddit.com/r/learnprogramming/...

# Sockets

A Beginners Guide to Socket Programming in C - SanjayRV, dev.to

Linux File Descriptors - medium.com/@tharinduimalka915

C sockets for dummies - Nita Andrei

Socket Programming in C - Geeksforgeeks

# I/O

Check substring exists in a string in C - stackoverflow.com/questions/12784766