project('rive_tizen',
        'cpp',
        default_options : ['cpp_std=c++17'],
        version : '0.1.0',
        license : 'MIT')

add_project_arguments('-DRIVE_FILE_DIR="@0@/example/resources/"'.format(meson.current_source_dir()), language : 'cpp')

thorvg_dep = dependency('thorvg', required : false)
if thorvg_dep.found() != true
    thorvg_dep = declare_dependency(include_directories : include_directories('../thorvg/inc'))
    if thorvg_dep.found() != true
        error('ThorVG dependency not found. Looking for ../thorvg')
    endif
endif
if host_machine.system() == 'windows'
    add_project_arguments('-D_USE_MATH_DEFINES', language: 'cpp')
endif

rive_cpp_src = [
   'submodule/src/math/aabb.cpp',
   'submodule/src/math/vec2d.cpp',
   'submodule/src/math/mat2d.cpp',
   'submodule/src/animation/animation_state.cpp',
   'submodule/src/animation/animation_state_instance.cpp',
   'submodule/src/animation/blend_animation.cpp',
   'submodule/src/animation/blend_animation_1d.cpp',
   'submodule/src/animation/blend_animation_direct.cpp',
   'submodule/src/animation/blend_state.cpp',
   'submodule/src/animation/blend_state_1d.cpp',
   'submodule/src/animation/blend_state_1d_instance.cpp',
   'submodule/src/animation/blend_state_direct.cpp',
   'submodule/src/animation/blend_state_direct_instance.cpp',
   'submodule/src/animation/blend_state_transition.cpp',
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
   'submodule/src/animation/state_instance.cpp',
   'submodule/src/animation/state_machine.cpp',
   'submodule/src/animation/state_machine_input.cpp',
   'submodule/src/animation/state_machine_input_instance.cpp',
   'submodule/src/animation/state_machine_instance.cpp',
   'submodule/src/animation/state_machine_layer.cpp',
   'submodule/src/animation/state_transition.cpp',
   'submodule/src/animation/system_state_instance.cpp',
   'submodule/src/animation/transition_bool_condition.cpp',
   'submodule/src/animation/transition_condition.cpp',
   'submodule/src/animation/transition_number_condition.cpp',
   'submodule/src/animation/transition_trigger_condition.cpp',
   'submodule/src/core/binary_reader.cpp',
   'submodule/src/core/field_types/core_bool_type.cpp',
   'submodule/src/core/field_types/core_color_type.cpp',
   'submodule/src/core/field_types/core_double_type.cpp',
   'submodule/src/core/field_types/core_string_type.cpp',
   'submodule/src/core/field_types/core_uint_type.cpp',
   'submodule/src/importers/artboard_importer.cpp',
   'submodule/src/importers/keyed_object_importer.cpp',
   'submodule/src/importers/keyed_property_importer.cpp',
   'submodule/src/importers/layer_state_importer.cpp',
   'submodule/src/importers/linear_animation_importer.cpp',
   'submodule/src/importers/state_machine_importer.cpp',
   'submodule/src/importers/state_machine_layer_importer.cpp',
   'submodule/src/importers/state_transition_importer.cpp',
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
   'submodule/src/generated/artboard_base.cpp',
   'submodule/src/generated/backboard_base.cpp',
   'submodule/src/generated/draw_rules_base.cpp',
   'submodule/src/generated/draw_target_base.cpp',
   'submodule/src/generated/node_base.cpp',
   'submodule/src/generated/animation/animation_base.cpp',
   'submodule/src/generated/animation/linear_animation_base.cpp',
   'submodule/src/generated/animation/animation_state_base.cpp',
   'submodule/src/generated/animation/state_machine_base.cpp',
   'submodule/src/generated/animation/any_state_base.cpp',
   'submodule/src/generated/animation/state_machine_bool_base.cpp',
   'submodule/src/generated/animation/cubic_interpolator_base.cpp',
   'submodule/src/generated/animation/state_machine_layer_base.cpp',
   'submodule/src/generated/animation/entry_state_base.cpp',
   'submodule/src/generated/animation/state_machine_number_base.cpp',
   'submodule/src/generated/animation/exit_state_base.cpp',
   'submodule/src/generated/animation/state_machine_trigger_base.cpp',
   'submodule/src/generated/animation/keyed_object_base.cpp',
   'submodule/src/generated/animation/state_transition_base.cpp',
   'submodule/src/generated/animation/keyed_property_base.cpp',
   'submodule/src/generated/animation/transition_bool_condition_base.cpp',
   'submodule/src/generated/animation/keyframe_color_base.cpp',
   'submodule/src/generated/animation/transition_number_condition_base.cpp',
   'submodule/src/generated/animation/keyframe_double_base.cpp',
   'submodule/src/generated/animation/transition_trigger_condition_base.cpp',
   'submodule/src/generated/animation/keyframe_id_base.cpp',
   'submodule/src/generated/bones/bone_base.cpp',
   'submodule/src/generated/bones/cubic_weight_base.cpp',
   'submodule/src/generated/bones/root_bone_base.cpp',
   'submodule/src/generated/bones/skin_base.cpp',
   'submodule/src/generated/bones/tendon_base.cpp',
   'submodule/src/generated/bones/weight_base.cpp',
   'submodule/src/generated/shapes/clipping_shape_base.cpp',
   'submodule/src/generated/shapes/cubic_asymmetric_vertex_base.cpp',
   'submodule/src/generated/shapes/cubic_detached_vertex_base.cpp',
   'submodule/src/generated/shapes/cubic_mirrored_vertex_base.cpp',
   'submodule/src/generated/shapes/ellipse_base.cpp',
   'submodule/src/generated/shapes/points_path_base.cpp',
   'submodule/src/generated/shapes/polygon_base.cpp',
   'submodule/src/generated/shapes/rectangle_base.cpp',
   'submodule/src/generated/shapes/shape_base.cpp',
   'submodule/src/generated/shapes/star_base.cpp',
   'submodule/src/generated/shapes/straight_vertex_base.cpp',
   'submodule/src/generated/shapes/triangle_base.cpp',
   'submodule/src/generated/shapes/paint/fill_base.cpp',
   'submodule/src/generated/shapes/paint/gradient_stop_base.cpp',
   'submodule/src/generated/shapes/paint/linear_gradient_base.cpp',
   'submodule/src/generated/shapes/paint/radial_gradient_base.cpp',
   'submodule/src/generated/shapes/paint/solid_color_base.cpp',
   'submodule/src/generated/shapes/paint/stroke_base.cpp',
   'submodule/src/generated/shapes/paint/trim_path_base.cpp',
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

if get_option('test') == true
   subdir('test')
endif

