FROM ubuntu:20.04
LABEL maintainer="Ben V. Brown <ralim@ralimtek.com>"

WORKDIR /build
# Add extra mirrors for options
RUN echo "deb mirror://mirrors.ubuntu.com/mirrors.txt focal main restricted universe multiverse" > /etc/apt/sources.list && \
  echo "deb mirror://mirrors.ubuntu.com/mirrors.txt focal-updates main restricted universe multiverse" >> /etc/apt/sources.list && \
  echo "deb mirror://mirrors.ubuntu.com/mirrors.txt focal-security main restricted universe multiverse" >> /etc/apt/sources.list && \
  DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y ca-certificates 
# Install dependencies to build the firmware
RUN apt-get install -y \
  make \
  bzip2 \
  git \
  python3 \
  python3-pip \
  clang-format \
  wget --no-install-recommends && \
  apt-get clean
RUN python3 -m pip install bdflib
# Download the two compilers
RUN wget -qO- "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2" | tar -xj
# This is the same as the one from Nuclei, just mirrored on Github as their download server is horrifically slow
RUN wget -qO- "https://github.com/Ralim/nuclei-compiler/releases/download/2020.08/nuclei_riscv_newlibc_prebuilt_linux64_2020.08.tar.bz2" | tar -xj

# Add compiler to the path
ENV PATH "/build/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH"
ENV PATH "/build/gcc/bin/:$PATH"
COPY . /build/source
COPY ./ci /build/ci
