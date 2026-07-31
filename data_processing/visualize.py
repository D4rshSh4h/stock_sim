"""
Stock sim visualization suite.

Reads CSV logs written by the C++ simulator and produces a multi-panel report.

Usage (from repo root):
  python data_processing/visualize.py
  python data_processing/visualize.py --show
  python data_processing/visualize.py --outdir data_processing/output
"""

from __future__ import annotations

import argparse
import os
import sys
from pathlib import Path

# Keep matplotlib cache inside the repo (sandbox / CI friendly)
_MPLDIR = Path(__file__).resolve().parent / ".mplconfig"
_MPLDIR.mkdir(parents=True, exist_ok=True)
os.environ.setdefault("MPLCONFIGDIR", str(_MPLDIR))

# Headless-friendly backend before pyplot import
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.gridspec import GridSpec

# ---------------------------------------------------------------------------
# Paths / style
# ---------------------------------------------------------------------------

REPO_ROOT = Path(__file__).resolve().parent.parent
DEFAULT_OUTDIR = Path(__file__).resolve().parent / "output"

COLORS = {
    "price": "#1f4e79",
    "volume": "#c47b2b",
    "bid": "#2e7d4f",
    "ask": "#a33b3b",
    "rate": "#5b4b8a",
    "cycle": "#4a7c8c",
    "fcf": "#6b5b4b",
    "buy": "#2e7d4f",
    "sell": "#a33b3b",
    "cons": "#3d5a80",
    "agg": "#e09f3e",
    "grid": "#d9d9d9",
    "text": "#222222",
}


def _style():
    plt.rcParams.update(
        {
            "figure.facecolor": "#f7f5f2",
            "axes.facecolor": "#fbfaf8",
            "axes.edgecolor": "#b0b0b0",
            "axes.labelcolor": COLORS["text"],
            "xtick.color": COLORS["text"],
            "ytick.color": COLORS["text"],
            "text.color": COLORS["text"],
            "grid.color": COLORS["grid"],
            "grid.linestyle": "-",
            "grid.linewidth": 0.6,
            "font.size": 9,
            "axes.titlesize": 11,
            "axes.labelsize": 9,
            "legend.fontsize": 8,
            "figure.titlesize": 14,
        }
    )


def _read_csv(path: Path) -> pd.DataFrame:
    if not path.exists():
        raise FileNotFoundError(f"Missing log file: {path}")
    df = pd.read_csv(path, skipinitialspace=True)
    df.columns = [c.strip() for c in df.columns]
    return df


def load_logs(root: Path) -> dict[str, pd.DataFrame]:
    logs = {
        "trades": _read_csv(root / "trade_log.csv"),
        "ticks": _read_csv(root / "tick_log.csv"),
        "orders": _read_csv(root / "order_log.csv"),
        "agents": _read_csv(root / "agent_log.csv"),
        "volume": _read_csv(root / "volume_log.csv"),
        "cycle": _read_csv(root / "cycle_log.csv"),
    }

    # Normalize common columns
    for key in ("trades", "ticks", "orders", "agents", "volume", "cycle"):
        if "Time" in logs[key].columns:
            logs[key]["Time"] = pd.to_numeric(logs[key]["Time"], errors="coerce")

    if "Type" in logs["trades"].columns:
        logs["trades"]["Side"] = logs["trades"]["Type"].map({0: "BuyAggressor", 1: "SellAggressor"})

    if "Side" in logs["orders"].columns:
        logs["orders"]["SideName"] = logs["orders"]["Side"].map({0: "Buy", 1: "Sell"})

    if "Behaviour" in logs["agents"].columns:
        logs["agents"]["BehaviourName"] = logs["agents"]["Behaviour"].map(
            {0: "Conservative", 1: "Aggressive"}
        )
    if "Behaviour" in logs["orders"].columns:
        logs["orders"]["BehaviourName"] = logs["orders"]["Behaviour"].map(
            {0: "Conservative", 1: "Aggressive"}
        )

    # Replace missing book markers
    for col in ("BestBid", "BestAsk", "Spread", "Mid"):
        if col in logs["ticks"].columns:
            logs["ticks"].loc[logs["ticks"][col] < 0, col] = np.nan

    return logs


