# hexagon-desk
Epoxy coffe table with 3d printed hexagon inserts and led pcbs with touch sensor

I used 61 "hexagon-pcb" with 61 ttp223 touch modules.

Print files can be found here:
https://www.printables.com/model/200964-hexagon-led-coffee-table

The main-pcb has 4x MCP23017, 1x 1500uF Capacitor, 1x Fuse holder, 1x SN74HC125N and one ESP32 Dev Kit C

Dev Kit link:
https://www.az-delivery.de/en/products/esp32-developmentboard
 

I uploaded the demo program i used to check the feasibility, i soldered the leds and the touch sensors directly to the esp32.


https://youtu.be/I4rWym8PK4s

https://www.youtube.com/watch?v=-OrUfqWGYhM&feature=emb_title


Added a schematic wiring, just for understanding:
[Wiring.pdf](https://github.com/mazls/hexagon-desk/files/8800328/Wiring.pdf)

Everything is wired to the 4 data channels. Every Data-channel has one mcp23017 for the sensors. First 3 wired to 15 hexagons, last is wired to 16 hexagons.
