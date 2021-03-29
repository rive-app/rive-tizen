source_file = [
   'rive_viewer.cpp'
]

elementary_dep = dependency('elementary', required : true)

executable('rive_viewer',
           source_file,
			  include_directories: headers,
           dependencies : [elementary_dep, rive_cpp_dep, rive_tizen_dep, rive_tizen_renderer_dep],
			  link_with: rive_tizen_lib,
           install : true)
