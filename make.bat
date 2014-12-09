@echo off
SET PATH=n:/tools/WinAVR/utils/bin;%PATH%
REM SET PATH=n:\tools\yagarto-20121222\bin;%PATH%
REM SET PATH=n:\tools\msys\bin;%PATH%
REM make.exe -j%NUMBER_OF_PROCESSORS% %1 %2 %3 %4 %5 %6
make.exe %1 %2 %3 %4 %5 %6
REM 

