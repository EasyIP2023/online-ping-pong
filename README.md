# Online Ping Pong

:turtle: [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](#license) :turtle:

Run times (Just because this was hackishly created):
* **Client Runtime:** O(n)
* **Server Runtime:** O(n^2)

Audio From: https://www.bensound.com

**Note: This is more for proof of concept**

Yes I am aware that I could just use UDP Packets with [recvfrom(2)](https://linux.die.net/man/2/recvfrom)/[sendto(2)](https://linux.die.net/man/2/sendto) and have my thread pool watch individual UDP packets, but **this is more for representation** of how epoll, TCP sockets, multiple processes, and a thread pool can be utilized in such a application.

This is an online ping pong game written in C for my Graduate Operating Systems course at the University of Texas at San Antonio. No I am not using [lucurious](https://github.com/EasyIP2023/lucurious) or [Vulkan](https://www.khronos.org/vulkan/) as I am still learning Vulkan and still writing lucurious. Plus I read that SDL is a simple to learn and use API.

## Dependencies
* SDL2
* SDL2_image
* SDL2_ttf
* SDL2_mixer

### Online Ping Pong makes full utilization of
* **SDL2:** For graphics
* **TCP Sockets:** For connection oriented socket streaming. Clients and server read/write exactly 13 bytes of data. Applicaton will read/write player y coordinate data, termination boolean variable, and the ball's x and y coordinate data (the initial player updates balls position).
* **Epoll:** To better manage an array of open file descriptors to see which one is ready for read/write operations.
* **Non-Blocking I/0:** All fd's are set into non-blocking mode, so that file descriptors don't have to wait for unix/linux system calls like [read(2)](https://linux.die.net/man/2/read)/[write(2)](https://linux.die.net/man/2/write) to finish.
* **Thread Pool:** Queue's up task/work for threads to execute. I usually like to make the number of threads equal to the same number of CPU Cores - 1 (Main process is a thread). Helps with resource thrashing don't want to waste time in context switching between threads. Also helps with decreasing thread scheduler overhead. The more threads you have the higher the overhead for the thread scheduler is. You also don't want too many threads blocking becuase of I/O operations, this leaves too many threads idle.
* **If Statements:** Bool functions final returns are not the same, so if statement checks vary from function to function. :stuck_out_tongue_winking_eye: :sweat_smile:

### Installation/Usage
```
git clone https://github.com/EasyIP2023/online-ping-pong.git
mkdir -v build
meson build
ninja -C build
```

```
sudo ninja install -C build
# Server
online-ping-pong --port 8080 --server

# Client
online-ping-pong --ip 127.0.0.1 --port 8080 --client
```

### Running Test
```
ninja test -C build/
```

### References
* [Will Usher SDL2 Tutorial](https://www.willusher.io/pages/sdl2/)
* [TheCplusPlusGuy simple pong game](https://www.youtube.com/watch?v=cf0vWJn9zZc&list=PL949B30C9A609DEE8&index=20)
* [Thread Pools In C](https://nachtimwald.com/2019/04/12/thread-pool-in-c/)
