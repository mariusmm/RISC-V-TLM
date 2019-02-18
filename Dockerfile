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
      && apt-get clean \
      && rm -rf /var/lib/apt/lists/*
 
RUN mkdir -p /usr/src/systemc \ 
 && wget --no-check-certificate https://accellera.org/images/downloads/standards/systemc/systemc-$SYSTEMC_VERSION.tar.gz \
 &&  tar fzxC systemc-$SYSTEMC_VERSION.tar.gz /usr/src/systemc \
 && cd /usr/src/systemc/systemc-$SYSTEMC_VERSION \
 && mkdir objs \
 && cd objs \
 && export CXX=g++-8 \
 && mkdir -p /usr/local/systemc-$SYSTEMC_VERSION \
 && ../configure --prefix=/usr/local/systemc-$SYSTEMC_VERSION CXXFLAGS="-DSC_CPLUSPLUS=201103L"\
 && make \
 && make install \
 && cd / \
 && rm -rf /usr/src/systemc
 
ENV CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/systemc-$SYSTEMC_VERSION/include
ENV LIBRARY_PATH=$LIBRARY_PATH:/usr/local/systemc-$SYSTEMC_VERSION/lib-linux64
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/systemc-$SYSTEMC_VERSION/lib-linux64
ENV SYSTEMC=/usr/local/systemc-$SYSTEMC_VERSION

RUN mkdir -p /root/.ssh
RUN ssh-keyscan github.com > /root/.ssh/known_hosts



RUN mkdir -p /usr/src/riscv64 \
&& cd /usr/src/riscv64 \
&& git config --global http.sslVerify false \
&& git clone https://github.com/mariusmm/RISC-V-TLM.git \
&& cd RISC-V-TLM \
&& mkdir obj \
&& make


