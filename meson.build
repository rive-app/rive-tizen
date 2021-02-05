project('rive_tizen',
        'cpp',
        version : '0.1.0',
        license : 'MIT')

#thorvg_dep = dependency('thorvg', required : true)

headers = [include_directories('inc')]

rive_tizen_src = [
   'src/rive_tizen.cpp',
]

install_headers([
                 'inc/rive_tizen.hpp',
                ])

rive_tizen_dep = declare_dependency(
   include_directories : include_directories('.'),
   sources : rive_tizen_src
)

rive_src = [
   'submodule/src/math/aabb.cpp',
   'submodule/src/math/vec2d.cpp',
]

rive_dep = declare_dependency(
   include_directories : include_directories(['submodule/include']),
   sources : rive_src
)

rive_tizen_lib = library(
	'rive_tizen',
	include_directories : [headers],
	version             : meson.project_version(),
	dependencies        : [rive_tizen_dep, rive_dep],
	install             : true,
	gnu_symbol_visibility : 'hidden',
)

rive_tizen_lib_dep = declare_dependency(
	include_directories : [headers],
	link_with : rive_tizen_lib
)

pkg_mod = import('pkgconfig')

pkg_mod.generate(
        libraries    : rive_tizen_lib,
        version      : meson.project_version(),
        name         : 'librive_tizen',
        filebase     : 'rive_tizen',
        description  : 'A Rive Animation Tizen Runtime Engine'
)

subdir('example')
