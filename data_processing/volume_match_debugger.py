"""Quick sanity check: trade qty sum vs volume_log total."""

from pathlib import Path
import pandas as pd

ROOT = Path(__file__).resolve().parent.parent


def main():
    trades = pd.read_csv(ROOT / "trade_log.csv", skipinitialspace=True)
    trades.columns = [c.strip() for c in trades.columns]
    volumes = pd.read_csv(ROOT / "volume_log.csv", skipinitialspace=True)
    volumes.columns = [c.strip() for c in volumes.columns]

    trade_vol = int(trades["Qty"].sum()) if "Qty" in trades.columns else 0
    log_vol = int(volumes["Volume"].sum()) if "Volume" in volumes.columns else 0

    print(f"Trade log total qty:   {trade_vol}")
    print(f"Volume log total qty:  {log_vol}")
    print("MATCH" if trade_vol == log_vol else "MISMATCH")


if __name__ == "__main__":
    main()
