# Online Ping Ping

[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](#license)

This was hackishly created. Runtime O(n)

Audio From: https://www.bensound.com

This is an online ping pong game written in C for my Graduate Operating Systems course at the University of Texas at San Antonio. No I am not using [lucurious](https://github.com/EasyIP2023/lucurious) or [Vulkan](https://www.khronos.org/vulkan/) as I am still learning Vulkan and still writing lucurious. Plus I read that SDL is a simple to learn and use API.

## Dependencies
* SDL2
* SDL2_image
* SDL2_ttf
* SDL2_mixer

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
