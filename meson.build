project('Chat', 'cpp')
catch_lib = subproject('catch2').get_variable('catch2_dep')
json_lib = subproject('jsoncpp').get_variable('jsoncpp_dep')
cc = meson.get_compiler('cpp')
pthread = cc.find_library('pthread', required: true)

executable('Servidor', 'Servidor.cpp', dependencies: [json_lib, pthread])
executable('Cliente', 'Cliente.cpp', dependencies: [json_lib, pthread])
test1 = executable('PruebaJSON', 'TestMensaje.cpp', dependencies: [catch_lib, json_lib])
test2 = executable('PruebaSala', 'TestSala.cpp', dependencies: [catch_lib])

test('Mensajes', test1)
test('Salas', test2)

run_target('docs', command: 'doxygen.sh')

