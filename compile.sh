clang -fsanitize=address $DISABLED -g3 -framework AppKit -framework OpenGl -framework CoreVideo macOS.m -o joyengine && ./joyengine
