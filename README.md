LILYGO T-Display-S3 KY-040 Rotary Encoder Module Project

This sketch reads the rotary encoder module and displays the value along with a slider on the built-in
screen using the TFT_eWidget library. The counter value and slider position are updated in real-time.

Description:

 The code is based off of the slider example project from the TFT_eWidget library and modified to
 work with the KY-040 rotary encoder module on a (non-touch) LilyGo T-Display-S3.
 The rotary encoder moves the knob on the slider and updates the counter value.
 Pressing the encoder button will reset the slider to the middle (50) and counter to 50.
 On startup, the slider will sweep from 50 to 0, then to 100, and back to 50 within 1sec.

Pin Connections:
 - Module VCC -> 3.3V
 - Module GND -> GND
 - Module SW  -> GPIO01 (input)
 - Module DT  -> GPIO02 (input)
 - Module CLK -> GPIO03 (input)

KY-040 Specifications:
 - Protocol: Digital
 - Operating Voltage: 3.3V to 5V
 - Pulses per Revolution: 20
 - Output: 2-bit Gray Code
 - Mechanical Angle: 360° Continuous
 - Built-in Switch: Yes (Push-to-Operate)
