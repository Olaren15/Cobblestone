forfiles /s /m *.vert /c "cmd /c %VK_SDK_PATH%\Bin32\glslc.exe @path -o @path.spv"
forfiles /s /m *.frag /c "cmd /c %VK_SDK_PATH%\Bin32\glslc.exe @path -o @path.spv"