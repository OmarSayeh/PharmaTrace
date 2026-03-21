import pandas as pd
import numpy as np
from itertools import combinations

# STEP 1: Load drug file 
print("\n[1/5]Loading drug file...")
drug_df = pd.read_csv("data/DRUG25Q2.txt", sep="$",
                      usecols=["primaryid", "drugname"],
                      dtype=str, on_bad_lines="skip")

drug_df["drugname"] = drug_df["drugname"].str.upper().str.strip()
drug_df = drug_df.dropna(subset=["primaryid", "drugname"])
print(f"  Loaded {len(drug_df):,} drug rows, {drug_df['primaryid'].nunique():,} unique reports")

print(f"      Rows loaded:      {len(drug_df):,}")
print(f"      Unique reports:   {drug_df['primaryid'].nunique():,}")
print(f"      Unique drugs:     {drug_df['drugname'].nunique():,}")


# STEP 2: Load reaction file + score severity 
print("\n[2/5]Loading reaction file and scoring severity...")
reac_df = pd.read_csv("data/REAC25Q2.txt", sep="$",
                      usecols=["primaryid", "pt"],
                      dtype=str, on_bad_lines="skip")

KEYWORD_SCORES = [
    ("death",        1.0),
    ("arrest",       0.95),
    ("shock",        0.92),
    ("rupture",      0.90),
    ("perforation",  0.90),
    ("overdose",     0.88),
    ("sepsis",       0.88),
    ("failure",      0.87),
    ("haemorrhage",  0.85),
    ("hemorrhage",   0.85),
    ("necrosis",     0.85),
    ("necrotis",     0.85),
    ("malignant",    0.82),
    ("infarct",      0.82),
    ("embolism",     0.82),
    ("anaphyla",     0.82),
    ("leukaemia",    0.80),
    ("leukemia",     0.80),
    ("carcinoma",    0.78),
    ("cancer",       0.78),
    ("thrombosis",   0.76),
    ("thrombo",      0.74),
    ("neoplasm",     0.74),
    ("fibrosis",     0.72),
    ("seizure",      0.72),
    ("convuls",      0.72),
    ("angioedema",   0.70),
    ("encephalop",   0.70),
    ("meningitis",   0.70),
    ("suicid",       0.75),
    ("hepatitis",    0.68),
    ("cirrhosis",    0.68),
    ("cardiac",      0.65),
    ("pulmonary",    0.62),
    ("neutropenia",  0.62),
    ("pancytopenia", 0.65),
    ("haemolysis",   0.60),
    ("respiratory",  0.58),
    ("renal",        0.58),
    ("kidney",       0.58),
    ("dyspnoea",     0.55),
    ("hypotension",  0.55),
    ("anaemia",      0.45),
    ("anemia",       0.45),
    ("hypertension", 0.45),
    ("infection",    0.42),
    ("disorder",     0.32),
    ("pain",         0.30),
    ("vomit",        0.28),
    ("diarrh",       0.28),
    ("headache",     0.28),
    ("dizziness",    0.28),
    ("rash",         0.26),
    ("nausea",       0.25),
    ("fatigue",      0.25),
    ("pruritus",     0.22),
    ("itch",         0.22),
    ("cough",        0.20),
    ("insomnia",     0.20),
    ("decreased",    0.20),
    ("abnormal",     0.22),
    ("increased",    0.18),
]

def get_severity(term):
    if pd.isna(term):
        return 0.2
    term_lower = str(term).strip().lower()
    for keyword, score in KEYWORD_SCORES:
        if keyword in term_lower:
            return score
    return 0.2  # default for unmatched terms

reac_df["severity"] = reac_df["pt"].apply(get_severity)
report_severity = reac_df.groupby("primaryid")["severity"].max()

print(f"      Rows loaded:      {len(reac_df):,}")
print(f"      Unique reactions: {reac_df['pt'].nunique():,}")

# Print severity distribution
bins   = [0, 0.2, 0.4, 0.6, 0.8, 1.01]
labels = ["Minimal (0.0-0.2)", "Low (0.2-0.4)",
          "Moderate (0.4-0.6)", "High (0.6-0.8)", "Critical (0.8-1.0)"]
reac_df["severity_bin"] = pd.cut(reac_df["severity"], bins=bins,
                                  labels=labels, right=False)
print("\n      Severity distribution:")
for label, count in reac_df["severity_bin"].value_counts().sort_index().items():
    pct = count / len(reac_df) * 100
    print(f"        {label:<22} {count:>8,}  ({pct:.1f}%)")

# STEP 3: Generate drug pairs per report (ex: a report has drugs A, B, and C, it produces pairs (A,B), (A,C), and (B,C))
print("\n[3/5] Generating drug pairs per report...")

def get_pairs(drugs):
    unique = list(set(drugs))
    if len(unique) < 2:
        return []
    return list(combinations(sorted(unique), 2))

pairs = (
    drug_df.groupby("primaryid")["drugname"]
    .apply(get_pairs)
    .explode()
    .dropna()
    .reset_index()
)

pairs[["drug_a", "drug_b"]] = pd.DataFrame(
    pairs["drugname"].tolist(), index=pairs.index
)
pairs = pairs.drop(columns=["drugname"])

print(f"      Raw pairs generated: {len(pairs):,}")

# STEP 4: Attach severity + compute edge weights (ex: If (WARFARIN, ASPIRIN) appeared in 500 different reports, that becomes one edge with occ = 500)
print("\n[4/5] Computing edge weights...")

pairs["severity"] = pairs["primaryid"].map(report_severity).fillna(0.2)

edge_df = (
    pairs.groupby(["drug_a", "drug_b"])
    .agg(
        co_occurrences=("primaryid", "count"),
        avg_severity=("severity", "mean")
    )
    .reset_index()
)

# weight = avg_severity * log(co_occurrence_count + 1)
edge_df["weight"] = edge_df["avg_severity"] * np.log1p(edge_df["co_occurrences"])

# Before filter stats
print(f"      Unique edges (before filter): {len(edge_df):,}")

# Filter out very rare pairs (< 3 co-occurrences = likely noise)
edge_df = edge_df[edge_df["co_occurrences"] >= 3].reset_index(drop=True)

print(f"      Unique edges (after filter):  {len(edge_df):,}")
print(f"      Unique drugs (nodes):         {edge_df['drug_a'].nunique():,}")

# STEP 5: Export edges.csv 
print("\n[5/5] Saving edges.csv...")

edge_df[["drug_a", "drug_b", "weight"]].to_csv("edges.csv", index=False)

print(f"      Saved to: edges.csv")

# FINAL SUMMARY 
print("\n" + "=" * 50)
print("  DONE — Summary")
print("=" * 50)
print(f"  Total edges:     {len(edge_df):,}")
print(f"  Total nodes:     {edge_df['drug_a'].nunique():,}")
print(f"  Avg weight:      {edge_df['weight'].mean():.4f}")
print(f"  Max weight:      {edge_df['weight'].max():.4f}")
print(f"  Min weight:      {edge_df['weight'].min():.4f}")
print("\n  Sample edges (highest weight = most dangerous):")
print(
    edge_df[["drug_a", "drug_b", "weight"]]
    .sort_values("weight", ascending=False)
    .head(10)
    .to_string(index=False)
)