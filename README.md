# CG-Project

## Change current Phase

 - Go to build directory and inside CMakeCache change CMAKE_HOME_DIRECTORY and Fase*_SOURCE_DIR to current phase

 - Next run the command: 'cmake -U "Unix Makefiles" -B "../build"' inside the directory of current phase

 ## Run program

 - Generator: ./generator <primitive> <measures> <file_name>
 -- <primitive> : - box
                  - plane
                  - cone
                  - sphere
                  - torus

- Engine: ./engine <config_dir> <config>
-- <config_dir> : - tests
                  - configs

