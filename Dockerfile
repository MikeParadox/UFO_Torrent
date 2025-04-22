FROM ubuntu:latest

WORKDIR /app
        
# -------------------------------------------------

RUN apt-get update 
RUN apt-get install -y g++-14
RUN apt-get install -y gdb
RUN apt-get install -y make
RUN apt-get install -y ninja-build
RUN apt-get install -y rsync
RUN apt-get install -y zip
RUN apt-get install -y cmake   
RUN apt-get install -y build-essential 

# --------------------------------------------------

RUN apt-get install -y libtorrent-rasterbar-dev

RUN apt-get install -y libgtest-dev 

RUN apt-get install -y libcurl4-openssl-dev

RUN apt-get install -y libncurses5-dev
RUN apt-get install -y libncurses-dev
RUN apt-get install -y libncurses6

RUN apt-get install -y libboost-dev
RUN apt-get install -y libboost-locale-dev
RUN apt-get install -y libboost-filesystem-dev

# ---------------------------------------------------

COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMakeUserPresets.json ./CMakeUserPresets.json
COPY ./docs ./docs
COPY ./libs ./libs
COPY ./tests ./tests
COPY ./includes ./includes
COPY ./src ./src

# ---------------------------------------------------

RUN rm -fr ./build/*
    
RUN cmake -G "Ninja" -B ./build/debug -S .
RUN export TERMINFO=/usr/share/terminfo
RUN ninja -C build/debug
      
RUN groupadd -r sample && useradd -r -g sample sample
USER sample
        
CMD ["/app/build/debug/main"]