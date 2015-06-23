                                                                                                                                                                       ⏎ ◼
FROM debian:jessie

ENV DEBIAN_FRONTEND noninteractive

# Install dependencies

RUN apt-get -y update && apt-get -y -q install wget git build-essential autoconf libboost-all-dev libssl-dev libtool libdb++-dev libprotobuf-dev protobuf-compiler libqt4-dev libqrencode-dev bsdmainutils 

# Set working directory
WORKDIR /usr/src

# Fetch, untar and make install BerkeleyDB from source
RUN wget http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz && echo '12edc0df75bf9abd7f82f821795bcee50f42cb2e5f76a6a281b85732798364ef  db-4.8.30.NC.tar.gz' | sha256sum -c 

RUN tar -xvf db-4.8.30.NC.tar.gz && cd db-4.8.30.NC/build_unix && mkdir -p build && BDB_PREFIX=$(pwd)/build && \
../dist/configure --disable-shared --enable-cxx --with-pic --prefix=$BDB_PREFIX && make install

# Clone truthcoin

RUN git clone https://github.com/truthcoin/truthcoin-cpp.git 

RUN apt-get -y install pkg-config

RUN cd /usr/src/truthcoin-cpp/ && ./autogen.sh && ./configure --with-incompatible-bdb && make && make install

ENTRYPOINT ["truthcoind"]
