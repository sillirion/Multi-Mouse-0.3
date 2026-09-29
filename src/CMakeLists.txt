
	
Update build-windows-lightgun.yml	15 minutes ago

Oui, donc le fichier a bien été poussé sur GitHub il y a 15 minutes.

Le problème probable maintenant : tu as relancé l’ancien run avec Re-run jobs. GitHub réutilise alors la version du workflow associée à cet ancien run, ce qui explique pourquoi Download godot-cpp n’apparaît pas.

Fais ceci :

    Va dans Actions.
    Clique Build Windows Lightgun DLLs à gauche.
    Clique Run workflow à droite.
    Branche main.
    Clique le bouton vert Run workflow.

⚠️ Ne clique pas sur Re-run jobs de l'ancien échec.

Dans le nouveau run, on doit voir :

Checkout Multi-Mouse → Download godot-cpp → Setup Python → Install SCons → Build template_debug

Si Download godot-cpp apparaît, on aura enfin lancé la nouvelle version.
svg                                                                                                                  
9s
Run ./scripts/build_windows.ps1 -Target template_release -Arch x86_64      
Building godot-cpp (template_release)...      
scons: Reading SConscript files ...      
WARNING: Unknown SCons variables were passed and will be ignored:      
    bits=64      
Building for architecture x86_64 on platform windows      
scons: *** 'api_version' must be provided      
File "D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\extern\godot-cpp\tools\godotcpp.py", line 557, in _get_api_file      
Write-Error: D:\a\_temp\4057a2e3-808f-4720-b750-5dcbb9d937e0.ps1:2      
Line |      
   2 |  ./scripts/build_windows.ps1 -Target template_release -Arch x86_64      
     |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~      
     | Expected      
     | D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\extern\godot-cpp\bin\libgodot-cpp.windows.template_release.x86_64.lib but      
     | it was not produced.      
Error: Process completed with exit code 1.
…
Réfléchi pendant 4s

Bien, on a progressé : godot-cpp est maintenant téléchargé et la compilation démarre.

La nouvelle erreur est claire : la version actuelle de godot-cpp exige api_version, alors que ton script lance SCons sans ce paramètre. Ton script utilise aussi l'ancien paramètre bits=64.

J’ai corrigé uniquement build_windows.ps1 pour utiliser Godot 4.7 et arch=x86_64 :

Maintenant :

    Renomme-le en build_windows.ps1.
    Remplace celui dans scripts.
    GitHub Desktop → Commit to main → Push origin.
    GitHub → Actions → nouveau Run workflow. Ne fais pas Re-run jobs.

On regarde ensuite la prochaine compilation.
build_windows_fixed.ps1Code
This warning is for project developers.  Use -Wno-dev to suppress it.      
-- Configuring done (10.4s)      
-- Generating done (0.0s)      
-- Build files have been written to: D:/a/Multi-Mouse-0.3/Multi-Mouse-0.3/build/windows-template_debug      
Building...      
MSBuild version 17.14.60+43b635718 for .NET Framework      
  1>Checking Build System      
  Building Custom Rule D:/a/Multi-Mouse-0.3/Multi-Mouse-0.3/src/CMakeLists.txt      
  register_types.cpp      
  multi_mouse_server.cpp      
D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\extern\godot-cpp\include\godot_cpp\core\defs.hpp(41,27): error C2338: static_assert failed: 'Minimum of C++17 required.' [D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\build\windows-template_debug\multi_mouse.vcxproj]      
  (compiling source file '../../src/register_types.cpp')      
D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\extern\godot-cpp\include\godot_cpp\core\defs.hpp(41,27): error C2338: static_assert failed: 'Minimum of C++17 required.' [D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\build\windows-template_debug\multi_mouse.vcxproj]      
  (compiling source file '../../src/multi_mouse_server.cpp')      
  multi_mouse_backend.cpp      
D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\extern\godot-cpp\include\godot_cpp\core\defs.hpp(41,27): error C2338: static_assert failed: 'Minimum of C++17 required.' [D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\build\windows-template_debug\multi_mouse.vcxproj]      
  (compiling source file '../../src/multi_mouse_backend.cpp')      
  multi_mouse_backend_windows.cpp      
D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\extern\godot-cpp\include\godot_cpp\core\defs.hpp(41,27): error C2338: static_assert failed: 'Minimum of C++17 required.' [D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\build\windows-template_debug\multi_mouse.vcxproj]      
  (compiling source file '../../src/platform/windows/multi_mouse_backend_windows.cpp')      
  Generating Code...      
