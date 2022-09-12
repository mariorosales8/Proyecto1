project('Chat', 'cpp')
catch_lib = subproject('catch2').get_variable('catch2_dep')

executable('Servidor', 'Servidor.cpp')
executable('Cliente', 'Cliente.cpp')

test1 = executable('Prueba', 'Test.cpp', dependencies: [catch_lib])
test('Prueba1', test1)