def _save(fig: plt.Figure, outdir: Path, name: str):
    outdir.mkdir(parents=True, exist_ok=True)
    path = outdir / f"{name}.png"
    fig.savefig(path, dpi=160, bbox_inches="tight")
    print(f"  wrote {path}")


# ---------------------------------------------------------------------------
# Individual figures
# ---------------------------------------------------------------------------

def fig_market_overview(logs: dict, outdir: Path):
    ticks = logs["ticks"]
    trades = logs["trades"]
    vol = logs["volume"]

    fig = plt.figure(figsize=(14, 9))
    gs = GridSpec(3, 1, height_ratios=[2.2, 1.2, 1.2], hspace=0.28)
    ax_p = fig.add_subplot(gs[0])
    ax_v = fig.add_subplot(gs[1], sharex=ax_p)
    ax_s = fig.add_subplot(gs[2], sharex=ax_p)

    ax_p.plot(ticks["Time"], ticks["LastPrice"], color=COLORS["price"], lw=1.6, label="Last trade price")
    if ticks["Mid"].notna().any():
        ax_p.plot(ticks["Time"], ticks["Mid"], color="#888888", lw=1.0, ls="--", label="Mid")
    if not trades.empty:
        ax_p.scatter(
            trades["Time"],
            trades["Price"],
            s=np.clip(trades["Qty"] * 2.5, 8, 60),
            c=trades["Type"].map({0: COLORS["buy"], 1: COLORS["sell"]}),
            alpha=0.35,
            linewidths=0,
            label="Trades (size∝qty)",
        )
    ax_p.set_ylabel("Price")
    ax_p.set_title("Market price path")
    ax_p.grid(True, alpha=0.5)
    ax_p.legend(loc="upper right", frameon=False)

    ax_v.bar(vol["Time"], vol["Volume"], color=COLORS["volume"], width=0.8, alpha=0.85)
    ax_v.set_ylabel("Volume")
    ax_v.set_title("Traded volume by tick")
    ax_v.grid(True, axis="y", alpha=0.5)

    ax_s.plot(ticks["Time"], ticks["Spread"], color=COLORS["ask"], lw=1.2, label="Bid–ask spread")
    ax_s.set_ylabel("Spread")
    ax_s.set_xlabel("Time (tick)")
    ax_s.set_title("Quoted spread")
    ax_s.grid(True, alpha=0.5)

    fig.suptitle("Market overview", y=0.98)
    _save(fig, outdir, "01_market_overview")
    return fig


def fig_macro(logs: dict, outdir: Path):
    ticks = logs["ticks"]
    cycle = logs["cycle"]

    fig, axes = plt.subplots(3, 1, figsize=(13, 8), sharex=True, constrained_layout=True)

    axes[0].plot(ticks["Time"], ticks["LastPrice"], color=COLORS["price"], lw=1.5)
    axes[0].set_ylabel("Price")
    axes[0].set_title("Price vs macro backdrop")
    axes[0].grid(True, alpha=0.5)

    axes[1].plot(cycle["Time"], cycle["InterestRate"], color=COLORS["rate"], lw=1.6, label="Interest rate")
    axes[1].plot(cycle["Time"], cycle["BusinessCycle"], color=COLORS["cycle"], lw=1.2, label="Business cycle (sin)")
    axes[1].axhline(0.03, color="#999999", ls=":", lw=1, label="GDP growth (0.03)")
    axes[1].set_ylabel("Rate / cycle")
    axes[1].legend(loc="upper right", frameon=False)
    axes[1].grid(True, alpha=0.5)

    axes[2].step(ticks["Time"], ticks["FCF"], where="post", color=COLORS["fcf"], lw=1.5)
    axes[2].set_ylabel("FCF")
    axes[2].set_xlabel("Time (tick)")
    axes[2].set_title("Company free cash flow (earnings releases)")
    axes[2].grid(True, alpha=0.5)

    fig.suptitle("Macro & fundamentals")
    _save(fig, outdir, "02_macro_fundamentals")
    return fig


