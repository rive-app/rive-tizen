project('rive_tizen',
        'cpp',
        default_options : ['cpp_std=c++17'],
        version : '0.1.0',
        license : 'MIT')

add_project_arguments('-DRIVE_FILE_DIR="@0@/example/resources/"'.format(meson.current_source_dir()), language : 'cpp')

thorvg_dep = dependency('thorvg', required : true)

rive_cpp_src = [
   'submodule/src/math/aabb.cpp',
   'submodule/src/math/vec2d.cpp',
   'submodule/src/math/mat2d.cpp',
   'submodule/src/animation/cubic_interpolator.cpp',
   'submodule/src/animation/keyed_object.cpp',
   'submodule/src/animation/keyed_property.cpp',
   'submodule/src/animation/keyframe.cpp',
   'submodule/src/animation/keyframe_color.cpp',
   'submodule/src/animation/keyframe_double.cpp',
   'submodule/src/animation/keyframe_id.cpp',
   'submodule/src/animation/linear_animation.cpp',
   'submodule/src/animation/linear_animation_instance.cpp',
   'submodule/src/animation/layer_state.cpp',
   'submodule/src/animation/state_machine_input.cpp',
   'submodule/src/animation/state_machine_layer.cpp',
   'submodule/src/animation/state_machine_trigger.cpp',
   'submodule/src/animation/state_transition.cpp',
   'submodule/src/animation/transition_condition.cpp',
   'submodule/src/core/binary_reader.cpp',
   'submodule/src/core/field_types/core_bool_type.cpp',
   'submodule/src/core/field_types/core_color_type.cpp',
   'submodule/src/core/field_types/core_double_type.cpp',
   'submodule/src/core/field_types/core_string_type.cpp',
   'submodule/src/core/field_types/core_uint_type.cpp',
   'submodule/src/importers/artboard_importer.cpp',
   'submodule/src/importers/keyed_object_importer.cpp',
   'submodule/src/importers/keyed_property_importer.cpp',
   'submodule/src/importers/linear_animation_importer.cpp',
   'submodule/src/shapes/clipping_shape.cpp',
   'submodule/src/shapes/cubic_mirrored_vertex.cpp',
   'submodule/src/shapes/metrics_path.cpp',
   'submodule/src/shapes/path.cpp',
   'submodule/src/shapes/points_path.cpp',
   'submodule/src/shapes/shape.cpp',
   'submodule/src/shapes/straight_vertex.cpp',
   'submodule/src/shapes/cubic_asymmetric_vertex.cpp',
   'submodule/src/shapes/cubic_vertex.cpp',
   'submodule/src/shapes/path_composer.cpp',
   'submodule/src/shapes/polygon.cpp',
   'submodule/src/shapes/shape_paint_container.cpp',
   'submodule/src/shapes/triangle.cpp',
   'submodule/src/shapes/cubic_detached_vertex.cpp',
   'submodule/src/shapes/ellipse.cpp',
   'submodule/src/shapes/parametric_path.cpp',
   'submodule/src/shapes/path_vertex.cpp',
   'submodule/src/shapes/rectangle.cpp',
   'submodule/src/shapes/star.cpp',
   'submodule/src/shapes/paint/color.cpp',
   'submodule/src/shapes/paint/fill.cpp',
   'submodule/src/shapes/paint/gradient_stop.cpp',
   'submodule/src/shapes/paint/linear_gradient.cpp',
   'submodule/src/shapes/paint/radial_gradient.cpp',
   'submodule/src/shapes/paint/shape_paint.cpp',
   'submodule/src/shapes/paint/shape_paint_mutator.cpp',
   'submodule/src/shapes/paint/solid_color.cpp',
   'submodule/src/shapes/paint/stroke.cpp',
   'submodule/src/shapes/paint/trim_path.cpp',
   'submodule/src/bones/bone.cpp',
   'submodule/src/bones/root_bone.cpp',
   'submodule/src/bones/skin.cpp',
   'submodule/src/bones/skinnable.cpp',
   'submodule/src/bones/tendon.cpp',
   'submodule/src/bones/weight.cpp',
   'submodule/src/artboard.cpp',
   'submodule/src/component.cpp',
   'submodule/src/dependency_sorter.cpp',
   'submodule/src/draw_rules.cpp',
   'submodule/src/draw_target.cpp',
   'submodule/src/drawable.cpp',
   'submodule/src/file.cpp',
   'submodule/src/layout.cpp',
   'submodule/src/node.cpp',
   'submodule/src/transform_component.cpp',
]

rive_cpp_dep = declare_dependency(
   include_directories : include_directories('submodule/include'),
   sources : rive_cpp_src,
)

headers = [include_directories('inc'), include_directories('submodule/include'), include_directories('src/renderer')]

subdir('inc')
subdir('src')

if get_option('example') == true
	message('Enable Examples')
	subdir('example')
endif
