FROM alpine:latest

WORKDIR /app

COPY web_server.c /app/
COPY system_info.c /app/
COPY system_info.h /app/

COPY makefile /app/

RUN apk update && apk add --no-cache build-base && make

EXPOSE 8005

CMD ["./web_server"]