def fig_order_book(logs: dict, outdir: Path):
    ticks = logs["ticks"]

    fig, axes = plt.subplots(2, 1, figsize=(13, 7), sharex=True, constrained_layout=True)

    axes[0].fill_between(ticks["Time"], ticks["BuyQty"], color=COLORS["buy"], alpha=0.35, label="Bid qty")
    axes[0].fill_between(ticks["Time"], ticks["SellQty"], color=COLORS["sell"], alpha=0.35, label="Ask qty")
    axes[0].plot(ticks["Time"], ticks["BuyQty"], color=COLORS["buy"], lw=1.0)
    axes[0].plot(ticks["Time"], ticks["SellQty"], color=COLORS["sell"], lw=1.0)
    axes[0].set_ylabel("Resting qty")
    axes[0].set_title("Order book depth (shares)")
    axes[0].legend(frameon=False)
    axes[0].grid(True, alpha=0.5)

    axes[1].plot(ticks["Time"], ticks["BuyLevels"], color=COLORS["buy"], lw=1.2, label="Bid levels")
    axes[1].plot(ticks["Time"], ticks["SellLevels"], color=COLORS["sell"], lw=1.2, label="Ask levels")
    imbalance = (ticks["BuyQty"] - ticks["SellQty"]) / (ticks["BuyQty"] + ticks["SellQty"]).replace(0, np.nan)
    ax_r = axes[1].twinx()
    ax_r.plot(ticks["Time"], imbalance, color="#666666", lw=1.0, ls="--", label="Qty imbalance")
    ax_r.set_ylabel("Imbalance (bid−ask)/(bid+ask)")
    axes[1].set_ylabel("Price levels")
    axes[1].set_xlabel("Time (tick)")
    axes[1].set_title("Book levels & imbalance")
    axes[1].legend(loc="upper left", frameon=False)
    ax_r.legend(loc="upper right", frameon=False)
    axes[1].grid(True, alpha=0.5)

    fig.suptitle("Microstructure")
    _save(fig, outdir, "03_order_book")
    return fig


def fig_trades_and_orders(logs: dict, outdir: Path):
    trades = logs["trades"]
    orders = logs["orders"]

    fig = plt.figure(figsize=(14, 8))
    gs = GridSpec(2, 2, hspace=0.32, wspace=0.28)
    ax0 = fig.add_subplot(gs[0, 0])
    ax1 = fig.add_subplot(gs[0, 1])
    ax2 = fig.add_subplot(gs[1, 0])
    ax3 = fig.add_subplot(gs[1, 1])

    if not trades.empty:
        ax0.hist(trades["Price"], bins=40, color=COLORS["price"], alpha=0.85, edgecolor="white")
        ax0.set_title("Trade price distribution")
        ax0.set_xlabel("Price")
        ax0.set_ylabel("Count")

        ax1.hist(trades["Qty"], bins=30, color=COLORS["volume"], alpha=0.85, edgecolor="white")
        ax1.set_title("Trade size distribution")
        ax1.set_xlabel("Qty")
        ax1.set_ylabel("Count")

        cum = trades.sort_values("Time").assign(Notional=lambda d: d["Price"] * d["Qty"])
        cum["CumNotional"] = cum["Notional"].cumsum()
        cum["CumQty"] = cum["Qty"].cumsum()
        ax2.plot(cum["Time"], cum["CumQty"], color=COLORS["price"], lw=1.4)
        ax2.set_title("Cumulative traded shares")
        ax2.set_xlabel("Time")
        ax2.set_ylabel("Shares")
        ax2.grid(True, alpha=0.5)

    if not orders.empty:
        order_flow = (
            orders.groupby(["Time", "SideName"], as_index=False)["Qty"].sum()
            .pivot(index="Time", columns="SideName", values="Qty")
            .fillna(0)
        )
        if "Buy" in order_flow.columns:
            ax3.bar(order_flow.index, order_flow["Buy"], color=COLORS["buy"], alpha=0.7, label="Buy orders")
        if "Sell" in order_flow.columns:
            sell = order_flow["Sell"] if "Sell" in order_flow.columns else 0
            ax3.bar(order_flow.index, -sell, color=COLORS["sell"], alpha=0.7, label="Sell orders")
        ax3.axhline(0, color="#444444", lw=0.8)
        ax3.set_title("Order flow by tick (signed qty)")
        ax3.set_xlabel("Time")
        ax3.set_ylabel("Qty (+buy / −sell)")
        ax3.legend(frameon=False)
        ax3.grid(True, axis="y", alpha=0.5)

    fig.suptitle("Trades & order flow")
    _save(fig, outdir, "04_trades_orders")
    return fig


