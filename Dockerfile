FROM alpine:latest

RUN apk add --update gcc g++ clang gdb cmake make ninja autoconf automake dos2unix tar rsync python3 busybox-extras \
    && rm -rf /tmp/* /var/cache/apk/*

EXPOSE 5678
