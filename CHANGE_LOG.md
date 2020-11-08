# ToDos
- Dynamic lod creator for lod sample
- Forward+
- DirectX 12
- Realtime Raytracing
- DEBUG, RELEASE, MinSizeRelease(does not have assimp and just use wscene files)

# 2.0.0.0
A major release, rewrite most of wolf.system with pure C
 - Build for Win64
 - Build for OSX
 - Build for iOS
 - Build for Android-armv7
 - Build for Linux64


# 1.68.0.9 (2018-10-03)
A minor release with many compatibility-breaking changes.

New features:
- CMAKE added for building wolf for linux platform

# 1.65.0.0 (2018-06-04)
A minor release with many compatibility-breaking changes.

New features:
- system::w_logger optimized and integrated with spdlog 
- framework::w_media_core optimized for streaming
- gpu occlusion culling has been added


# 1.63.1.0 (2018-04-19)

A minor release with many compatibility-breaking changes.

New features:
- Integrated with Vulkan SDK version 1.1.73.0
- Integrated with VulkanMemoryAllocator for better gpu memory managment
- The new coordinate system is Left handed Y-Up 
- The function "contains" have been added to wolf::system::w_bounding_sphere
