# Online Ping Pong

[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](#license)

This was hackishly created, Run times (Just becuase):
* **Client Runtime:** O(n)
* **Server Runtime:** O(n^2)

Audio From: https://www.bensound.com

This is an online ping pong game written in C for my Graduate Operating Systems course at the University of Texas at San Antonio. No I am not using [lucurious](https://github.com/EasyIP2023/lucurious) or [Vulkan](https://www.khronos.org/vulkan/) as I am still learning Vulkan and still writing lucurious. Plus I read that SDL is a simple to learn and use API.

## Dependencies
* SDL2
* SDL2_image
* SDL2_ttf
* SDL2_mixer

### Online Ping Pong makes full utilization of:
* **SDL2:** For graphics
* **TCP Sockets:** For connection oriented socket streaming. Need to make sure a game is established.
* **Epoll:** To better manage an array of open file descriptors to see which one is ready for read/write operations.
* **Non-Blocking I/0:** All fd's are set into non-blocking mode, so that file descriptors don't have to wait for unix/linux system calls like [read(2)](https://linux.die.net/man/2/read)/[write(2)](https://linux.die.net/man/2/write) to finish.
* **Thread Pool:** Queue's up task for threads to execute. Number of threads is equal to the same number of CPU Cores - 1 (Main process is a thread). Helps a little with context switching.
* **If Statements:** Bool functions final returns are not the same, so if statement checks vary from function to function. :stuck_out_tongue_winking_eye: :sweat_smile:

### Installation/Usage
```
git clone https://github.com/EasyIP2023/online-ping-pong.git
mkdir -v build
meson build
./build/online-ping-pong
```

```
sudo ninja install -C build
online-ping-pong
```

### Running Test
```
ninja test -C build/
```

### References
* [Will Usher SDL2 Tutorial](https://www.willusher.io/pages/sdl2/)
* [TheCplusPlusGuy simple pong game](https://www.youtube.com/watch?v=cf0vWJn9zZc&list=PL949B30C9A609DEE8&index=20)
