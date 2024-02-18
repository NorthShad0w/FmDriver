set "proProjectpath=%cd%"

cd ../

set "preProjectpath=%cd%"

cd %proProjectpath%

set "SignSysPath=%preProjectpath%/x64/Release/DriverLoad.sys.vmp"

set "SignDlllPath=%preProjectpath%/x64/Release/FmComm.dll.vmp"

"E:\VMProtect\VMProtect_Con.exe" %SignSysPath%

"E:\VMProtect\VMProtect_Con.exe" %SignDlllPath%

"E:\DSignTool\this\CSignTool.exe"  sign /r 22 /f %SignPath% /ac

copy "E:\windowsDriver\FmKernel\FmDriver\x64\Release\DriverLoad.vmp.sys" "D:\FmDriver.sys"

copy "E:\windowsDriver\FmKernel\FmDriver\x64\Release\FmComm.vmp.dll" "D:\Comm.dll"

