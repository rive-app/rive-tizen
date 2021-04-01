project('rive_tizen',
        'cpp',
        default_options : ['cpp_std=c++17'],
        version : '0.1.0',
        license : 'MIT')

run_command('script/install.sh')

add_project_arguments('-DRIVE_FILE_DIR="@0@/example/resource/"'.format(meson.current_source_dir()), language : 'cpp')

thorvg_dep = dependency('thorvg', required : true)

install_headers([
   'submodule/include/artboard.hpp',
   'submodule/include/backboard.hpp',
   'submodule/include/command_path.hpp',
   'submodule/include/component.hpp',
   'submodule/include/component_dirt.hpp',
   'submodule/include/container_component.hpp',
   'submodule/include/core.hpp',
   'submodule/include/core_context.hpp',
   'submodule/include/dependency_sorter.hpp',
   'submodule/include/draw_rules.hpp',
   'submodule/include/draw_target.hpp',
   'submodule/include/draw_target_placement.hpp',
   'submodule/include/drawable.hpp',
   'submodule/include/file.hpp',
   'submodule/include/layout.hpp',
   'submodule/include/node.hpp',
   'submodule/include/renderer.hpp',
   'submodule/include/runtime_header.hpp',
   'submodule/include/status_code.hpp',
   'submodule/include/transform_component.hpp',
   'src/renderer/tvg_renderer.hpp'
], subdir: 'rive')

install_headers([
   'submodule/include/animation/animation.hpp',
   'submodule/include/animation/cubic_interpolator.hpp',
   'submodule/include/animation/keyed_object.hpp',
   'submodule/include/animation/keyed_property.hpp',
   'submodule/include/animation/keyframe.hpp',
   'submodule/include/animation/keyframe_color.hpp',
   'submodule/include/animation/keyframe_double.hpp',
   'submodule/include/animation/keyframe_id.hpp',
   'submodule/include/animation/linear_animation.hpp',
   'submodule/include/animation/linear_animation_instance.hpp',
   'submodule/include/animation/loop.hpp'
], subdir: 'rive/animation')

install_headers([
   'submodule/include/bones/bone.hpp',
   'submodule/include/bones/cubic_weight.hpp',
   'submodule/include/bones/root_bone.hpp',
   'submodule/include/bones/skeletal_component.hpp',
   'submodule/include/bones/skin.hpp',
   'submodule/include/bones/skinnable.hpp',
   'submodule/include/bones/tendon.hpp',
   'submodule/include/bones/weight.hpp'
], subdir: 'rive/bones')

install_headers([
   'submodule/include/math/aabb.hpp',
   'submodule/include/math/circle_constant.hpp',
   'submodule/include/math/color.hpp',
   'submodule/include/math/mat2d.hpp',
   'submodule/include/math/transform_components.hpp',
   'submodule/include/math/vec2d.hpp'
], subdir: 'rive/math')

install_headers([
   'submodule/include/core/binary_reader.hpp',
   'submodule/include/core/reader.h'
], subdir: 'rive/core')

install_headers([
   'submodule/include/core/field_types/core_bool_type.hpp',
   'submodule/include/core/field_types/core_color_type.hpp',
   'submodule/include/core/field_types/core_double_type.hpp',
   'submodule/include/core/field_types/core_string_type.hpp',
   'submodule/include/core/field_types/core_uint_type.hpp'
], subdir: 'rive/core/field_types')

install_headers([
   'submodule/include/generated/artboard_base.hpp',
   'submodule/include/generated/backboard_base.hpp',
   'submodule/include/generated/component_base.hpp',
   'submodule/include/generated/container_component_base.hpp',
   'submodule/include/generated/core_registry.hpp',
   'submodule/include/generated/draw_rules_base.hpp',
   'submodule/include/generated/draw_target_base.hpp',
   'submodule/include/generated/drawable_base.hpp',
   'submodule/include/generated/node_base.hpp',
   'submodule/include/generated/transform_component_base.hpp'
], subdir: 'rive/generated')

install_headers([
   'submodule/include/generated/animation/animation_base.hpp',
   'submodule/include/generated/animation/cubic_interpolator_base.hpp',
   'submodule/include/generated/animation/keyed_object_base.hpp',
   'submodule/include/generated/animation/keyed_property_base.hpp',
   'submodule/include/generated/animation/keyframe_base.hpp',
   'submodule/include/generated/animation/keyframe_color_base.hpp',
   'submodule/include/generated/animation/keyframe_double_base.hpp',
   'submodule/include/generated/animation/keyframe_id_base.hpp',
   'submodule/include/generated/animation/linear_animation_base.hpp'
], subdir: 'rive/generated/animation')

