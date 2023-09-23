set FilePath=S:\Dev\Projects\VulkanTest\out\build\x64-debug\Shaders
mkdir %FilePath%

dxc.exe -spirv -T vs_6_0 -E main .\Vertex.hlsl -Fo %FilePath%\Vertex.spv
dxc.exe -spirv -T vs_6_0 -E main .\Fragment.hlsl -Fo %FilePath%\Fragment.spv