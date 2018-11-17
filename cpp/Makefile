pointers:
	g++ pointers.cpp -o pointers.out

arrays:
	g++ arrays.cpp -o arrays.out

arrays_2d:
	g++ array_2d.cpp -o arrays_2d.out

graph-enummap:
	g++ graph/enum_map.cpp -o graph-enummap.out

glfw_cb_context:
	g++ -std=c++11 -g -Wall -Wextra -Wfatal-errors -pedantic \
		-lglfw -lGLEW -lGL -lX11 -lpthread -lXrandr -lXi\
		glfw_cb_context.cpp -o glfw_cb_context.out

list_files:
	g++ -std=c++11 -lboost_system -lboost_filesystem \
		list_files.cpp -o list_files.out

clean:
	rm -f *.out
