# Reflow Oven
###### June 2017

[//]: # (Image References)

[image1]: ./Reflow_Oven/images/16_2_LCD.jpg
[image2]: ./Reflow_Oven/images/relay_wiring.jpg
[image3]: ./Reflow_Oven/images/arduino_wiring.jpg
[image4]: ./Reflow_Oven/images/bloom_faceplate.jpg
[image5]: ./Reflow_Oven/images/Kester_EP256_Reflow_Profile.png
[image6]: ./Reflow_Oven/images/preheating.jpg
[image7]: ./Reflow_Oven/images/CircuitBoard.jpg
[image8]: ./Reflow_Oven/images/schematic.png
[image9]: ./Reflow_Oven/images/uno_front_2.jpg

![alt text][image6]
The goal of this project is to control a two-element toaster oven for reflowing printed circuit boards with surface mount devices.

Before we begin, credit is due to Whizoo.com for his walkthrough of the oven build. If you'd like a prebuilt oven controller, check out the ControlLeo on his site. 

But there's so much more to learn by building it yourself! Check out his guide that I referenced while building mine: Whizoo.com/reflowoven

Let's begin.
### Parts List:
1. Toaster Oven
2. K-type thermocouple
3. MAX6675 or similar(for k type thermocouple)
4. Arduino Uno/Nano/Pro Mini or other similar MCU
5. 16-2 LCD screen
6. Momentary Push Button
7. SPST Switch
8. 2 Solid State Relays (rated for your voltage and amperage - I used 25A, 380VAC/32VDC)
9. 2 3mm LEDs (5mm will work also). I used Blue/Red leds with 3 leads.
10. Heat tape

I used a generic black and decker toaster much like the one below. This toaster already has two heating elements(top and a bottom).



![](http://www.whizoo.com/i/c/l/b02.jpg)



I used a k-type thermocouple to give us high temperature readings from the oven, and a MAX6675 module to digitize the signal into SPI:

![](https://images-na.ssl-images-amazon.com/images/I/61lwhdlVPcL._SL1010_.jpg)

Of course the Arduino Uno (or similiar):
![](https://store-cdn.arduino.cc/uni/catalog/product/cache/1/image/1800x/ea1ef423b933d797cfca49bc5855eef6/A/0/A000066_front_2.jpg)

16-2 LCD:

![](https://cdn.shopify.com/s/files/1/0174/1800/products/5V_LCD_2_of_3_1024x1024.JPG?v=1427991643)



Simple momentary push button(to send interrupts to the Arduino):

![](https://images-na.ssl-images-amazon.com/images/I/61gFHrMfplL._SY355_.jpg)

Single Pole Single Throw (SPST) Rocker Switch:

![](https://cdn.sparkfun.com//assets/parts/1/8/4/7/08837-03-L.jpg)


Two solid state relays (one for each heating element):

![](https://images-na.ssl-images-amazon.com/images/I/615ijSjUgPL._SL1100_.jpg)

Couple LEDs (color doesn't matter) -  CAD enclosure is designed for 3mm.

![](http://unitednuclear.com/images/ledir.jpg)


Heat tape for sealing up the inside of the toaster oven:

![](https://images-na.ssl-images-amazon.com/images/I/51at3Wkm%2BGL._SL1006_.jpg)


That should do it for parts! Let's get started on the build!
##
##
##
##
##

 ### The Build: