Refactoring Task List
 Verify baseline compilation and execution.
 Create config.h and types.h (or combine them) to store enums (AgentState, OrderStatus, OrderType) and configuration constants (RANGE, BUFFER, Float/Int conversion helpers).
 Refactor Order_structures (order.h/cpp & orderbook.h/cpp)
Change float to int for price indexing. Ensure precision conversions happen at boundary inputs/outputs.
Test compilation and logic for Order_structures.
 Refactor File I/O & Simulator Logging (trades.h/cpp)
Implement a buffered logging system.
Include a debug toggle in config.h to choose between buffered logging or immediate raw logging.
Test compilation and verify logs are still generated correctly.
 Refactor the Simulator Timeout Mechanism (Simulator.cpp)
Modify the timeout mechanism to use an efficient tracking method instead of $O(N)$ searches over past time indexes.
Test full simulator run.
 Refactor Trade_logic (trades.h/cpp)
Encapsulate matching logic into a MatchingEngine class.
Decouple global state (trade_observer = nullptr).
Final full simulation test.