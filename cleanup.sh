#!bin/bash

mexec()
{ 
  export THE_COMMAND=$@
  find . -type d -maxdepth 1 -mindepth 1 -print0 | xargs -0 -I{} zsh -c 'cd "{}" && echo "{}" && echo "$('$THE_COMMAND')" && echo -e'
}

mexec make clean
