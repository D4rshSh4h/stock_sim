import pandas as pd

# Load CSV files
trades = pd.read_csv("trade_log.csv")
volumes = pd.read_csv("volume_log.csv")
print("Read files starting to process")

# Add empty Time column
trades["Time"] = None

trade_index = 0

# Loop through each volume timestamp
for _, volume_row in volumes.iterrows():
    time = volume_row["Time"]
    remaining_volume = volume_row[" Volume"]

    # Assign trades to this time until volume is exhausted
    while remaining_volume > 0 and trade_index < len(trades):
        qty = trades.loc[trade_index, " Qty"]

        trades.loc[trade_index, "Time"] = time

        remaining_volume -= qty
        trade_index += 1

        # Optional safety check
        if remaining_volume < 0:
            raise ValueError(
                f"Volume mismatch at time {time}: exceeded by {-remaining_volume}"
            )

#Save result
trades.to_csv("trade_log_with_time.csv", index=False)

#print(trades)