def fig_agents(logs: dict, outdir: Path):
    agents = logs["agents"]
    orders = logs["orders"]

    start = agents[agents["Time"] == agents["Time"].min()].copy()
    end = agents[agents["Time"] == agents["Time"].max()].copy()
    end = end.rename(columns={"Wealth": "WealthEnd", "Cash": "CashEnd", "Shares": "SharesEnd"})
    merged = start.merge(
        end[["AgentID", "WealthEnd", "CashEnd", "SharesEnd"]],
        on="AgentID",
        how="inner",
    )
    merged["PnL"] = merged["WealthEnd"] - merged["Wealth"]
    merged["Return"] = merged["PnL"] / merged["Wealth"].replace(0, np.nan)

    fig = plt.figure(figsize=(14, 9))
    gs = GridSpec(2, 2, hspace=0.32, wspace=0.28)
    ax0 = fig.add_subplot(gs[0, 0])
    ax1 = fig.add_subplot(gs[0, 1])
    ax2 = fig.add_subplot(gs[1, 0])
    ax3 = fig.add_subplot(gs[1, 1])

    for name, color in [("Conservative", COLORS["cons"]), ("Aggressive", COLORS["agg"])]:
        sub = merged[merged["BehaviourName"] == name]
        ax0.scatter(sub["GrowthRate"], sub["PnL"], s=36, alpha=0.8, c=color, label=name, edgecolors="white", linewidths=0.4)
    ax0.axhline(0, color="#666666", lw=0.8)
    ax0.set_xlabel("Growth belief (g_s, thousandths)")
    ax0.set_ylabel("Wealth change")
    ax0.set_title("P&L vs growth belief")
    ax0.legend(frameon=False)
    ax0.grid(True, alpha=0.5)

    # Mean wealth path by behaviour
    wealth_path = (
        agents.groupby(["Time", "BehaviourName"], as_index=False)["Wealth"].mean()
        .pivot(index="Time", columns="BehaviourName", values="Wealth")
    )
    for col, color in [("Conservative", COLORS["cons"]), ("Aggressive", COLORS["agg"])]:
        if col in wealth_path.columns:
            ax1.plot(wealth_path.index, wealth_path[col], color=color, lw=1.6, label=col)
    ax1.set_title("Mean wealth over time")
    ax1.set_xlabel("Time")
    ax1.set_ylabel("Wealth")
    ax1.legend(frameon=False)
    ax1.grid(True, alpha=0.5)

    ax2.hist(
        [merged.loc[merged["BehaviourName"] == "Conservative", "WealthEnd"],
         merged.loc[merged["BehaviourName"] == "Aggressive", "WealthEnd"]],
        bins=20,
        color=[COLORS["cons"], COLORS["agg"]],
        label=["Conservative", "Aggressive"],
        alpha=0.8,
        edgecolor="white",
    )
    ax2.set_title("Terminal wealth distribution")
    ax2.set_xlabel("Wealth")
    ax2.set_ylabel("Agents")
    ax2.legend(frameon=False)

    # Top agents by |order qty|
    if not orders.empty:
        activity = orders.groupby("AgentID", as_index=False).agg(Orders=("Qty", "count"), Qty=("Qty", "sum"))
        activity = activity.merge(merged[["AgentID", "BehaviourName", "GrowthRate", "PnL"]], on="AgentID", how="left")
        top = activity.nlargest(12, "Qty")
        colors = [COLORS["cons"] if b == "Conservative" else COLORS["agg"] for b in top["BehaviourName"]]
        ax3.barh(top["AgentID"].astype(str), top["Qty"], color=colors)
        ax3.set_title("Most active agents (order qty)")
        ax3.set_xlabel("Total ordered qty")
        ax3.set_ylabel("Agent ID")

    fig.suptitle("Agent behaviour & performance")
    _save(fig, outdir, "05_agents")
    return fig


