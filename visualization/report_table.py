# report_table.py

import csv
import re
from pathlib import Path

RESULTS_DIR = Path("results/results_out")

time_re = re.compile(r"Task took ([0-9.eE+-]+)")
avg_re = re.compile(r"Avg error = ([0-9.eE+-]+)")
max_re = re.compile(r"Max error = ([0-9.eE+-]+)")

rows = []

for file in RESULTS_DIR.glob("*.txt"):
    text = file.read_text()

    time_match = time_re.search(text)
    if not time_match:
        continue

    avg_match = avg_re.search(text)
    max_match = max_re.search(text)

    name = file.stem

    row = {
        "impl": "",
        "case": "",
        "grid": 0,
        "threads": 1,
        "time": float(time_match.group(1)),
        "avg_error": float(avg_match.group(1)) if avg_match else 0.0,
        "max_error": float(max_match.group(1)) if max_match else 0.0,
    }

    if "_LPI_" in name:
        row["case"] = "LPI"
    elif "_L1_" in name:
        row["case"] = "L1"
    else:
        continue

    if name.startswith("serial"):
        row["impl"] = "serial"

        m = re.search(r"N(\d+)", name)
        if not m:
            continue

        row["grid"] = int(m.group(1))

    elif name.startswith("openmp"):
        row["impl"] = "openmp"

        m = re.search(r"N(\d+)_T(\d+)", name)
        if not m:
            continue

        row["grid"] = int(m.group(1))
        row["threads"] = int(m.group(2))

    else:
        continue

    rows.append(row)

# serial-время для каждой серии экспериментов
serial_times = {}

for row in rows:
    if row["impl"] == "serial":
        serial_times[
            (row["case"], row["grid"])
        ] = row["time"]

# вычисление ускорения
for row in rows:
    baseline = serial_times.get(
        (row["case"], row["grid"])
    )

    if baseline is None:
        row["speedup"] = 0.0
        row["efficiency"] = 0.0
        continue

    if row["impl"] == "serial":
        row["speedup"] = 1.0
        row["efficiency"] = 1.0
    else:
        row["speedup"] = baseline / row["time"]
        row["efficiency"] = (
            row["speedup"] /
            row["threads"]
        )

rows.sort(
    key=lambda r: (
        r["case"],
        r["grid"],
        r["threads"],
        r["impl"]
    )
)

print(
    f"{'case':6}"
    f"{'impl':10}"
    f"{'grid':8}"
    f"{'thr':8}"
    f"{'time':15}"
    f"{'speedup':12}"
    f"{'eff':12}"
)

for row in rows:
    print(
        f"{row['case']:6}"
        f"{row['impl']:10}"
        f"{row['grid']:8}"
        f"{row['threads']:8}"
        f"{row['time']:15.6f}"
        f"{row['speedup']:12.4f}"
        f"{row['efficiency']:12.4f}"
    )

# общий CSV
with open(
    "report_table.csv",
    "w",
    newline=""
) as csvfile:

    writer = csv.writer(csvfile)

    writer.writerow([
        "case",
        "implementation",
        "grid",
        "threads",
        "time_sec",
        "speedup",
        "efficiency",
        "avg_error",
        "max_error",
    ])

    for row in rows:
        writer.writerow([
            row["case"],
            row["impl"],
            row["grid"],
            row["threads"],
            row["time"],
            row["speedup"],
            row["efficiency"],
            row["avg_error"],
            row["max_error"],
        ])

print("\nSaved: report_table.csv")
