CC = g++-14
CCFLAGS = -std=gnu++26 -fmodules-ts -flang-info-module-cmi -Wall -Wpedantic# -fsanitize=address -static-libasan -fno-omit-frame-pointer #-O3
EXECUTABLE = hft.exe

BASIC = ./udp_socket.cpp ./latency.cpp ./order_book.cpp

MODULES = $(BASIC)

all: $(EXECUTABLE)
$(EXECUTABLE): $(MODULES) main.cpp
	$(CC) $(CCFLAGS) $+ -o $@

clean:
	rm -f $(EXECUTABLE) ./build/* ./gcm.cache/* ./compile_commands.json
