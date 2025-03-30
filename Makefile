# Biến lưu trình biên dịch
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Định nghĩa các thư mục chứa mã nguồn
## single threaded
ST_CLIENT_V1_DIR = client/st_client/v1
ST_CLIENT_V2_DIR = client/st_client/v2

ST_SERVER_V1_DIR = server/st_server/v1
ST_SERVER_V2_DIR = server/st_server/v2
## end

# CLIENT_V2_DIR = client/st_server/v2  # Thêm thư mục v2


# Định nghĩa các file nguồn
## single threaded
ST_CLIENT_V1_SRC=$(wildcard $(ST_CLIENT_V1_DIR)/*.c)
ST_CLIENT_V2_SRC=$(wildcard $(ST_CLIENT_V2_DIR)/*.c)

ST_SERVER_V1_SRC=$(wildcard $(ST_SERVER_V1_DIR)/*.c)
ST_SERVER_V2_SRC=$(wildcard $(ST_SERVER_V2_DIR)/*.c)
## end

# CLIENT_SRC = $(wildcard $(CLIENT_DIR)/*.c)
# CLIENT_V2_SRC = $(wildcard $(CLIENT_V2_DIR)/*.c) # Thêm v2
# SERVER_SRC = $(wildcard $(SERVER_DIR)/*.c)

# Định nghĩa các file đối tượng (object files)
## single threaded
ST_CLIENT_V1_OBJ=$(ST_CLIENT_V1_SRC:.c=.o)
ST_CLIENT_V2_OBJ=$(ST_CLIENT_V2_SRC:.c=.o)

ST_SERVER_V1_OBJ=$(ST_SERVER_V1_SRC:.c=.o)
ST_SERVER_V2_OBJ=$(ST_SERVER_V2_SRC:.c=.o)
## end

# CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
# CLIENT_V2_OBJ = $(CLIENT_V2_SRC:.c=.o)
# SERVER_OBJ = $(SERVER_SRC:.c=.o)

# Tên file thực thi
## single threaded
ST_CLIENT_V1_EXEC=$(ST_CLIENT_V1_DIR)/st_client_v1
ST_CLIENT_V2_EXEC=$(ST_CLIENT_V2_DIR)/st_client_v2
ST_SERVER_V1_EXEC=$(ST_SERVER_V1_DIR)/st_server_v1
ST_SERVER_V2_EXEC=$(ST_SERVER_V2_DIR)/st_server_v2
## end
# CLIENT_EXEC = st_client
# CLIENT_V2_EXEC = st_client_v2  # Tên mới cho v2
# SERVER_EXEC = st_server


# Mục tiêu mặc định
all: $(ST_CLIENT_V1_EXEC) $(ST_CLIENT_V2_EXEC) $(ST_SERVER_V1_EXEC) $(ST_SERVER_V2_EXEC)
# all: $(CLIENT_EXEC) $(CLIENT_V2_EXEC) $(SERVER_EXEC)

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

# Quy tắc biên dịch file .c thành .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dọn dẹp file biên dịch
clean:
	rm -f $(ST_CLIENT_V1_OBJ) $(ST_CLIENT_V2_OBJ) $(ST_SERVER_V1_OBJ) $(ST_SERVER_V2_OBJ) $(ST_CLIENT_V1_EXEC) $(ST_CLIENT_V2_EXEC) $(ST_SERVER_V1_EXEC) $(ST_SERVER_V2_EXEC) 
