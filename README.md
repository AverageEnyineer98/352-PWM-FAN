# 352-PWM-FAN
#Background Story
This project was actually my school project in 2022 Fall term! 
I was taking a Digital Logic / Microcontroller class, the class were separated in 2 parts, the first is digital logic, basic AND OR gates,flip flops and finite state machines (FSM)
The second part of the course is about the famous 8051 microcontroller, it was developed by in the 1980s(wiki), intel was the manufacturer.
We basically learnt the functions of the 8051 and actually got labs to implment hex code on the machine! 
Fun fact I did not participated much in the lab at the start, I helped to cut the cables LOLOL
But than We were told that our project is to make a 8051 PWM fan controller with feedback, AND than I also knew that we can actually code the MCU in C. 
This comes to another story where I bought a XMOS MCU before and the IDE (xTimeComposer) uses embedeed C too, so it gave me the motivation to learn embedded C in the course
So I decided to take over the project and finished it on my own. (Except the report writing XD)

#Project Description
The project is simple, the PWM fan is powered by a 12V power supply, the PWM input of the fan is controlled by the MCU, 
and we will use 3 digit of a DIP switch to control the speed of the fan, and a MOSFET is needed.
The PWM fan also provides a blue cable for feedback, and it is connected to the MCU with a pull down resistor.
The feedback will be monitored with interrupts and timers, and the speed will be converted with a certain period and then shown on 3 7-segments displays.
Funny thing the max speed is 2400 but we only show the first three digit for a speed >999

#Facts
One thing that I didnt know before was that, the fan input and output (PWM and feedback respectively) signals work totally different!
For the speed sensor, the mechanism is that when the speed is high, frequency of output wave is high, where when speed is low, frequency of output wave is low. 
On the other hand, the input of the fan is based on PWM

#RPM calculation
Basically the RPM is calculated based on counting the interrupts from the fan in a certain time frame.
For example, the time frame is 50ms, and than I will count how many interrupts captured/2/time frame *60
*Divided by 2 because this fan model gives 2 interrupts in each cycle.
*Divided by 60 because we want it in RPMinutes

