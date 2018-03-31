clang -fsanitize=address $DISABLED -g3 -framework AppKit -framework OpenGl -framework CoreVideo MacOS.mm -o joyengine && ./joyengine