def fig_agent_allocation(logs: dict, outdir: Path):
    agents = logs["agents"]
    end = agents[agents["Time"] == agents["Time"].max()].copy()

    fig, axes = plt.subplots(1, 2, figsize=(13, 5), constrained_layout=True)

    for name, color in [("Conservative", COLORS["cons"]), ("Aggressive", COLORS["agg"])]:
        sub = end[end["BehaviourName"] == name]
        axes[0].scatter(sub["Cash"], sub["Shares"], s=40, alpha=0.8, c=color, label=name, edgecolors="white", linewidths=0.4)
    axes[0].set_xlabel("Cash")
    axes[0].set_ylabel("Shares")
    axes[0].set_title("Terminal cash vs shares")
    axes[0].legend(frameon=False)
    axes[0].grid(True, alpha=0.5)

    # Gini-ish concentration: wealth share curve
    w = np.sort(end["Wealth"].clip(lower=0).to_numpy())
    if w.sum() > 0:
        cum_share = np.cumsum(w) / w.sum()
        x = np.linspace(0, 1, len(w))
        axes[1].plot(x, cum_share, color=COLORS["price"], lw=1.8, label="Wealth Lorenz")
        axes[1].plot([0, 1], [0, 1], color="#999999", ls="--", lw=1, label="Equality")
        axes[1].set_title("Wealth concentration (Lorenz)")
        axes[1].set_xlabel("Fraction of agents")
        axes[1].set_ylabel("Fraction of wealth")
        axes[1].legend(frameon=False)
        axes[1].grid(True, alpha=0.5)

    fig.suptitle("Allocation & inequality")
    _save(fig, outdir, "06_allocation")
    return fig


def fig_iv_beliefs(logs: dict, outdir: Path):
    """Orders colored by growth belief — proxy for IV disagreement."""
    orders = logs["orders"]
    ticks = logs["ticks"]
    if orders.empty:
        return None

    fig, axes = plt.subplots(2, 1, figsize=(13, 8), sharex=True, constrained_layout=True)

    sc = axes[0].scatter(
        orders["Time"],
        orders["Price"],
        c=orders["GrowthRate"],
        s=np.clip(orders["Qty"] / 5.0, 8, 70),
        cmap="coolwarm",
        alpha=0.65,
        linewidths=0,
    )
    axes[0].plot(ticks["Time"], ticks["LastPrice"], color="#222222", lw=1.2, label="Last price")
    cb = fig.colorbar(sc, ax=axes[0], pad=0.01)
    cb.set_label("Growth belief g_s")
    axes[0].set_ylabel("Order price")
    axes[0].set_title("Submitted orders colored by growth belief")
    axes[0].legend(frameon=False)
    axes[0].grid(True, alpha=0.5)

    # Mean order price premium vs mkt by behaviour over time
    orders = orders.copy()
    orders["Premium"] = orders["Price"] - orders["MktPrice"]
    prem = (
        orders.groupby(["Time", "BehaviourName"], as_index=False)["Premium"].mean()
        .pivot(index="Time", columns="BehaviourName", values="Premium")
    )
    for col, color in [("Conservative", COLORS["cons"]), ("Aggressive", COLORS["agg"])]:
        if col in prem.columns:
            axes[1].plot(prem.index, prem[col], color=color, lw=1.4, label=col)
    axes[1].axhline(0, color="#666666", lw=0.8)
    axes[1].set_xlabel("Time")
    axes[1].set_ylabel("Mean (order − mkt)")
    axes[1].set_title("Aggressiveness of quotes vs market")
    axes[1].legend(frameon=False)
    axes[1].grid(True, alpha=0.5)

    fig.suptitle("Beliefs & quoting (IV engine)")
    _save(fig, outdir, "07_iv_beliefs")
    return fig


