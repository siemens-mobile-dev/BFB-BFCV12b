@color 7
@set COMn=%1
@if no%1==no set COMn=2
@echo ************************************
@echo          Power Off Phone!
@echo          Close "PapuaKey"?
@echo ************************************
@pause
@color 3
x65srvMode.exe %COMn%
@if errorlevel 1 exit
@x65sendKey.exe %COMn% CLOSE
@if errorlevel 1 exit
@color 7
@echo ************************************
@echo          "PapuaKey" CLOSE!
@echo ************************************
@pause
