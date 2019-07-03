My Main Page
------------
------------

# RaspNet

RaspNet is the Network constructed with several Raspberry Pi.
The RaspNet uses the ISO/OSI layering model depicting the several layers of protocols.
This code has implemented from the Layer 1 to the Layer 3.

## Layers

Here is the [Raspnet](http://osg.informatik.tu-chemnitz.de/lehre/emblab/?lang=en#material) to download the file.

```bash
1. Layer 1 - Physical Layer
  - The lowest layer describes how bits are sent over the wires. There are two wires used from one node to the next.
2. Layer 2 - Data Layer
  - Using the service of layer 1 to send bits, layer 2 bundles them together to send network frames.
  - Data send begins with a preamble of "01111110".
  - The frame is constructed of CRC(32bits), SIZE(8bits) and Payload(2008bits).
3. Layer 3 - Network Layer
  - The third layer handles addressing and routing of packets.
  - The Layer 3-packet is contructed of Destination(8bits), Source(8bits) and Payload(1992bits).
```

## Source Code
* main.c
* init_setup.h
* extra.c
* layer2.h
* layer2.c
* layer3.h
* layer3.c
* user_input.c
* interrupt.h
* interrupt.c

## Usage

```C code
Using Minicom to see the receiving and sending datas.

1. Connect the Physical Line to another Pi
 - Input: PD2(receive data), PD3(receive clock)
 - Output: PB1(send clock), PB2(send data)
2. Open Minicom
3. make run: to compile and flash the source code to Atmega.
4. Input Destination I(0:Broadcast) on the Minicom console
5. Input SIZE of data to send
6. Input data to send(1 or 0)
```

## License
**Jongseo Choi**
