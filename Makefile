CC = g++-16
CCFLAGS = -std=gnu++26 -fmodules-ts -flang-info-module-cmi -Wall -Wextra -Wpedantic
# CCFLAGS += -fsanitize=address -static-libasan -fno-omit-frame-pointer
# CCFLAGS += -O3 -fprofile-generate -fprofile-use -flto -fwhole-program -no-rtti
EXECUTABLE = exchange.exe
COMMON = common/id.cpp common/Side.cpp common/Price.cpp common/Quantity.cpp common/Priority.cpp common/Queue.cpp common/Pool.cpp common/Log.cpp common/common.cpp
EXCHANGE = exchange/Request.cpp exchange/Response.cpp exchange/MarketUpdate.cpp exchange/Order.cpp exchange/MatchingEngine.cpp
SOURCES = $(COMMON) $(EXCHANGE) exchange/main.cpp
all: $(EXECUTABLE)
$(EXECUTABLE): $(SOURCES)
	$(CC) $(CCFLAGS) $^ -o $@
clean:
	rm -f $(EXECUTABLE) ./build/* ./gcm.cache/* ./compile_commands.json
