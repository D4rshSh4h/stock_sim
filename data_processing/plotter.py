"""
Thin entrypoint — prefer visualize.py for the full report.

  python data_processing/plotter.py
"""

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
from visualize import main

if __name__ == "__main__":
    main()
