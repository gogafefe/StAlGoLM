FROM debian:bookworm-slim AS builder

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        g++ \
        make \
        qmake6 \
        qt6-base-dev \
        qt6-base-dev-tools \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

COPY StAlGoLM.pro ./
COPY main.cpp mytcpserver.cpp server_functions.cpp databasemanager.cpp ./
COPY task1.cpp task2.cpp task3.cpp task4.cpp ./
COPY mytcpserver.h server_functions.h databasemanager.h ./
COPY task1.h task2.h task3.h task4.h ./

RUN qmake6 StAlGoLM.pro CONFIG+=release \
    && make -j"$(nproc)"

FROM debian:bookworm-slim AS runtime

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        libqt6core6 \
        libqt6network6 \
        libqt6sql6 \
        libqt6sql6-sqlite \
        netcat-openbsd \
    && rm -rf /var/lib/apt/lists/* \
    && useradd --system --uid 10001 --home-dir /data stalgolm \
    && mkdir -p /app /data \
    && chown -R stalgolm:stalgolm /data

COPY --from=builder /src/StAlGoLM /app/StAlGoLM

WORKDIR /data
USER stalgolm

EXPOSE 33333

HEALTHCHECK --interval=10s --timeout=3s --start-period=5s --retries=3 \
    CMD nc -z 127.0.0.1 33333 || exit 1

ENTRYPOINT ["/app/StAlGoLM"]
