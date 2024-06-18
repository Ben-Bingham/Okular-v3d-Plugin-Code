remove_definitions(-DTRANSLATION_DOMAIN="okular")
add_definitions(-DTRANSLATION_DOMAIN="okular_v3d" -DGLFW_INCLUDE_VULKAN -DVULKAN_DEBUG=1)

include_directories("src/Rendering" "src/" "src/V3dFile" "dependencies/" "dependencies/VulkanTools" "/usr/include/tirpc" "/home/benjaminb/kde/build/kparts/src/" "/home/benjaminb/kde/build/kparts/")

add_compile_options(-fexceptions)

########### next target ###############
set(okularGenerator_v3d_SRCS src/v3d_generator.cpp src/Rendering/renderheadless.cpp dependencies/VulkanTools/VulkanTools.cpp src/V3dFile/V3dFile.cpp src/V3dFile/V3dObjects.cpp src/V3dFile/V3dObject.cpp src/V3dFile/V3dUtil.cpp)

okular_add_generator(okularGenerator_v3d ${okularGenerator_v3d_SRCS})

target_link_libraries(okularGenerator_v3d okularcore Qt5::Core KF5::I18n GLEW glfw GL vulkan dl pthread X11 Xxf86vm Xrandr Xi tirpc KF5::Parts)

#list( APPEND CMAKE_INSTALL_RPATH "/usr/lib/dependencies" )
set_target_properties(okularGenerator_v3d
        PROPERTIES
        LINK_FLAGS "-Wl,-rpath,/usr/lib:/usr/lib/dependencies"
)

########### install files ###############
install( PROGRAMS okularApplication_v3d.desktop org.kde.mobile.okular_v3d.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES org.kde.okular-v3d.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )

install( FILES v3d-mime.xml PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ DESTINATION "/usr/share/mime/packages" )
# IMPORTANT - need to update mime database
