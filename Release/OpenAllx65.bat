@color 7
@set COMn=%1
@if no%1==no set COMn=2
@echo ************************************
@echo          Power Off Phone!
@if not exist key.bin echo  Calk and Send "PapuaKey" to x65 ?
@if exist key.bin echo  Send Old Saved "PapuaKey" to x65 ?
@echo ************************************
@pause
@color 3
x65srvMode.exe %COMn%
@if errorlevel 1 exit
@if exist key.bin goto sendkey
x65GetCode.exe %COMn%
@if errorlevel 1 exit
FFkey.exe
@if errorlevel 1 exit
@:sendkey
@x65sendKey.exe %COMn%
@if errorlevel 1 exit
@color 7
@echo ************************************
@echo          Send "PapuaKey" Ok!
@echo         For close key - Use:
@echo        x65sendKey.exe %COMn% CLOSE
@echo ************************************
@pause
