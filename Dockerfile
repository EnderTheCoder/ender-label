FROM enderthecoder/oatpp-dev:latest
COPY . /app
RUN mkdir /app/build
WORKDIR /app/build
RUN mkdir /data
RUN cmake ..
RUN cmake --build . --config Release -j
WORKDIR /app


CMD ["/app/build/ender-label-exe", "--profile", "production@default"]