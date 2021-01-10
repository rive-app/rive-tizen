project('rive-tizen',
        'cpp',
        version : '0.1.0',
        license : 'MIT')

subdir('lib')

thorvg_dep = dependency('thorvg', required : true)

rive-tizen_lib = library(
	'rive-tizen',
	include_directories : [headers, math_headers, thorvg_headers],
	version             : meson.project_version(),
	dependencies        : [math_dep, rive-tizen_dep, rive-tizen_render_dep, thorvg_dep],
	install             : true,
        gnu_symbol_visibility : 'hidden',
)

rive-tizen_lib_dep = declare_dependency(
	include_directories : [headers, math_headers, thorvg_headers],
        link_with : rive-tizen_lib
)

pkg_mod = import('pkgconfig')

pkg_mod.generate(
        libraries    : rive-tizen_lib,
        version      : meson.project_version(),
        name         : 'librive-tizen',
        filebase     : 'rive-tizen',
        description  : 'A Rive Animation Tizen Runtime Engine'
)

subdir('example')
subdir('assets')

