FROM gcc:7

# slang build files expect gcc to be named gcc-7
RUN curl https://cmake.org/files/v3.11/cmake-3.11.3-Linux-x86_64.sh -o /tmp/curl-install.sh \
      && chmod u+x /tmp/curl-install.sh \
      && mkdir /usr/bin/cmake \
      && /tmp/curl-install.sh --skip-license --prefix=/usr/bin/cmake \
      && rm /tmp/curl-install.sh \
      && ln -s /usr/local/bin/gcc /usr/local/bin/gcc-7 \
      && ln -s /usr/local/bin/g++ /usr/local/bin/g++-7 \
      && apt-get update \
      && apt-get --yes install zip python-dev \
      && curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py \
      && python get-pip.py \
      && pip install conan

WORKDIR /opt/tools/slang

ENV PATH="/usr/bin/cmake/bin:${PATH}"

COPY runme runme
