module;
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sched.h>
export module udp_socket;
export class udp_socket {
  public:
    int fd = -1;
    std::uint16_t rx_port = 0;
    std::uint16_t tx_port = 0;
    std::uint32_t iface_index = 0;
    // If the caller passes nullptr, an empty string, or the literal "none"
    // we simply *don’t* call SO_BINDTODEVICE – the socket will listen on
    // all interfaces (or on the interface selected by the routing table).
    void open(
      const char* iface,
      std::uint16_t listen_port,
      const char* mcast_addr = nullptr,
      std::uint16_t mcast_port = 0) {
      fd = ::socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
      if (fd < 0) {
        throw std::runtime_error("socket() failed");
      }
      bool bind_dev = iface && iface[0] && std::strcmp(iface, "none");
      if (bind_dev) {
        if (::setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, iface, std::strlen(iface)) < 0) {
          ::close(fd);
          throw std::runtime_error("SO_BINDTODEVICE failed");
        }
      }
      sockaddr_in local{};
      local.sin_family = AF_INET;
      local.sin_addr.s_addr = INADDR_ANY;
      local.sin_port = htons(listen_port);
      if (::bind(fd, reinterpret_cast<sockaddr*>(&local), sizeof(local)) < 0) {
        ::close(fd);
        throw std::runtime_error("bind() failed");
      }
      if (mcast_addr) {
        ip_mreqn mreq{};
        mreq.imr_multiaddr.s_addr = inet_addr(mcast_addr);
        mreq.imr_address.s_addr = INADDR_ANY;
        mreq.imr_ifindex = iface_index;
        if (::setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
          ::close(fd);
          throw std::runtime_error("IP_ADD_MEMBERSHIP failed");
        }
      }
      rx_port = listen_port;
      tx_port = mcast_port;
    }
    void pin_to_core(unsigned core_id) const {
      cpu_set_t mask{};
      CPU_ZERO(&mask);
      CPU_SET(core_id, &mask);
      if (::sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        throw std::runtime_error("sched_setaffinity() failed");
      }
    }
    std::size_t recv(void* buf, std::size_t buf_len) const {
      sockaddr_in src{};
      socklen_t slen = sizeof(src);
      ssize_t rc =
        ::recvfrom(fd, buf, buf_len, 0, reinterpret_cast<sockaddr*>(&src), &slen);
      return rc > 0 ? static_cast<std::size_t>(rc) : 0;
    }
    void send(const void* buf, std::size_t len) const {
      sockaddr_in dst{};
      dst.sin_family = AF_INET;
      dst.sin_port = htons(tx_port);
      dst.sin_addr.s_addr = inet_addr("127.0.0.1");
      ::sendto(fd, buf, len, 0, reinterpret_cast<sockaddr*>(&dst), sizeof(dst));
    }
    void close() {
      if (fd >= 0) {
        ::close(fd);
      }
      fd = -1;
    }
};
