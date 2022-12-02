# Build and link the launcher.
set(LAUNCHER_SRC launcher.cpp resource.rc)
add_executable(launchbusiness ${LAUNCHER_SRC})
target_link_libraries(launchbusiness)
