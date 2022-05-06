compiler_flags = ['-DRIVE_BUILD']

cc = meson.get_compiler('cpp')
override_options = []
if (cc.get_id() == 'clang-cl')
   override_options += ['cpp_eh=none', 'cpp_rtti=false']
elif (cc.get_id() != 'msvc')
    compiler_flags += ['-fno-exceptions', '-fno-rtti',
                       '-fno-unwind-tables' , '-fno-asynchronous-unwind-tables',
                       '-Woverloaded-virtual', '-Wno-unused-parameter']
endif

rive_tizen_src = [
   'rive_tizen.cpp',
]

rive_tizen_dep = declare_dependency(
   include_directories : include_directories('.'),
   sources : rive_tizen_src
)

subdir('renderer')

rive_tizen_lib = library(
	'rive_tizen',
	include_directories : headers,
	version             : meson.project_version(),
	dependencies        : [rive_cpp_dep, rive_tizen_dep, rive_tizen_renderer_dep],
	install             : true,
	cpp_args            : compiler_flags,
   override_options    : override_options
)

rive_tizen_lib_dep = declare_dependency(
	include_directories : headers,
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
