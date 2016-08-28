@set COMn=%1
@if no%1==no set COMn=2
@if exist key.bin goto sendkey
@echo ********************************
@echo        Power Off Phone!
@echo ********************************
@pause
x55srvMode.exe %COMn% 115200
@if errorlevel 1 exit
GetCode.exe %COMn%
@if errorlevel 1 exit
FFkey.exe
@if errorlevel 1 exit
@:sendkey
x55sendKey.exe %COMn%
@if errorlevel 1 exit
@rem "Siemens EEPROM tool.exe"