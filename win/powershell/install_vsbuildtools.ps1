mkdir /install -Force
Invoke-WebRequest https://aka.ms/vs/16/release/vs_buildtools.exe  -OutFile /install/vs_buildtools.exe
/install/vs_buildtools.exe --passive --wait --norestart --nocache --installPath C:\BuildTools --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.VC.CMake.Project --add Microsoft.VisualStudio.Component.VC.CoreBuildTools --add Microsoft.VisualStudio.Component.VC.Llvm.Clang --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Workload.MSBuildTools --includeRecommended
while (!(Test-Path "C:/BuildTools/VC/Tools/MSVC/*/bin/Hostx64/x64/cl.exe")){ echo "Waiting for vs_buildtools to complete for a valid cl.exe compiler... Sleeping..."; Start-Sleep 60 }
echo "Windows Build Tools installed!"
echo 'Example command: cmake -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release ../'