def fig_summary_dashboard(logs: dict, outdir: Path):
    trades = logs["trades"]
    ticks = logs["ticks"]
    agents = logs["agents"]
    orders = logs["orders"]

    end = agents[agents["Time"] == agents["Time"].max()]
    start = agents[agents["Time"] == agents["Time"].min()]
    total_vol = int(logs["volume"]["Volume"].sum()) if not logs["volume"].empty else 0
    n_trades = len(trades)
    avg_px = float(trades["Price"].mean()) if n_trades else float("nan")
    final_px = float(ticks["LastPrice"].iloc[-1]) if not ticks.empty else float("nan")
    init_px = float(ticks["LastPrice"].iloc[0]) if not ticks.empty else float("nan")
    ret = (final_px / init_px - 1.0) * 100 if init_px else float("nan")

    lines = [
        "SIMULATION SUMMARY",
        "────────────────────────",
        f"Ticks:              {int(ticks['Time'].max()) if not ticks.empty else 0}",
        f"Agents:             {start['AgentID'].nunique() if not start.empty else 0}",
        f"Trades:             {n_trades}",
        f"Orders placed:      {len(orders)}",
        f"Total volume:       {total_vol}",
        f"Avg trade price:    {avg_px:.2f}",
        f"Start → end price:  {init_px:.2f} → {final_px:.2f} ({ret:+.1f}%)",
        f"Mean terminal wealth:{end['Wealth'].mean():.1f}" if not end.empty else "",
        f"Wealth std:         {end['Wealth'].std():.1f}" if not end.empty else "",
        f"Final FCF:          {int(ticks['FCF'].iloc[-1]) if not ticks.empty else 0}",
        f"Rate range:         {logs['cycle']['InterestRate'].min():.3f} – {logs['cycle']['InterestRate'].max():.3f}",
    ]

    fig, ax = plt.subplots(figsize=(8, 5))
    ax.axis("off")
    ax.text(
        0.02,
        0.98,
        "\n".join([ln for ln in lines if ln is not None]),
        va="top",
        ha="left",
        family="monospace",
        fontsize=11,
        color=COLORS["text"],
    )
    ax.set_title("Run summary", loc="left")
    _save(fig, outdir, "00_summary")
    return fig


# ---------------------------------------------------------------------------
# Entrypoint
# ---------------------------------------------------------------------------

def run(root: Path, outdir: Path, show: bool):
    _style()
    print(f"Loading logs from {root}")
    logs = load_logs(root)
    print("Generating figures...")

    figs = [
        fig_summary_dashboard(logs, outdir),
        fig_market_overview(logs, outdir),
        fig_macro(logs, outdir),
        fig_order_book(logs, outdir),
        fig_trades_and_orders(logs, outdir),
        fig_agents(logs, outdir),
        fig_agent_allocation(logs, outdir),
        fig_iv_beliefs(logs, outdir),
    ]

    if show:
        plt.show()
    else:
        for fig in figs:
            if fig is not None:
                plt.close(fig)

    print(f"Done. Figures in {outdir}")


def main(argv: list[str] | None = None):
    parser = argparse.ArgumentParser(description="Visualize stock_sim CSV logs")
    parser.add_argument("--root", type=Path, default=REPO_ROOT, help="Directory containing CSV logs")
    parser.add_argument("--outdir", type=Path, default=DEFAULT_OUTDIR, help="Where to write PNGs")
    parser.add_argument("--show", action="store_true", help="Display figures interactively")
    args = parser.parse_args(argv)
    try:
        run(args.root, args.outdir, args.show)
    except FileNotFoundError as exc:
        print(exc, file=sys.stderr)
        print("Run ./main from the repo root first to generate logs.", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
