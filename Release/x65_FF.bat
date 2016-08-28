@echo "All FF x65"
@set COMn=2
@set MAXBAUD=115200
BFXReader.exe %COMn% 0xA0000000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA0200000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA0400000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA0600000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA0800000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA0A00000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA0C00000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA0E00000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA1000000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA1200000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA1400000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA1600000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA1800000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA1A00000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA1C00000 0x00200000 %MAXBAUD%
@if errorlevel 1 exit
BFXReader.exe %COMn% 0xA1E00000 0x00200000 %MAXBAUD%