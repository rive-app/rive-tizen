source_file = [
   'rive_viewer.cpp'
]

elementary_dep = dependency('elementary', required : true)

executable('rive_viewer',
           source_file,
           dependencies : [rive_tizen_lib_dep, thorvg_renderer_dep, rive_dep, elementary_dep],
           install : true)
