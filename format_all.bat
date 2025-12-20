@echo off
echo Formatting all C++ files...
for /r %%f in (*.cpp *.h) do (
    echo Formatting %%~nxf
    clang-format -i "%%f"
)
echo Done!