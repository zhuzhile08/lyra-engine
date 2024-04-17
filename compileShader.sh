/usr/local/bin/glslc LyraEngine/src/Shaders/src/vert.vert -o data/shader/vert.spv
/usr/local/bin/glslc LyraEngine/src/Shaders/src/frag.frag -o data/shader/frag.spv
/usr/local/bin/glslc LyraEngine/src/Shaders/src/vert.vert -o LyraAssets/data/shader/vert.spv
/usr/local/bin/glslc LyraEngine/src/Shaders/src/frag.frag -o LyraAssets/data/shader/frag.spv

/usr/local/bin/glslc LyraEngine/src/Shaders/src/cubemap.vert -o data/shader/cubemapVert.spv
/usr/local/bin/glslc LyraEngine/src/Shaders/src/cubemap.frag -o data/shader/cubemapFrag.spv
/usr/local/bin/glslc LyraEngine/src/Shaders/src/skybox.vert -o data/shader/skyboxVert.spv
/usr/local/bin/glslc LyraEngine/src/Shaders/src/skybox.frag -o data/shader/skyboxFrag.spv
/usr/local/bin/glslc LyraEngine/src/Shaders/src/compute.comp -o data/shader/compute.spv
