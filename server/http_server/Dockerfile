FROM ubuntu:20.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    gcc make php-cgi spawn-fcgi nginx \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Tổ chức source code:The following NEW packages will be installed:
#     file libargon2-1 libbsd0 libedit2 libicu66 libmagic-mgc libmagic1 libsodium23 libssl1.1 libxml2 mime-support
#     php-cgi php-common php7.4-cgi php7.4-cli php7.4-common php7.4-json php7.4-opcache php7.4-readline psmisc tzdata
#     ucf xz-utils 
