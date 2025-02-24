cmake_minimum_required(VERSION 3.19)
project(ChangePlayerNum LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(ChangePlayerNum
    main.cpp
)

target_link_libraries(ChangePlayerNum
    PRIVATE
        Qt::Core
)

include(GNUInstallDirs)

install(TARGETS ChangePlayerNum
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ChangePlayerNum
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
