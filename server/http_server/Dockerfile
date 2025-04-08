FROM ubuntu:20.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    gcc make php-cgi spawn-fcgi nginx \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app