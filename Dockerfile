FROM ubuntu:18.04

RUN apt-get update && apt-get install --yes clang \
    python3-dev \
    cmake \
    gcc \
    bash \
    libc-dev \
    python3-pip \
    less \
    wget

RUN pip3 install conan

RUN echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-7 main" > /etc/apt/sources.list.d/llvm.sources.list \
    && echo "deb-src http://apt.llvm.org/bionic/ llvm-toolchain-bionic-7 main" >> /etc/apt/sources.list.d/llvm.sources.list

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN apt-get update && apt-get install --yes llvm-7 clang-7
RUN apt-get install --yes clang-tidy-7
RUN rm /usr/bin/clang && ln -s /usr/bin/clang-7 /usr/bin/clang

WORKDIR /slang

ENV PATH="/usr/bin/cmake/bin:${PATH}"

