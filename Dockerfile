FROM gcc:latest

RUN apt-get update && apt-get install -y cmake

WORKDIR /app

# Copy only source files, not build artifacts
COPY CMakeLists.txt .
COPY include/ include/
COPY src/ src/
COPY examples/ examples/

# Build the compiler
RUN mkdir build && cd build && cmake .. && make

WORKDIR /app/build

CMD ["/bin/bash"]