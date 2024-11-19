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
    @cmake -DCMAKE_BUILD_TYPE=Debug \
           -DUSE_OPENMP=ON \
           -DCMAKE_CXX_FLAGS="-fopenmp" \
           -B {{ build_debug_dir }}
    @cmake --build {{ build_debug_dir }}

release:
    @cmake -B {{ build_release_dir }} -DCMAKE_BUILD_TYPE=Release
    @cmake --build {{ build_release_dir }}

release-openmp:
    @cmake -DCMAKE_BUILD_TYPE=Release \
           -DUSE_OPENMP=ON \
           -DCMAKE_CXX_FLAGS="-fopenmp" \
           -B {{ build_release_dir }}
    @cmake --build {{ build_release_dir }}

clean:
    rm -rf {{ build_release_dir }}
    rm -rf {{ build_debug_dir }}

video images output:
    @ffmpeg -framerate 10 -i {{ images }}/snapshot_%d.pgm -c:v vp8 {{output}}.webm