# Biến lưu trình biên dịch
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -O2
# Thêm các flag cho multi-threaded server và client
MT_LDFLAGS = -pthread
# MT_LDFLAGS = -pthread -lssh
HTTPS_LDFLAGS = -lpthread -lssl -lcrypto

# Định nghĩa các thư mục chứa mã nguồn
## single threaded
ST_CLIENT_V1_DIR = client/st_client/v1
ST_CLIENT_V2_DIR = client/st_client/v2

ST_SERVER_V1_DIR = server/st_server/v1
ST_SERVER_V2_DIR = server/st_server/v2
## end

## multi threaded
MT_CLIENT_DIR=client/mt_client
MT_SERVER_DIR=server/mt_server
## end

## http server
HTTP_SERVER_DIR=server/http_server
## end

## https server
HTTPS_SERVER_DIR=server/https_server
## end

# CLIENT_V2_DIR = client/st_server/v2  # Thêm thư mục v2

# Định nghĩa các file nguồn
## single threaded
ST_CLIENT_V1_SRC=$(wildcard $(ST_CLIENT_V1_DIR)/*.c)
ST_CLIENT_V2_SRC=$(wildcard $(ST_CLIENT_V2_DIR)/*.c)

ST_SERVER_V1_SRC=$(wildcard $(ST_SERVER_V1_DIR)/*.c)
ST_SERVER_V2_SRC=$(wildcard $(ST_SERVER_V2_DIR)/*.c)
## end

## multi threaded
MT_CLIENT_SRC=$(wildcard $(MT_CLIENT_DIR)/*.c)
MT_SERVER_SRC=$(wildcard $(MT_SERVER_DIR)/*.c)
## end

## http server
HTTP_SERVER_SRC=$(wildcard $(HTTP_SERVER_DIR)/src/*.c)
## end

## https server
HTTPS_SERVER_SRC=$(wildcard $(HTTPS_SERVER_DIR)/src/*.c)
## end

# Định nghĩa các file đối tượng (object files)
## single threaded
ST_CLIENT_V1_OBJ=$(ST_CLIENT_V1_SRC:.c=.o)
ST_CLIENT_V2_OBJ=$(ST_CLIENT_V2_SRC:.c=.o)

ST_SERVER_V1_OBJ=$(ST_SERVER_V1_SRC:.c=.o)
ST_SERVER_V2_OBJ=$(ST_SERVER_V2_SRC:.c=.o)
## end

## multi threaded
MT_CLIENT_OBJ=$(MT_CLIENT_SRC:.c=.o)
MT_SERVER_OBJ=$(MT_SERVER_SRC:.c=.o)
## end

## http server
HTTP_SERVER_OBJ=$(HTTP_SERVER_SRC:.c=.o)
## end

## https server
HTTPS_SERVER_OBJ=$(HTTPS_SERVER_SRC:.c=.o)
## end

# Tên file thực thi
## single threaded
ST_CLIENT_V1_EXEC=$(ST_CLIENT_V1_DIR)/st_client_v1
ST_CLIENT_V2_EXEC=$(ST_CLIENT_V2_DIR)/st_client_v2
ST_SERVER_V1_EXEC=$(ST_SERVER_V1_DIR)/st_server_v1
ST_SERVER_V2_EXEC=$(ST_SERVER_V2_DIR)/st_server_v2
## end

## multi threaded
MT_CLIENT_EXEC=$(MT_CLIENT_DIR)/mt_client
MT_SERVER_EXEC=$(MT_SERVER_DIR)/mt_server
## end

## http server
HTTP_SERVER_EXEC=$(HTTP_SERVER_DIR)/http_server
## end

## https server
HTTPS_SERVER_EXEC=$(HTTPS_SERVER_DIR)/https_server
## end

# Mục tiêu mặc định
all: $(ST_CLIENT_V1_EXEC) $(ST_CLIENT_V2_EXEC) $(ST_SERVER_V1_EXEC) $(ST_SERVER_V2_EXEC) $(MT_CLIENT_EXEC) $(MT_SERVER_EXEC) $(HTTP_SERVER_EXEC) $(HTTPS_SERVER_EXEC)

## single threaded
# Biên dịch chương trình client
$(ST_CLIENT_V1_EXEC): $(ST_CLIENT_V1_OBJ)
	$(CC) $(CFLAGS) -o $@ $^
$(ST_CLIENT_V2_EXEC): $(ST_CLIENT_V2_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Biên dịch chương trình server
$(ST_SERVER_V1_EXEC): $(ST_SERVER_V1_OBJ)
	$(CC) $(CFLAGS) -o $@ $^
$(ST_SERVER_V2_EXEC): $(ST_SERVER_V2_OBJ)
	$(CC) $(CFLAGS) -o $@ $^
## end

## multi threaded
# Biên dịch chương trình client
$(MT_CLIENT_EXEC): $(MT_CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(MT_LDFLAGS)

# Biên dịch chương trình server
$(MT_SERVER_EXEC): $(MT_SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(MT_LDFLAGS)
## end

## http server
$(HTTP_SERVER_EXEC): $(HTTP_SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(MT_LDFLAGS)
## end

## https server
$(HTTPS_SERVER_EXEC): $(HTTPS_SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(HTTPS_LDFLAGS)
## end

# Quy tắc biên dịch file .c thành .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dọn dẹp file biên dịch
clean:
	rm -f $(ST_CLIENT_V1_OBJ) $(ST_CLIENT_V2_OBJ) $(ST_SERVER_V1_OBJ) $(ST_SERVER_V2_OBJ) $(ST_CLIENT_V1_EXEC) $(ST_CLIENT_V2_EXEC) $(ST_SERVER_V1_EXEC) $(ST_SERVER_V2_EXEC) $(MT_CLIENT_OBJ) $(MT_SERVER_OBJ) $(MT_CLIENT_EXEC) $(MT_SERVER_EXEC) $(HTTP_SERVER_EXEC) $(HTTP_SERVER_OBJ) $(HTTPS_SERVER_EXEC) $(HTTPS_SERVER_OBJ)
