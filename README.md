# MA4830-Project
Created for NTU MA4830 Realtime Software for Mechatronics System Project.<br>
Built on `Ubuntu 20.04` and tested on `QNX 6.X` using `GNU gcc 9.3.0`. <br>


# Cloning Repository
This repository can be cloned using
```
git clone https://github.com/gabrieljames01/MA4830-Project.git
cd MA4830-Project/
```

# Compiling and Running
**Due to Compability issues, wave output from PCI is only available on QNX Systems.**<br><br>

Ubuntu:
```
gcc wave_generator.c -o wg -lm
./wg
```
QNX:
```
cc wave_generator.c -o wg -lm
./wg
```
# Usage
`-w sine|square|sawtooth|triangular -f [float] -a [float]`<br><br>
Functions:<br>
-w Specific Type of Wave to be output.<br>
-f Specify the Frequency of the wave in Hertz (Hz).<br>
-a Specify the Amplitude of the wave.<br>
-? Show this message
