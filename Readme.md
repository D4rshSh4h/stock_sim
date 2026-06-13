# Stock Market Simulator

This is a fun project i'm doing to also (hopefully) learn some c++ along the way.
---

## Overview

A stock market simulator built from first principles. Currently includes multiple autonomous agents trading a single asset. 
The simulator runs a tick-based loop. At each tick, every agent independently decides whether to buy, sell, or hold based on the current market price. Orders are submitted to a matching engine which attempts to pair buyers and sellers. Unmatched orders rest in the order book until they are filled, expire, or the simulation ends. As of now, an agent makes a decision completely at random using a uniform distribution based on the price of the previous trade.

Certain data is then logged to csv files and can then be processed using python scripts. Data logging is buffered or continous.

Note that all simulation parameters are in config.h
---

## Future tasks
Currently working on a more accurate simulator. This will include agents with different 'personalities' to simulate varied behaviour like in real markets. Furthermore, the simulator will be encapsulated in a 'financial world', which will influence interest rates and money supply for some macro factors. 

Updates for data processing, logging and presentation will be continous
---

### Disclaimer
Although i'm trying to make it as accurate as possible this project is for fun and will not fully simulate any financial market.

Excuse the poor documentation - it will be fixed soon (I promise)


