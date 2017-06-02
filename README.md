# Bot Library

"Bot Library was born out of curiosity. BL is meant to act as the buffer between block and hard coding of robotic components. The goal is to foster individual's from a younger age into the field of AI and robotics. Hopefully Bot Library will reduce the barrier of intimidation towards the subject."

# Libraries Included

### botlibcore

The core of the botlib library holds the underlining function that will help the developer determine what libraries will work on their boards. In additon the core support gettimeofday() function in milliseconds consiodering lua only does timing in second. milliseconds are extremely important in the finding distance in sensor and delays in sensors.

### botlibgpio

GPIO (General Purpose Input Output) pins are built in to most of the computer boards for example ODroid and Raspberry Pi. This library is a clean and simple to ease the user in test and writing script to write and read hardware from the pins configurations. Botlibgpio supports all the conventionally GPIO protocol located at <a href="https://www.kernel.org/doc/Documentation/gpio/sysfs.txt">https://www.kernel.org/doc/Documentation/gpio/sysfs.txt</a>. Lastly the library has preload board configurations which allows the user to see the pin layout with color and labeling in the terminal.

### botlibi2c

### botlibsensor

### botlibservo





## License

MIT License

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
