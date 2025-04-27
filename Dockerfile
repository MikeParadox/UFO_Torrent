FROM ubuntu:22.04

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
COPY ./libs ./libs
COPY ./includes ./includes
COPY ./src ./src
COPY ./torrent ./ufo-torrent
COPY ./docs/ufo-torrent.1 ./docs/ufo-torrent.1

# ---------------------------------------------------

RUN cmake -G "Ninja" -B ./build/release -S . -DCMAKE_BUILD_TYPE=Release
RUN export TERMINFO=/usr/share/terminfo
RUN ninja -C build/release
      
# create .deb ----------------------------------------

RUN cp build/release/main ufo-torrent/usr/local/bin/
RUN mv ufo-torrent/usr/local/bin/main ufo-torrent/usr/local/bin/ufo-torrent
RUN mkdir -p ufo-torrent/usr/share/man/man1
RUN gzip -c -k -n docs/ufo-torrent.1 > ufo-torrent/usr/share/man/man1/ufo-torrent.1.gz
RUN chmod +x ufo-torrent/DEBIAN/postinst
RUN dpkg-deb --build ufo-torrent

RUN groupadd -r sample && useradd -r -g sample sample
USER sample

# ----------------------------------------------------

#ENTRYPOINT ["./build/release/main"]
