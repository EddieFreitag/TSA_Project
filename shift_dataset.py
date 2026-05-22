import numpy as np

# ============================================================
# Create shifted Chlorine dataset
# Each column is circularly shifted by a random amount
# up to +/- 1 day (288 samples)
# ============================================================

INPUT_FILE = "tkcm/cl2fullLarge.dat"
OUTPUT_FILE = "tkcm/cl2fullLarge_shifted.dat"
# 288 one day at 5-minute intervals (12 per hour * 24 hours)
MAX_SHIFT = 144   # half a day to avoid excessive distortion of temporal patterns

# reproducibility
rng = np.random.default_rng(seed=7)

# load dataset
data = np.loadtxt(INPUT_FILE)

rows, cols = data.shape

print(f"Loaded dataset: {rows} rows, {cols} columns")

shifted = np.empty_like(data)

shifts = []

# independently shift each time series (column)
for col in range(cols):

    # random shift between -288 and +288
    shift = rng.integers(-MAX_SHIFT, MAX_SHIFT + 1)

    shifts.append(shift)

    # circular shift
    shifted[:, col] = np.roll(data[:, col], shift)

    print(f"Column {col:3d}: shift = {shift:4d}")

# save shifted dataset
np.savetxt(
    OUTPUT_FILE,
    shifted,
    fmt="%.10f"
)

print(f"\nSaved shifted dataset to: {OUTPUT_FILE}")

# also save the actual shifts for reproducibility
with open("tkcm/cl2fullLarge_shifts.txt", "w") as f:
    for col, shift in enumerate(shifts):
        f.write(f"{col}\t{shift}\n")

print("Saved applied shifts to: tkcm/cl2fullLarge_shifts.txt")