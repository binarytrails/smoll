#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

// Given two types
struct Foo { void foo() { std::cout << "foo" << std::endl; } };
struct Bar { void bar() { std::cout << "bar" << std::endl; } };

// A callback context:
struct callback_context {
  Foo foo;
  Bar bar;
};

// And a function to get the context from a window:
callback_context* get_context(GLFWwindow* w) { 
  return static_cast<callback_context*>(glfwGetWindowUserPointer(w)); 
}

int main() {
  // Construct a callback context, get window, and set user pointer:
  callback_context cbc;
  
  glfwInit();
  auto *window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
  glfwSetWindowUserPointer(window, &cbc);

  // Define some callback, get the context, and call methods directly: 
  auto wcc = [](GLFWwindow* w) {
    callback_context* cbc_ptr = get_context(w);
    cbc_ptr->foo.foo(); 
    cbc_ptr->bar.bar();
  };

  // Set the callback and enjoy: 
  glfwSetWindowCloseCallback(window, wcc);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();  
    // on window close "foo\nbar\n" is printed to stdout.
  }
  // destroy windows and close
  return 0;
}
