# Stock Market Simulator

This is a fun project i'm doing to also (hopefully) learn some c++ along the way.

---

## Overview

A stock market simulator built from first principles. Currently includes multiple autonomous agents trading a single asset. 
The simulator runs a tick-based loop. At each tick, every agent independently decides whether to buy, sell, or hold based on the current market price. Orders are submitted to a matching engine which attempts to pair buyers and sellers. Unmatched orders rest in the order book until they are filled, expire, or the simulation ends. As of now, an agent makes a decision completely at random using a uniform distribution based on the price of the previous trade. Certain data is then logged to csv files and can then be processed using python scripts. Data logging is buffered or continous.

Note that all simulation parameters are in config.h

---

## Usage

Link on releases will show output using pre-configurated values FOR WINDOWS ONLY. For mac/linux, or to change values, download the repo and build/run it yourself.

Compile all files in Order_structures, Simulator_structures and Trade_logic along with main, config and file_handler. Adjust values in config.h (you can play around with them) and run to see values printed in your csv files. 

---
### Disclaimer
Although i'm trying to make it as accurate as possible this project is for fun and will not fully simulate any financial market.

Excuse the poor documentation - it will be fixed soon (I promise). 

The new IVDecisionEngine used by agents is not yet perfect which might result in abnormal prices during the simulator. 


Credit - github@D4rshSh4h
