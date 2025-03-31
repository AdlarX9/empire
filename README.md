# Empire

## Lancer le projet

- `$ sfml src/core/main.cpp -o main`
- `$ ./main`
ou encore
- `$ sfml src/core/main.cpp -o main && ./main`

En admettant dans votre `.zshrc` la déclaration `alias sfml='g++ -std=c++20 -L/usr/local/lib -F/Library/Frameworks -framework sfml-graphics -framework sfml-network -framework sfml-audio -framework sfml-window -framework sfml-system -Wl,-rpath,/Library/Frameworks'`.