Copy-Item: D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\scripts\build_windows.ps1:54      
Line |      
  54 |  Copy-Item $OutputDll $DestDll -Force      
     |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~      
     | Cannot find path 'D:\a\Multi-Mouse-0.3\Multi-Mouse-0.3\build\windows-template_debug\multi_mouse.dll' because it      
     | does not exist.      
Error: Process completed with exit code 1.
…

Bien : godot-cpp compile maintenant. Le blocage suivant est simplement que le projet Multi-Mouse est compilé avec une norme C++ trop ancienne. godot-cpp exige au minimum C++17.

Il faut donc corriger src/CMakeLists.txt, pas build_windows.ps1.

Envoie-moi ton fichier :

src/CMakeLists.txt

Je te le corrige directement pour C++17, sans toucher au reste.
CMakeLists.txtDocument

J’ai trouvé le détail : ton CMakeLists.txt demandait déjà C++17.

Le problème vient de MSVC : il faut aussi lui passer /Zc:__cplusplus pour que godot-cpp détecte correctement C++17. J’ai ajouté cette option dans le bloc MSVC existant.

Fais simplement :

    Renomme-le CMakeLists.txt.
    Remplace src/CMakeLists.txt.
    GitHub Desktop → Commit → Push origin.
    GitHub Actions → nouveau Run workflow.

On ne touche à rien d’autre pour ce test.
CMakeLists_fixed.txtDocument


cmake_minimum_required(VERSION 3.20)
project(multi_mouse LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED GODOT_CPP_PATH)
    set(GODOT_CPP_PATH "${CMAKE_SOURCE_DIR}/../extern/godot-cpp" CACHE PATH "Path to godot-cpp checkout" FORCE)
endif()

if(NOT EXISTS "${GODOT_CPP_PATH}/include")
    message(FATAL_ERROR "godot-cpp headers not found in ${GODOT_CPP_PATH}. Did you init the submodule?")
endif()

set(GODOT_CPP_LIB "" CACHE FILEPATH "Path to the prebuilt godot-cpp static library")
if(NOT GODOT_CPP_LIB)
    message(FATAL_ERROR "Set GODOT_CPP_LIB to the built godot-cpp library (e.g. extern/godot-cpp/bin/libgodot-cpp.linux.template_debug.x86_64.a)")
endif()

set(PLATFORM_SOURCES)
if(WIN32)
    list(APPEND PLATFORM_SOURCES
        platform/windows/multi_mouse_backend_windows.cpp
    )
elseif(UNIX AND NOT APPLE)
    list(APPEND PLATFORM_SOURCES
        platform/linux/multi_mouse_backend_linux.cpp
        ../extern/manymouse/manymouse.c
        ../extern/manymouse/linux_evdev.c
        ../extern/manymouse/x11_xinput2.c
        ../extern/manymouse/manymouse_platform_stubs.c
    )
endif()

add_library(multi_mouse SHARED
    register_types.cpp
    multi_mouse_server.cpp
    multi_mouse_backend.cpp
    ${PLATFORM_SOURCES}
)

target_include_directories(multi_mouse PRIVATE
    ${GODOT_CPP_PATH}/include
    ${GODOT_CPP_PATH}/include/godot_cpp
    ${GODOT_CPP_PATH}/gen/include
    ${GODOT_CPP_PATH}/gen/include/godot_cpp
    ${GODOT_CPP_PATH}/gdextension
    ${CMAKE_SOURCE_DIR}
)

if(UNIX AND NOT APPLE)
    target_include_directories(multi_mouse PRIVATE ${CMAKE_SOURCE_DIR}/../extern/manymouse)
endif()

target_link_libraries(multi_mouse PRIVATE ${GODOT_CPP_LIB})

if(WIN32)
    target_link_libraries(multi_mouse PRIVATE user32)
elseif(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(multi_mouse PRIVATE Threads::Threads dl)
    find_package(X11 REQUIRED)
    if(X11_FOUND)
        target_include_directories(multi_mouse PRIVATE ${X11_INCLUDE_DIR})
        target_link_libraries(multi_mouse PRIVATE ${X11_LIBRARIES})
        if(X11_Xi_LIB)
            target_link_libraries(multi_mouse PRIVATE ${X11_Xi_LIB})
        endif()
    endif()
endif()

if(MSVC)
    target_compile_definitions(multi_mouse PRIVATE TYPED_METHOD_BIND NOMINMAX)
    # MSVC needs /Zc:__cplusplus so godot-cpp can detect the selected C++17 standard.
    target_compile_options(multi_mouse PRIVATE /Zc:__cplusplus)
    set_property(TARGET multi_mouse PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

install(TARGETS multi_mouse DESTINATION ${CMAKE_SOURCE_DIR}/../addons/multi_mouse/bin)
