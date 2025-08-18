# md_generator.py
import socket, struct, time, random
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
dst = ('127.0.0.1', 9000)

price = 10000   # tick size = 1
while True:
  side = random.choice([0, 1])
  delta = random.randint(-5, 5)
  price = max(1, price + delta)
  size = random.choice([1, 2, 3])
  pkt = struct.pack('!Bq i', side, price, size)
  sock.sendto(pkt, dst)
  time.sleep(0.0001)   # ~10 k msgs/s per core
   