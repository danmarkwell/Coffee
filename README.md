# Coffee

This project implements a timer on/off control into a Rocket Appartemento Espresso Machine. In addition it also implements grind-time control into a Rancilio Rocky grinder. While this setup has been created for these machines, it should be easily addaptable to any home espresso setup.

The setup includes three relays, one to control the grinder, and two to control the coffee machine. Two relays were necessary on the coffe machine due to the fact that the original switch for the machine switched both the live and neutral wires. This will vary depending on the machine, but the code will remain the same. 

The switch for the coffee machine was changed to a momentary button to facilitate the timer function. This step is of course optional, but was taken to avoid situations where the machine had been turned on by the timer function, but the switch remained in the off position. If such a situation would be acceptable to you then only a few small changes would need to be made to the code to facilitate such a setup. Personally I prefer the look of the new button but to each their own.

The switch for the grinder was changed from a single throw to a double throw switch. This is where the left and right comes from in the program, and again was an optional choice. It was chosen this was to be able to have two preprogrammed grind times, or one grind time and one push to operate. Should one time be sufficient the original switch can be left and all of the programing corresponding to the grind_r can be removed.

Finally this program integrates a 20x4 LCD display with backlight, and an I2C backpack board for easy wiring and interface. This screen is used to show the time as well as the on/off times and the grind times. Feel free to customise what is displayed as you will. The screen is initialized using the vari() program, and it is there that most of the changes will need to be made. 

Should you have any questions, comments or concerns feel free to email at dmarkwell83@gmail.com -- use subject CoffeePro
