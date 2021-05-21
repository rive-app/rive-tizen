
override_default = ['werror=false']

gtest_dep  = dependency('gtest')

controller_test_sources = [
    'testsuite.cpp',
    'test_controller.cpp',
    ]

rive_tizen_controller_testsuite = executable('ControllerTestSuite',
                                              controller_test_sources,
                                              include_directories : headers,
                                              override_options : override_default,
                                              dependencies : [gtest_dep, thorvg_dep, rive_cpp_dep, rive_tizen_dep, rive_tizen_renderer_dep],
                                              )

test('Controller Testsuite', rive_tizen_controller_testsuite)

