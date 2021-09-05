elementary_dep = dependency('elementary', required : true)

source_file = [
   'rive_viewer.cpp',
   'runtime_property_change.cpp',
   'animation_mixing.cpp',
   'user_interaction_rollinout.cpp',
   'user_interaction_follow_cursor.cpp',
   'user_interaction_animation_speed.cpp',
   'user_interaction_touch.cpp',
   'rive_tizen_controller.cpp'
]

foreach current_file : source_file
    name = current_file.split('.')[0]
    executable(name, current_file,
        include_directories : headers,
        dependencies : [elementary_dep, rive_cpp_dep, rive_tizen_dep, rive_tizen_renderer_dep],
        link_with: rive_tizen_lib,
        install : true)
endforeach

