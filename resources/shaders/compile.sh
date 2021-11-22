VULKAN_SDK_PATH=/home/nic/vulkan/vulkansdk-linux-x86_64-1.2.182.0/1.2.182.0/x86_64

$VULKAN_SDK_PATH/bin/glslc quad.vert -o vert_quad.spv
$VULKAN_SDK_PATH/bin/glslc uv.frag -o frag_uv.spv