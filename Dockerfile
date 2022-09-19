FROM ubuntu:bionic
LABEL maintainer="Màrius Montón"
ENV SYSTEMC_VERSION 2.3.3

RUN apt-get update -q && apt-get install -qy gcc-riscv64-linux-gnu

RUN apt-get update -q && apt-get install -qy --no-install-recommends \
      build-essential curl \
      cmake \
      git \
      openssh-client \
      wget \
      g++-8 \
      xterm \
      libboost-all-dev libspdlog-dev \
      && apt-get clean \
      && rm -rf /var/lib/apt/lists/*
 
RUN mkdir -p /usr/src/riscv64 \ 
 && wget --no-check-certificate https://accellera.org/images/downloads/standards/systemc/systemc-$SYSTEMC_VERSION.tar.gz \
 && tar fzxC systemc-$SYSTEMC_VERSION.tar.gz /usr/src/riscv64 \
 && cd /usr/src/riscv64/systemc-$SYSTEMC_VERSION \
 && mkdir build \
 && cd build \
 && cmake .. -DCMAKE_CXX_STANDARD=17 \
 && make \
 && make install \
 && cd / 
 
#RUN rm -r /root/.ssh
RUN mkdir -p /root/.ssh
RUN ssh-keyscan github.com > /root/.ssh/known_hosts
RUN git config --global http.sslBackend "openssl"

RUN cd /usr/src/riscv64 \
 && git config --global http.sslVerify false \
 && git clone https://github.com/gabime/spdlog.git \
 && cd spdlog \
 && cmake -H. -B_builds -DCMAKE_BUILD_TYPE=Release \
 && cmake --build _builds --config Release \
 && cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Release \
 && cmake --build _builds --target install 

RUN cd /usr/src/riscv64 \
 && chmod -R a+rw . \
 && git config --global http.sslVerify false \
 && git clone https://github.com/mariusmm/RISC-V-TLM.git \
 && cd RISC-V-TLM \
 && mkdir build \
 && cd build \ 
 && chmod a+rw . \
 && cmake .. \
 && make

WORKDIR /usr/src/riscv64