install_headers([
   'submodule/include/generated/bones/bone_base.hpp',
   'submodule/include/generated/bones/cubic_weight_base.hpp',
   'submodule/include/generated/bones/root_bone_base.hpp',
   'submodule/include/generated/bones/skeletal_component_base.hpp',
   'submodule/include/generated/bones/skin_base.hpp',
   'submodule/include/generated/bones/tendon_base.hpp',
   'submodule/include/generated/bones/weight_base.hpp'
], subdir: 'rive/generated/bones')

install_headers([
   'submodule/include/generated/shapes/clipping_shape_base.hpp',
   'submodule/include/generated/shapes/cubic_asymmetric_vertex_base.hpp',
   'submodule/include/generated/shapes/cubic_detached_vertex_base.hpp',
   'submodule/include/generated/shapes/cubic_mirrored_vertex_base.hpp',
   'submodule/include/generated/shapes/cubic_vertex_base.hpp',
   'submodule/include/generated/shapes/ellipse_base.hpp',
   'submodule/include/generated/shapes/parametric_path_base.hpp',
   'submodule/include/generated/shapes/path_base.hpp',
   'submodule/include/generated/shapes/path_composer_base.hpp',
   'submodule/include/generated/shapes/path_vertex_base.hpp',
   'submodule/include/generated/shapes/points_path_base.hpp',
   'submodule/include/generated/shapes/polygon_base.hpp',
   'submodule/include/generated/shapes/rectangle_base.hpp',
   'submodule/include/generated/shapes/shape_base.hpp',
   'submodule/include/generated/shapes/star_base.hpp',
   'submodule/include/generated/shapes/straight_vertex_base.hpp',
   'submodule/include/generated/shapes/triangle_base.hpp'
], subdir: 'rive/generated/shapes')

install_headers([
   'submodule/include/generated/shapes/paint/fill_base.hpp',
   'submodule/include/generated/shapes/paint/gradient_stop_base.hpp',
   'submodule/include/generated/shapes/paint/linear_gradient_base.hpp',
   'submodule/include/generated/shapes/paint/radial_gradient_base.hpp',
   'submodule/include/generated/shapes/paint/shape_paint_base.hpp',
   'submodule/include/generated/shapes/paint/solid_color_base.hpp',
   'submodule/include/generated/shapes/paint/stroke_base.hpp',
   'submodule/include/generated/shapes/paint/trim_path_base.hpp',
], subdir: 'rive/generated/shapes/paint')

install_headers([
   'submodule/include/shapes/clipping_shape.hpp',
   'submodule/include/shapes/cubic_asymmetric_vertex.hpp',
   'submodule/include/shapes/cubic_detached_vertex.hpp',
   'submodule/include/shapes/cubic_mirrored_vertex.hpp',
   'submodule/include/shapes/cubic_vertex.hpp',
   'submodule/include/shapes/ellipse.hpp',
   'submodule/include/shapes/metrics_path.hpp',
   'submodule/include/shapes/parametric_path.hpp',
   'submodule/include/shapes/path.hpp',
   'submodule/include/shapes/path_composer.hpp',
   'submodule/include/shapes/path_space.hpp',
   'submodule/include/shapes/path_vertex.hpp',
   'submodule/include/shapes/points_path.hpp',
   'submodule/include/shapes/polygon.hpp',
   'submodule/include/shapes/rectangle.hpp',
   'submodule/include/shapes/shape.hpp',
   'submodule/include/shapes/shape_paint_container.hpp',
   'submodule/include/shapes/star.hpp',
   'submodule/include/shapes/straight_vertex.hpp',
   'submodule/include/shapes/triangle.hpp'
], subdir: 'rive/shapes')

install_headers([
   'submodule/include/shapes/paint/blend_mode.hpp',
   'submodule/include/shapes/paint/color.hpp',
   'submodule/include/shapes/paint/fill.hpp',
   'submodule/include/shapes/paint/gradient_stop.hpp',
   'submodule/include/shapes/paint/linear_gradient.hpp',
   'submodule/include/shapes/paint/radial_gradient.hpp',
   'submodule/include/shapes/paint/shape_paint.hpp',
   'submodule/include/shapes/paint/shape_paint_mutator.hpp',
   'submodule/include/shapes/paint/solid_color.hpp',
   'submodule/include/shapes/paint/stroke.hpp',
   'submodule/include/shapes/paint/stroke_cap.hpp',
   'submodule/include/shapes/paint/stroke_effect.hpp',
   'submodule/include/shapes/paint/stroke_join.hpp',
   'submodule/include/shapes/paint/trim_path.hpp'
], subdir: 'rive/shapes/paint')


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
   'submodule/src/core/binary_reader.cpp',
   'submodule/src/core/field_types/core_bool_type.cpp',
   'submodule/src/core/field_types/core_color_type.cpp',
   'submodule/src/core/field_types/core_double_type.cpp',
   'submodule/src/core/field_types/core_string_type.cpp',
   'submodule/src/core/field_types/core_uint_type.cpp',
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
