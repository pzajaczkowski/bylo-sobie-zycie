project_dir := justfile_directory()
build_release_dir := project_dir + "/cmake-build-release"
build_debug_dir := project_dir + "/cmake-build-debug"

default:
    @just --list --justfile {{ justfile() }}

format:
    @find \
      include solutions src \
      \( -name '*.c' \
      -o -name '*.cc' \
      -o -name '*.cpp' \
      -o -name '*.h' \
      -o -name '*.hh' \
      -o -name '*.hpp' \) \
      -exec clang-format -i '{}' \;

build:
    @cmake -B {{ build_debug_dir }} -DCMAKE_BUILD_TYPE=Debug
    @cmake --build {{ build_debug_dir }}

build-openmp:
    @cmake -B {{ build_debug_dir }} \
           -DCMAKE_BUILD_TYPE=Debug \
           -DCMAKE_CXX_FLAGS="-fopenmp" \
           -DCMAKE_C_FLAGS="-fopenmp"
    @cmake --build {{ build_debug_dir }}

release:
    @cmake -B {{ build_release_dir }} -DCMAKE_BUILD_TYPE=Release
    @cmake --build {{ build_release_dir }}

release-openmp:
    @cmake -B {{ build_release_dir }} \
           -DCMAKE_BUILD_TYPE=Release \
           -DCMAKE_CXX_FLAGS="-fopenmp" \
           -DCMAKE_C_FLAGS="-fopenmp"
    @cmake --build {{ build_release_dir }}

clean:
    rm -rf {{ build_release_dir }}
    rm -rf {{ build_debug_dir }}

video images output:
    @ffmpeg -framerate 10 -i {{ images }}/snapshot_%d.pgm -c:v vp8 {{output}}.webm