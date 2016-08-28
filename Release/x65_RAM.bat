@echo "All RAM x65"
@set COMn=2
@set MAXBAUD=115200
BFXReader.exe %COMn% 0x00000000 0x00004000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0x00080000 0x00010000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA8000000 0x00100000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA8100000 0x00100000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA8200000 0x00100000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA8300000 0x00100000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA8400000 0x00100000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA8500000 0x00100000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA8600000 0x00100000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA8700000 0x00100000 %MAXBAUD%