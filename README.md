## Edith-HTTP-Web-Server
Edith is a C++ based HTTP Web Server which can serve static and dynamic web applications. Server side programming should be C/C++, which makes it very interesting.
## How to Use Edith
First of all you need to download the zip file, unzip it and in folder Edition One you can create your own folder for specific web application, as for testing I have made a folder named as webone in which I am developing my web application. In webone folder you need to create a cpp file which should **include edith** library and you need to use **namespace edith**.
```cpp
#include<edith>
using namespace edith;`
```
For compiling you need to run the command as
```
//for windows
g++ -static file_name.cpp -o file_name.exe -I ../edith/include -L ../edith/lib -ledith -lws2_32
```
