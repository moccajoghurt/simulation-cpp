@ECHO OFF
g++ -std=c++11 %1 -IC:\opencv\build\include -LC:\opencv\build\x86\mingw\lib -lopencv_core245 -lopencv_highgui245 -lopencv_imgproc245
