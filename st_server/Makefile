# Tên file thực thi (không trùng với thư mục)
SERVER = server/server_v1
CLIENT = client/client_v1

# Trình biên dịch
CC = gcc

# Cờ biên dịch
CFLAGS = -Wall -Wextra -Werror

# Thư mục chứa code
SERVER_DIR = server
CLIENT_DIR = client

# File nguồn
SERVER_SRCS = $(SERVER_DIR)/tcp_server.c $(SERVER_DIR)/socket_utils.c
CLIENT_SRCS = $(CLIENT_DIR)/tcp_client.c $(CLIENT_DIR)/socket_utils.c

# File object
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)

# Quy tắc build chính
all: $(SERVER) $(CLIENT)

# Biên dịch server
$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJS)

# Biên dịch client
$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJS)

# Biên dịch từng file .c thành .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dọn dẹp file biên dịch
clean:
	rm -f $(SERVER) $(CLIENT) $(SERVER_OBJS) $(CLIENT_OBJS)

# Xây dựng lại từ đầu
rebuild: clean all
