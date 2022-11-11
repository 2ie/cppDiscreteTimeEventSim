# Discrete Time Event Simulation Demo
## Mateo Escamilla

**This program needs to be compiled with -std=c++11** 

## Usage:

The program takes the following commandline arguments:
```
./prog2 <average_arrival_rate> <average_service_time>
```
For instance, if I wanted the simulation to have an average arrival rate of 30 processes per second, and an average service time of 0.04 seconds, I would type:
```
./prog2 30 0.04
```
Then hit `enter`, and prog2 will output the simulation's metrics to the console. No extra user input is needed!