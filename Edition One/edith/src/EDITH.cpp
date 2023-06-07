#include <edith>
#include <Windows.h>
#include <bits/stdc++.h>

using namespace edith;
using namespace std;

void Request::setKeyValue(string key, string value)
{
    keyValues.insert(pair<string, string>(key, value));
}

string Request::getValue(string key)
{
    map<string, string>::iterator iter;
    iter = keyValues.find(key);
    if (iter == keyValues.end())
        return string("");
    return iter->second;
}

string Request::get(string name)
{
    string val;
    int i, e;
    for (i = 0; i < this->dataCount; i++)
    {
        for (e = 0; data[i][e] != '\0' && data[i][e] != '='; e++)
            ;
        if (data[i][e] != '=')
            continue; // back to next cycle of for loop
        if (strncmp(data[i], name.c_str(), e) == 0)
            break;
    }
    if (i == this->dataCount)
    {
        val = "";
    }
    else
    {
        val = decode(string(data[i] + (e + 1)));
    }
    return val;
}

void Request::forward(string forwardTo)
{
    this->forwardTo = forwardTo;
}

Response::Response(int clientSocketDescriptor)
{
    this->clientSocketDescriptor = clientSocketDescriptor;
    this->isClosed = false;
    this->headerCreated = false;
}

void Response::createHeader()
{
    char header[51];
    strcpy(header, "HTTP/1.1 200 OK\nContent-Type:text/html\n\n");
    send(clientSocketDescriptor, header, strlen(header), 0);
    this->headerCreated = true;
}

string Request::decode(string requestURL)
{
    string m;
    for (int e = 0; e < requestURL.size(); e++)
    {
        if (requestURL[e] == '+')
            m += ' ';
        else
            m += requestURL[e];
    }
    return m;
}

void Response::write(const char *str)
{
    if (str == NULL)
        return;
    int len = strlen(str);
    if (len == 0)
        return;
    if (!this->headerCreated)
        createHeader();
    send(clientSocketDescriptor, str, len, 0);
}

void Response::write(string str)
{
    write(str.c_str());
}

void Response::close()
{
    if (this->isClosed)
        return;
    closesocket(this->clientSocketDescriptor);
    this->isClosed = true;
}

int extensionEquals(const char *left, const char *right)
{
    char a, b;
    while (*left && *right)
    {
        a = *left;
        b = *right;
        if (a >= 65 && a <= 90)
            a += 32;
        if (b >= 65 && b <= 90)
            b += 32;
        if (a != b)
            return 0;
        left++;
        right++;
    }
    return *left == *right;
}

char *getMIMEType(char *resource)
{
    char *mimeType;
    mimeType = NULL;
    int len = strlen(resource);
    if (len < 4)
        return mimeType;
    int lastIndexOfDot = len - 1;
    while (lastIndexOfDot > 0 && resource[lastIndexOfDot] != '.')
        lastIndexOfDot--;
    if (lastIndexOfDot == 0)
        return mimeType;
    if (extensionEquals(resource + lastIndexOfDot + 1, "html"))
    {
        mimeType = (char *)malloc(sizeof(char) * 10);
        strcpy(mimeType, "text/html");
    }
    if (extensionEquals(resource + lastIndexOfDot + 1, "js"))
    {
        mimeType = (char *)malloc(sizeof(char) * 16);
        strcpy(mimeType, "text/javascript");
    }
    if (extensionEquals(resource + lastIndexOfDot + 1, "css"))
    {
        mimeType = (char *)malloc(sizeof(char) * 9);
        strcpy(mimeType, "text/css");
    }
    if (extensionEquals(resource + lastIndexOfDot + 1, "jpg"))
    {
        mimeType = (char *)malloc(sizeof(char) * 11);
        strcpy(mimeType, "image/jpeg");
    }
    if (extensionEquals(resource + lastIndexOfDot + 1, "jpeg"))
    {
        mimeType = (char *)malloc(sizeof(char) * 11);
        strcpy(mimeType, "image/jpeg");
    }
    if (extensionEquals(resource + lastIndexOfDot + 1, "png"))
    {
        mimeType = (char *)malloc(sizeof(char) * 10);
        strcpy(mimeType, "image/png");
    }
    if (extensionEquals(resource + lastIndexOfDot + 1, "ico"))
    {
        mimeType = (char *)malloc(sizeof(char) * 13);
        strcpy(mimeType, "image/x-icon");
    }
    return mimeType;
}

char isClientSideResource(char *resource)
{
    int i;
    for (i = 0; resource[i] != '\0' && resource[i] != '.'; i++)
        ;
    if (resource[i] == '\0')
        return 'N';
    return 'Y'; // this will have to be changed later on
}

