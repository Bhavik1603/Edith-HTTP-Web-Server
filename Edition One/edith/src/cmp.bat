g++ -c EDITH.cpp -I ..\include
ar rcs edith.lib *.o
del ..\lib\edith.lib
move edith.lib ..\lib