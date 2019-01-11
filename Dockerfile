# Wish I could use alpine but could not find clang, llvm and a few others
# so resorting to ubuntu.
FROM ubuntu:18.04

# Documented requirements:
#     pyton3
#     conan
#     cmake 3.10 or later
#     c++17 compatible compiler

# Additional requirements:
#     gcc and clang can be used as c++17 compilers
#     g++-8 brings the #include <filesystem> dependency
#     wget needed by docker to get clang-7
#     bash and curl needed by slang for coverage report publishing

RUN apt-get update && apt-get install --yes clang \
    python3-dev \
    python3-pip \
    cmake \
    gcc \
    g++-8 \
    bash \
    libc-dev \
    less \
    wget \
    curl

RUN pip3 install conan

# clang 7, clang-tidy-7 and llvm 7
RUN echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-7 main" > /etc/apt/sources.list.d/llvm.sources.list \
    && echo "deb-src http://apt.llvm.org/bionic/ llvm-toolchain-bionic-7 main" >> /etc/apt/sources.list.d/llvm.sources.list
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN apt-get update && apt-get install --yes llvm-7 clang-7 clang-tidy-7
RUN rm /usr/bin/clang && ln -s /usr/bin/clang-7 /usr/bin/clang

# The scripts/build_clang.sh script requires this path
WORKDIR /slang

