## Edith-HTTP-Web-Server
Edith is a C++ based HTTP Web Server which can serve static and dynamic web applications. Server side programming should be C/C++, which makes it very interesting.
## How to Use Edith
First of all you need to download the zip file, unzip it and in folder Edition One you can create your own folder for specific web application, as for testing I have made a folder named as webone in which I am developing my web application. In webone folder you need to create a cpp file which should **include edith** library and you need to use **namespace edith**.
```cpp
#include<edith>
using namespace edith;
```
For compiling you need to run the command as
```
//for windows
g++ -static file_name.cpp -o file_name.exe -I ../edith/include -L ../edith/lib -ledith -lws2_32
```
For creating instance of Web Server and starting it
```cpp
EdithWebServer server(5050);                  //Creating object
server.onRequest("/addStudent",addStudent);   //mapping request with function
server.onRequest("/getStudent",getStudent);   //mapping request with function
server.start();                               //Starting the server
```

Whenever a request is arrived at server end, it checked that it is a client side resource or server side resource that means there has to be processing on server side or directly the resouce has to be served.

Such as request arrived for a.jpeg (this is a client side resource, no processing is required) so directly contents of the resource are served.

Request arrived for /addStudent (this is a server side script, that needs to be processed) after processing, necessary actions are taken such as forwarding request to another resource or sending contents of response.

## Demonstration Of Request forwarding feature
If application developer wants to forward the request to anyother resource (it can be server side or client side resource), he/she has to use forward() function.
```cpp
void aaa(Request &request,Response &response)
{
//many many lines of code of processing
request.forward("bbb");
//or it can be
// request.forward("abcd.html?x=10");
}
```

## Demonstration of creating Web Application using Viper Web Server
This script contains code to add student's details (roll number and name to file using file handling) and get details of student.
[serverscript.cpp](https://pip.pypa.io/en/stable/)
