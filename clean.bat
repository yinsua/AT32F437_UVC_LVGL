@echo off

:: remove keil_v5 temporary file 
for /d /r %%d in (*listings*) do rd /s /q "%%d"
for /d /r %%d in (*objects*) do rd /s /q "%%d"
del *.uvguix.* /s

:: remove keil_v4 temporary file 
for /d /r %%d in (*lists*) do rd /s /q "%%d"
for /d /r %%d in (*objects*) do rd /s /q "%%d"
del *.uvgui.* /s
del *.dep /s
del *.bak /s

:: remove iar temporary file 
::for /d /r %%d in (*Debug*) do rd /s /q "%%d"
::for /d /r %%d in (*settings*) do rd /s /q "%%d"
::del *.dep /s

::remove scvd
del *.scvd /s

exit
