FROM jjanzic/docker-python3-opencv

RUN apt-get update && apt-get install -y \
  qt4-default \
  libgtk2.0-dev \
  pkg-config

RUN wget http://downloads.sourceforge.net/zbar/0.10/zbar-0.10.tar.bz2 &&\
  tar xf zbar-0.10.tar.bz2 &&\
  cd zbar-0.10 &&\
  export CFLAGS="" &&\
  ./configure --prefix=/usr/local --without-gtk --without-python --disable-video &&\
  make &&\
  make install

COPY . ./app

WORKDIR /app

RUN mkdir build &&\
  cd build &&\
  cmake .. &&\
  make

CMD [ "./build/video-qr-code-scanner" ]