Request *parseRequest(char *bytes)
{
    // GET / HTTP/1.1
    // method / resource
    char method[11];
    char resource[1001];
    int i, j;
    for (i = 0; bytes[i] != ' '; i++)
        method[i] = bytes[i];
    method[i] = '\0';
    i += 2;

    char **data = NULL;
    int dataCount = 0;
    if (strcmp(method, "GET") == 0)
    {
        // register?nm=Bhavik+Mundra&ct=Ujjain
        for (j = 0; bytes[i] != ' '; j++, i++)
        {
            if (bytes[i] == '?')
                break;
            resource[j] = bytes[i];
        }
        resource[j] = '\0';
        if (bytes[i] == '?')
        {
            i++;
            int si = i;
            dataCount = 0;
            while (bytes[i] != ' ')
            {
                if (bytes[i] == '&')
                    dataCount++;
                i++;
            }
            dataCount++;
            data = (char **)malloc(sizeof(char *) * dataCount);
            int *pc = (int *)malloc(sizeof(int) * dataCount);
            i = si;
            int j = 0;
            while (bytes[i] != ' ')
            {
                if (bytes[i] == '&')
                {
                    pc[j] = i;
                    j++;
                }
                i++;
            }
            pc[j] = i;
            i = si;
            j = 0;
            int howManyToPick;
            while (j < dataCount)
            {
                howManyToPick = pc[j] - i;
                data[j] = (char *)malloc(sizeof(char) * (howManyToPick + 1));
                strncpy(data[j], bytes + i, howManyToPick);
                data[j][howManyToPick] = '\0';
                i = pc[j] + 1;
                j++;
            }
        }
    } // method is of GET type
    printf("Request arrived for %s\n", resource);
    // Request *request = (Request *)malloc(sizeof(Request));
    Request *request = new Request; // v v v v v v v important change, agar functions wagerah call krenge toh dikkat ho jaayegi malloc se
    request->dataCount = dataCount;
    request->data = data;
    request->method = (char *)malloc((sizeof(char) * strlen(method)) + 1);
    strcpy(request->method, method);
    if (resource[0] == '\0')
    {
        request->resource = NULL;
        request->isClientSideTechnologyResource = 'Y';
        request->mimeType = NULL;
    }
    else
    {
        request->resource = (char *)malloc((sizeof(char) * strlen(resource)) + 1);
        strcpy(request->resource, resource);
        request->isClientSideTechnologyResource = isClientSideResource(resource);
        request->mimeType = getMIMEType(resource);
        printf("Resource: %s\n", request->resource);
        printf("MIME Type: %s\n", request->mimeType);
        printf("Is Client Side Resource: %c\n", request->isClientSideTechnologyResource);
    }
    return request;
}

// Constructor
EdithWebServer::EdithWebServer(int portNumber)
{
    this->portNumber = portNumber;
}

// Destructor
EdithWebServer::~EdithWebServer()
{
}

void EdithWebServer::onRequest(string url, void (*ptrOnRequest)(Request &, Response &))
{
    if (url.length() == 0 || ptrOnRequest == NULL)
        return;
    requestMappings.insert(pair<string, void (*)(Request &, Response &)>(url, ptrOnRequest));
}

