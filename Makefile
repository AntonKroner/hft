CC = g++-14
CCFLAGS = -std=gnu++26 -fmodules-ts -flang-info-module-cmi -Wall -Wextra -Wpedantic# -fsanitize=address -static-libasan -fno-omit-frame-pointer #-O3
EXECUTABLE = hft.exe

# -fprofile-generate -fprofile-use -flto -fwhole-program -no-rtti

COMMON = ./common/Queue.cpp ./common/Log.cpp ./common/id.cpp ./common/Side.cpp ./common/Price.cpp ./common/Quantity.cpp ./common/Priority.cpp ./common/common.cpp
EXCHANGE = ./exchange/Request.cpp ./exchange/Response.cpp# ./exchange/Exchange.cpp

MODULES = $(COMMON) $(EXCHANGE)

all: $(EXECUTABLE)
$(EXECUTABLE): $(MODULES) main.cpp
	$(CC) $(CCFLAGS) $+ -o $@

clean:
	rm -f $(EXECUTABLE) ./build/* ./gcm.cache/* ./compile_commands.json
