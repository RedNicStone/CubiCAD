VULKAN_SDK_PATH=${VULKAN_PATH}

echo ${VULKAN_PATH}

$VULKAN_SDK_PATH/bin/glslc quad.vert -o vert_quad.spv
$VULKAN_SDK_PATH/bin/glslc uv.frag -o frag_uv.spv