void EdithWebServer::start()
{
    FILE *f;
    int length;
    int i, rc; // readcount
    char g;
    char responseBuffer[1024]; // a chunk of of 1024 + 1 space for string terminator
    char requestBuffer[8192];  // GET 8KB = 8192 = 1024 x 8
    WORD ver;
    WSADATA wsaData;
    int serverSocketDescriptor;
    int clientSocketDescriptor;
    struct sockaddr_in serverSocketInformation;
    struct sockaddr_in clientSocketInformation;
    int successCode;
    int bytesExtracted;
    int len;

    ver = MAKEWORD(1, 1);
    WSAStartup(ver, &wsaData); // socket library initialized
    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketDescriptor < 0)
    {
        printf("Unable to create socket\n");
        return;
    }
    serverSocketInformation.sin_family = AF_INET;
    serverSocketInformation.sin_port = htons(this->portNumber);
    serverSocketInformation.sin_addr.s_addr = htonl(INADDR_ANY);
    successCode = bind(serverSocketDescriptor, (struct sockaddr *)&serverSocketInformation, sizeof(serverSocketInformation));
    if (successCode < 0)
    {
        printf("Unable to bind socket to port %d\n", this->portNumber);
        WSACleanup();
        return;
    }
    listen(serverSocketDescriptor, 10);
    len = sizeof(clientSocketInformation);
    while (1) // loop to accept connection
    {
        printf("Server is ready to accept request on port %d\n", this->portNumber);
        clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *)&clientSocketInformation, &len);
        if (clientSocketDescriptor < 0)
        {
            printf("Unable to accept client connection");
            closesocket(serverSocketDescriptor);
            WSACleanup();
            return;
        }
        bytesExtracted = recv(clientSocketDescriptor, requestBuffer, 8192, 0);
        if (bytesExtracted < 0)
        {
            // what to do is yet to be decided
        }
        else if (bytesExtracted == 0)
        {
            // what to do is yet to be decided
        }
        else
        {
            requestBuffer[bytesExtracted] = '\0';
            Request *request = parseRequest(requestBuffer);
            while (1) // infinite loop to enable the forwarding feature
            {
                if (request->isClientSideTechnologyResource == 'Y')
                {
                    if (request->resource == NULL)
                    {
                        f = fopen("index.html", "rb");
                        if (f != NULL)
                            printf("Sending index.html\n");
                        if (f == NULL)
                        {
                            f = fopen("index.htm", "rb");
                            if (f != NULL)
                                printf("Sending index.htm\n");
                        }
                        if (f == NULL)
                        {
                            strcpy(responseBuffer, "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:163\nConnection: close\n\n<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><title>Bhavik's Web Projector</title></head><body><h2 style='color:red'>Resource / Not Found</h2></body></html>");
                            send(clientSocketDescriptor, responseBuffer, strlen(responseBuffer), 0);
                            printf("Sending 404 Page\n");
                            closesocket(clientSocketDescriptor);
                            break; // introduced because of the forwarding feature
                        }
                        else
                        {
                            fseek(f, 0, 2); // move the internal pointer to the end of file
                            length = ftell(f);
                            fseek(f, 0, 0);                                                                                                       // move the internal pointer to the start of file                                                                                                                     // move the internal pointer to the start of file
                            sprintf(responseBuffer, "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection: close\n\n", length); // Header is Ready
                            send(clientSocketDescriptor, responseBuffer, strlen(responseBuffer), 0);
                            i = 0;
                            while (i < length)
                            {
                                rc = length - i;
                                if (rc > 1024)
                                    rc = 1024;
                                fread(&responseBuffer, rc, 1, f);
                                send(clientSocketDescriptor, responseBuffer, rc, 0);
                                i += rc;
                            }
                            fclose(f);
                            closesocket(clientSocketDescriptor);
                            break; // introduced because of the forwarding feature
                        }
                    }
                    else
                    {
                        // resource name is present
                        f = fopen(request->resource, "rb");
                        if (f != NULL)
                            printf("Sending %s\n", request->resource);
                        if (f == NULL)
                        {
                            // something needs to be done over here
                            printf("Sending 404 Page\n");
                            char tmp[501];
                            sprintf(tmp, "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><title>Bhavik's Web Projector</title></head><body><h2 style='color:red'>Resource %s Not Found</h2></body></html>", request->resource);
                            sprintf(responseBuffer, "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection: close\n\n", strlen(tmp));
                            strcat(responseBuffer, tmp);
                            send(clientSocketDescriptor, responseBuffer, strlen(responseBuffer), 0);
                            closesocket(clientSocketDescriptor);
                            break; // introduced because of the forwarding feature
                        }
                        else
                        {
                            fseek(f, 0, 2); // move the internal pointer to the end of file
                            length = ftell(f);
                            fseek(f, 0, 0);                                                                                                                   // move the internal pointer to the start of file
                            sprintf(responseBuffer, "HTTP/1.1 200 OK\nContent-Type:%s\nContent-Length:%d\nConnection: close\n\n", request->mimeType, length); // Header is Ready
                            send(clientSocketDescriptor, responseBuffer, strlen(responseBuffer), 0);
                            i = 0;
                            while (i < length)
                            {
                                rc = length - i;
                                if (rc > 1024)
                                    rc = 1024;
                                fread(&responseBuffer, rc, 1, f);
                                send(clientSocketDescriptor, responseBuffer, rc, 0);
                                i += rc;
                            }
                            fclose(f);
                            closesocket(clientSocketDescriptor);
                            break; // introduced because of the forwarding feature
                        }
                    }
                }
                else
                {
                    // more changes required in context to server side resource
                    map<string, void (*)(Request &, Response &)>::iterator iter;
                    iter = requestMappings.find(string("/") + string(request->resource));
                    if (iter == requestMappings.end())
                    {
                        printf("Sending 404 Page\n");
                        char tmp[501];
                        sprintf(tmp, "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><title>Bhavik's Web Projector</title></head><body><h2 style='color:red'>Resource %s Not Found</h2></body></html>", request->resource);
                        sprintf(responseBuffer, "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection: close\n\n", strlen(tmp));
                        strcat(responseBuffer, tmp);
                        send(clientSocketDescriptor, responseBuffer, strlen(responseBuffer), 0);
                        closesocket(clientSocketDescriptor);
                        break; // introduced because of the forwarding feature
                    }
                    else
                    {
                        Response response(clientSocketDescriptor);
                        iter->second(*request, response);
                        if (request->forwardTo.length() > 0)
                        {
                            free(request->resource);
                            request->resource = (char *)malloc(sizeof(char) * request->forwardTo.length() + 1);
                            strcpy(request->resource, request->forwardTo.c_str());
                            request->isClientSideTechnologyResource = isClientSideResource(request->resource);
                            request->mimeType = getMIMEType(request->resource);
                            request->forwardTo = "";
                            continue;
                        }
                        cout << "Forwarding Request to : " << request->resource << endl;
                        if (request->data != NULL)
                        {
                            for (int k = 0; k < request->dataCount; k++)
                                free(request->data[k]);
                            free(request->data);
                        }
                        break; // introduced because of the forwarding feature
                    }
                }
            } // the infinite loop introduced because of the forwarding feature ends here
        }
    } // the infinite loop related to accept methods ends here
    closesocket(serverSocketDescriptor);
    WSACleanup();
    return;
}