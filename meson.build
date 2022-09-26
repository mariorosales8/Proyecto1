project('Chat', 'cpp')
catch_lib = subproject('catch2').get_variable('catch2_dep')
json_lib = subproject('jsoncpp').get_variable('jsoncpp_dep')

executable('Servidor', 'Servidor.cpp', dependencies: [json_lib])
executable('Cliente', 'Cliente.cpp', dependencies: [json_lib])

test1 = executable('PruebaJSON', 'TestMensaje.cpp', dependencies: [catch_lib, json_lib])
test('JSON', test1)