demo1_source_file = [
   'rive_viewer.cpp'
]

elementary_dep = dependency('elementary', required : true)

executable('rive_viewer',
           demo1_source_file,
			  include_directories: headers,
           dependencies : [elementary_dep, rive_cpp_dep, rive_tizen_dep, rive_tizen_renderer_dep],
			  link_with: rive_tizen_lib,
           install : true)

demo2_source_file = [
   'runtime_property_change.cpp'
]

executable('runtime_property_chnage',
           demo2_source_file,
			  include_directories: headers,
           dependencies : [elementary_dep, rive_cpp_dep, rive_tizen_dep, rive_tizen_renderer_dep],
			  link_with: rive_tizen_lib,
           install : true)

demo3_source_file = [
   'animation_mixing.cpp'
]

executable('animation_mixing',
           demo3_source_file,
			  include_directories: headers,
           dependencies : [elementary_dep, rive_cpp_dep, rive_tizen_dep, rive_tizen_renderer_dep],
			  link_with: rive_tizen_lib,
           install : true)
