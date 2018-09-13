c_cpp_properties.json configures VS Code's language support works.

It needs to mostly reflect how our application code is compiled. (to the extent possible, with one shared set of includes and defines)

The current set came from running a build with gcc -v (to emit the command lines) and then capturing the -I's and -D's that were used to compile led_control.c.
