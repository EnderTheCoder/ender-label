FROM enderthecoder/oatpp-dev:latest
COPY . /app
RUN mkdir /app/build
WORKDIR /app/build
RUN cmake ..
RUN cmake --build . --config Release -j
WORKDIR /app


ENTRYPOINT ["/app/build/ender-label-exe"]