"""
Attach trade timestamps using volume_log.csv.

Prefer the Time column now written directly into trade_log.csv by the
simulator. This script remains as a fallback / validator.
"""

from pathlib import Path
import pandas as pd

ROOT = Path(__file__).resolve().parent.parent


def main():
    trades = pd.read_csv(ROOT / "trade_log.csv", skipinitialspace=True)
    trades.columns = [c.strip() for c in trades.columns]

    if "Time" in trades.columns and trades["Time"].notna().all():
        trades.to_csv(ROOT / "trade_log_with_time.csv", index=False)
        print("trade_log.csv already has Time — copied to trade_log_with_time.csv")
        return

    volumes = pd.read_csv(ROOT / "volume_log.csv", skipinitialspace=True)
    volumes.columns = [c.strip() for c in volumes.columns]

    trades = trades.copy()
    trades["Time"] = None
    trade_index = 0

    for _, volume_row in volumes.iterrows():
        time = volume_row["Time"]
        remaining_volume = volume_row["Volume"]

        while remaining_volume > 0 and trade_index < len(trades):
            qty = trades.loc[trade_index, "Qty"]
            trades.loc[trade_index, "Time"] = time
            remaining_volume -= qty
            trade_index += 1
            if remaining_volume < 0:
                raise ValueError(f"Volume mismatch at time {time}: exceeded by {-remaining_volume}")

    trades.to_csv(ROOT / "trade_log_with_time.csv", index=False)
    print(f"Wrote trade_log_with_time.csv ({len(trades)} trades)")


if __name__ == "__main__":
    main()
