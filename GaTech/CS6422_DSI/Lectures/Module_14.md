# Columnar vs Row Storage in Databases

## 1. Overview

This lesson explains how **columnar storage formats** improve the performance of **analytical queries**, compared to traditional **row-based storage**.

We use a **weather dataset** with the following columns:
- `timestamp`
- `temperature`
- `humidity`
- `wind_speed`

---

## 2. Example Query

**Goal:**
Find the **average temperature** between timestamps 5 and 75.

**Important insight:**
- Only **2 columns are needed**:
  - `timestamp`
  - `temperature`

---

## 3. Row Storage

### How it Works
- Stores **entire rows together** in sequence.
- Each page contains full records:
[timestamp, temperature, humidity, wind_speed]


### Advantages
- Efficient for **transactional workloads (OLTP)**
- Good when you frequently access **entire records**

### Disadvantages
- Inefficient for analytical queries:
- Must read **all columns**, even unnecessary ones
- Causes extra:
  - Disk I/O
  - Memory usage

### Example Cost
- Query requires **7 pages read**
- Includes unnecessary columns:
- `humidity`
- `wind_speed`

---

## 4. Columnar Storage

### How it Works
- Stores **each column separately**
- Example:
- Page 1 → timestamps
- Page 2 → temperatures
- Page 3 → humidity
- Page 4 → wind speed

### Advantages
- Only reads **needed columns**
- Reduces:
- Disk I/O
- Memory usage
- Much faster for **analytical queries (OLAP)**

### Example Cost
- Only **4 pages read**
- Reads only:
- `timestamp`
- `temperature`

---

## 5. Key Differences

| Feature              | Row Storage                  | Columnar Storage              |
|---------------------|-----------------------------|-------------------------------|
| Storage Layout      | Rows stored together        | Columns stored separately     |
| Best For            | Transactional workloads     | Analytical workloads          |
| Access Pattern      | Full row access             | Specific column access        |
| I/O Efficiency      | Low (reads extra data)      | High (reads only needed data) |
| Compression         | Limited                     | High (same data types)        |

---

## 6. Compression Advantage

### Row Storage
- Mixed data types in each row
- Harder to compress efficiently

### Columnar Storage
- Same data type per column
- Enables:
- Better compression
- Smaller storage footprint
- Faster scans

---

## 7. Synthetic Dataset Example

### Dataset Details
- **1 million rows**
- Columns:
- `timestamp` (sequential)
- `temperature` (normal distribution)
- `humidity` (uniform distribution)
- `wind_speed` (uniform distribution)
- Stored in **4 KB pages**

---

## 8. Query Execution Comparison

### Row Storage Execution

1. Scan all rows
2. Filter by timestamp
3. Read ALL columns
4. Compute average temperature

**Problem:**
- Reads unnecessary data → high I/O cost

---

### Columnar Storage Execution

1. Scan `timestamp` column
2. Identify matching rows
3. Store matching offsets
4. Read only corresponding `temperature` values
5. Compute average

**Benefit:**
- Reads minimal data → efficient

---

## 9. Performance Results

- Columnar storage is **~30x faster** for this query
- Reason:
- Reads **4x fewer pages**
- Avoids unnecessary columns

---

## 10. Key Takeaways

### Core Idea
> Columnar storage improves analytical query performance by minimizing unnecessary data access.

### When to Use Each

- Use **Row Storage** when:
- You need full records
- Workload is transactional (OLTP)

- Use **Columnar Storage** when:
- You analyze subsets of data
- Workload is analytical (OLAP)

### Big Picture Insight

There is a tight relationship between:
- **Storage format**
- **Query execution performance**

Choosing the right storage layout directly impacts speed and efficiency.

---

## 11. Mental Model (For Exams)

- **Row = good for writes + full reads**
- **Column = good for analytics + partial reads**

Think:
> "Do I need the whole row, or just a few columns?"

---

## 12. One-Line Summary

Columnar storage speeds up analytical queries by reading **only the necessary columns**, reducing I/O and improving performance dramatically.


# Data Compression in Columnar Databases

## 1. Overview

This lesson focuses on how **data compression**:
- Reduces **storage costs**
- Improves **query performance**
- Minimizes **disk I/O operations** (major bottleneck)

Compression is especially powerful when combined with **columnar storage**.

---

## 2. Why Compression Matters

### Core Idea
> Smaller data = fewer disk reads = faster queries

### Benefits
- Less disk space used
- Faster data scans
- More data fits in memory (buffer pool)
- Reduced I/O (biggest performance win)

---

## 3. Why Columnar Storage Helps Compression

### Row Storage Problem
- Each row contains mixed data types:
  - timestamp, product_id, price
- Hard to find patterns → poor compression

### Columnar Storage Advantage
- Each column contains:
  - Same data type
  - Similar values
- Enables:
  - Pattern detection
  - High compression ratios

### Examples
- Product IDs → repeat frequently
- Temperatures → small numeric range
- Timestamps → incremental changes

---

## 4. Types of Data

### Numeric Data
- Measurable values
- Examples:
  - timestamps
  - temperature

### Categorical Data
- Discrete labels
- Examples:
  - product IDs
  - country names

---

## 5. Compression Algorithms

## 5.1 Delta Encoding (Numeric Data)

### Idea
Store **differences (deltas)** instead of full values

### Example
Instead of:
1000, 1005, 1010, 1015

Store:
1000, +5, +5, +5


### Why It Works
- Differences are smaller numbers
- Require fewer bits

### Best Use Case
- Sequential or slowly changing values (e.g., timestamps)

---

## 5.2 Bit Packing (Numeric Data)

### Idea
Use **minimum number of bits** needed per value

### Example
Temperatures:
35, 40, 45, 50


- Range: 0–63 → fits in **6 bits**
- Instead of:
  - 32 bits (4 bytes) per value
- Use:
  - 6 bits per value

### Benefit
- Massive space savings

---

### How Bit Packing Works

#### Compression
1. Determine max value → required bit width
2. Pack values tightly into a byte array
3. Use bitwise operations

#### Decompression
1. Locate starting bit position
2. Read required number of bits
3. Reconstruct original value

---

## 5.3 Huffman Encoding (Categorical Data)

### Idea
Assign **shorter codes to frequent values**

### Example
If "US" appears frequently:
- "US" → `01`
- Rare value → `110101`

### Benefit
- Reduces size based on frequency

### Best Use Case
- Text / string data with uneven frequency distribution

---

## 5.4 Dictionary Encoding (Categorical Data)

### Idea
Replace values with **compact integer codes**

### Example
Original:
"US", "CA", "US", "MX"

Dictionary:
US → 1
CA → 2
MX → 3


Stored data:
1, 2, 1, 3


### Benefit
- Smaller storage
- Faster comparisons

### Best Use Case
- Columns with **few unique values (low cardinality)**

---

## 6. Why Compression Works Better in Columnar Storage

### Key Insight
> Compression thrives on similarity and repetition

Columnar storage provides:
- Same data types per column
- High redundancy
- Predictable patterns

This enables:
- Better compression ratios
- Faster decompression

---

## 7. Performance Impact

Compression improves performance by:
- Reducing disk reads
- Increasing cache efficiency
- Allowing more data in memory

Especially important for:
- Analytical queries (OLAP)
- Large datasets

---

## 8. Mental Model (For Exams)

- **Delta Encoding → store changes**
- **Bit Packing → use fewer bits**
- **Huffman → compress by frequency**
- **Dictionary → map to small codes**

---

## 9. When to Use What

| Algorithm            | Data Type     | Best For                          |
|---------------------|--------------|----------------------------------|
| Delta Encoding      | Numeric       | Sequential data (timestamps)      |
| Bit Packing         | Numeric       | Small value ranges               |
| Huffman Encoding    | Categorical   | Skewed frequency distributions    |
| Dictionary Encoding | Categorical   | Few unique values                 |

---

## 10. Key Takeaways

- Compression reduces **storage + I/O**
- Columnar storage makes compression **much more effective**
- Different algorithms suit different data types
- Compression is critical for **database performance at scale**

---

## 11. One-Line Summary

Compression + columnar storage dramatically speeds up queries by shrinking data size and reducing disk I/O.

# Advanced Compression Techniques & Performance in Columnar Databases

## 1. Overview

This lesson dives deeper into:
- **Huffman Encoding**
- **Dictionary Encoding (Byte Encoding)**
- Applying compression to real datasets
- Measuring **performance improvements**

---

## 2. Huffman Encoding

### Core Idea
> Assign **short binary codes to frequent values** and **longer codes to rare values**

---

### Example

| Product ID | Frequency | Huffman Code |
|------------|----------|--------------|
| 111111     | High     | 0            |
| 444444     | Low      | 1111         |

---

### How It Works

1. Count frequency of each value
2. Build a **Huffman Tree**:
   - Frequent values → closer to root
   - Rare values → deeper in tree
3. Assign binary codes based on position

---

### Key Properties

- Variable-length encoding
- Prefix-free (no code is a prefix of another)
- Optimized for frequency distribution

---

### Tradeoffs

**Pros:**
- Strong compression for skewed data
- Reduces storage size

**Cons:**
- Requires **bit-level decoding**
- Adds computational overhead

---

## 3. Dictionary Encoding (Byte Encoding)

### Core Idea
> Replace categorical values with **small integer codes**

---

### Constraints

- Only works well when:
  - **Few unique values (low cardinality)**
- Uses:
  - 1 byte → values from `0–255`

---

### Compression Process

1. Iterate through column values
2. If value exists:
   - Use existing code
3. If new value:
   - Assign next available byte
4. Maintain:
   - Dictionary (encode)
   - Reverse dictionary (decode)

---

### Example
Original:
"111111", "222222", "111111"

Encoded:
0, 1, 0


---

### Decompression

- Use reverse dictionary:
0 → "111111"


---

### Advantages

- Extremely fast comparisons
- Minimal storage
- Simple decoding

---

## 4. Performance Comparison

### Query: Total Sales

| Method                  | Time (ms) | Why |
|--------------------------|----------|-----|
| No Compression           | 7.79     | Full string comparisons |
| Huffman Encoding         | 2.24     | Smaller data, but decode overhead |
| Byte Dictionary Encoding | 0.71     | Fast integer comparisons |

---

### Key Insight

> Compression improves performance **not just by shrinking data**, but by making comparisons faster.

---

## 5. Applying Compression to Weather Dataset

### Dataset Columns
- `timestamp`
- `temperature`

---

## 5.1 Timestamp Compression → Delta Encoding

### Approach
- Store:
- First timestamp (full value)
- Subsequent values as differences

### Why It Works
- Timestamps increase incrementally
- Deltas are small → fewer bits

---

## 5.2 Temperature Compression (2 Steps)

### Step 1: Range Reduction

- Subtract **minimum temperature**
- Shrinks value range

Example:
Original: 35, 40, 45
Min = 35
Adjusted: 0, 5, 10


---

### Step 2: Bit Packing

- Store adjusted values using minimal bits
- Example:
  - Small range → use **4 bits per value**

---

## 6. Storage Strategy

- Store columns separately as binary files:
  - Timestamps → delta encoded
  - Temperatures → bit packed

---

## 7. Query Execution with Compression

### Reconstruction Process

#### Timestamps
- Rebuild using:
  - Previous value + delta

#### Temperatures
- Extract values using:
  - Bitwise operations
  - Add back minimum value

---

## 8. Performance Comparison (Storage Formats)

| Storage Type              | Query Time (ms) | Reason |
|---------------------------|----------------|--------|
| Row Storage               | 17.57          | Reads all columns |
| Columnar Storage          | 5.13           | Reads only needed columns |
| Compressed Columnar       | 4.08           | Reads fewer bytes + efficient decompression |

---

## 9. Key Insights

### Why Compression + Columnar Wins

- Less data read from disk
- Better cache utilization
- Faster comparisons (especially dictionary encoding)

---

## 10. Mental Model (Exam Ready)

- **Huffman → optimize by frequency**
- **Dictionary → replace with small IDs**
- **Delta → store differences**
- **Bit Packing → minimize bits used**

---

## 11. Big Picture Takeaway

> Columnar storage + compression = maximum efficiency for analytical workloads

---

## 12. Final Recap

- Compression:
  - Saves storage
  - Reduces I/O
  - Speeds up queries
- Different algorithms work best for different data types
- Combining compression with columnar storage gives **compounding performance gains**

---

## 13. One-Line Summary

Compression techniques (Huffman, dictionary, delta, bit packing) combined with columnar storage significantly improve query performance by reducing data size and optimizing access patterns.

# Vectorized Execution & SIMD in Database Systems

## 1. Overview

This lesson explains how **vectorized (batch) processing** improves performance over **tuple-at-a-time execution**, and how **SIMD instructions** enable this speedup.

---

## 2. Tuple-at-a-Time Processing (Scalar Execution)

### How It Works
- Processes **one tuple (row)** at a time
- Each tuple goes through:
  - Fetch → Decode → Execute

---

### Problems

1. **High Function Call Overhead**
   - Operators called per tuple

2. **Poor CPU Utilization**
   - CPU can't leverage parallelism

3. **Pipeline Stalls**
   - Frequent branching
   - Instruction overhead

4. **Cache Inefficiency**
   - Non-contiguous memory access

---

## 3. Vector-at-a-Time Processing

### Core Idea
> Process **batches of tuples** instead of one at a time

---

### Benefits

- Fewer function calls (per batch, not per tuple)
- Better CPU utilization
- Reduced overhead
- Matches modern CPU architecture

---

## 4. SIMD (Single Instruction, Multiple Data)

### Definition
> A CPU executes **one instruction on multiple data elements simultaneously**

---

### What SIMD Enables

- Parallel processing of arrays
- High throughput for repetitive operations

---

### Common Database Use Cases

- Filtering (e.g., WHERE conditions)
- Aggregations (SUM, AVG)
- Decoding compressed data (bit-packed columns)

---

## 5. Core SIMD Operations

1. **Load**
   - Bring multiple values into a SIMD register

2. **Compare**
   - Apply conditions across all values

3. **Masking**
   - Identify matching elements

4. **Reduction**
   - Combine results (e.g., sum)

---

## 6. Example: Filtering Timestamps

### Goal
Find timestamps:
20 ≤ timestamp ≤ 30


---

### SIMD Steps
1. Load 4 values:
[18, 40, 25, 15]


2. Compare ≥ 20:
[false, true, true, false]


3. Compare ≤ 30:
[true, false, true, true]


4. Combine (AND):
[false, false, true, false]


→ Result: **25**

---

### Key Insight
> SIMD replaces loops with **parallel comparisons**

---

## 7. Performance Improvement

### Scalar Execution

- Processes **n elements**
- Requires **n instructions**

---

### SIMD Execution

- Vector width = **w**
- Requires:
n / w instructions


---

### Result

- Fewer instructions
- Less decoding overhead
- Faster execution

---

## 8. Memory & Cache Efficiency

### Why SIMD Works Better with Columnar Storage

- Data stored in **contiguous arrays**
- Matches CPU cache line size
- Efficient memory access

---

### Row Storage Problem

- Scattered memory access
- Poor cache utilization

---

### Key Insight

> Columnar + SIMD = optimal memory performance

---

## 9. Reduced Branching

### Scalar Execution

- Frequent `if` statements
- Branch mispredictions → pipeline stalls

---

### SIMD Execution

- Uses **masks instead of branches**
- Keeps CPU pipeline full

---

## 10. Historical Evolution of SIMD

### 1970s – Early Supercomputers
- Example: **ILIAC IV**
- Used for scientific computing

---

### 1990s – Multimedia Era

- :contentReference[oaicite:0]{index=0} MMX (1996)
- Parallel integer operations
- Use case: image processing

---

### 2000s – General-Purpose CPUs

- SSE (Streaming SIMD Extensions)
- :contentReference[oaicite:1]{index=1} NEON
- 128-bit vector processing
- Added floating-point support

---

### 2010+ – Modern SIMD

- AVX, AVX-512 (512-bit registers)
- Massive parallelism
- Used in:
  - Analytics engines
  - Databases

---

### Modern Example

- :contentReference[oaicite:2]{index=2}
  - Designed for columnar + SIMD processing

---

## 11. Data Layout for SIMD

### Structure of Arrays (SoA)

Instead of:
[(timestamp, temp), (timestamp, temp)]


Use:
timestamps: [ ... ]
temperatures: [ ... ]


---

### Benefits

- Contiguous memory
- Efficient vector loads
- SIMD-friendly alignment

---

## 12. Example: Sensor Data Generation

### Data Characteristics

- `timestamp`:
  - Incremental (uniform distribution)

- `temperature`:
  - Normal distribution

---

### Why This Matters

- Matches real-world patterns
- Ideal for:
  - Compression
  - Vectorized processing

---

## 13. Key Takeaways

- **Tuple-at-a-time = simple but slow**
- **Vectorized execution = faster, scalable**
- **SIMD = hardware-level parallelism**
- **Columnar storage amplifies SIMD benefits**

---

## 14. Mental Model (Exam Ready)

- **Scalar = 1 operation → 1 value**
- **SIMD = 1 operation → many values**
- **Vectorization = batching work**
- **Columnar = contiguous data → SIMD-friendly**

---

## 15. One-Line Summary

Vectorized execution using SIMD dramatically improves database performance by processing multiple data elements per instruction and aligning with modern CPU architecture.

# SIMD Vectorized Query Processing (Sensor Data Example)

## 1. Problem Overview

We aim to compute the **average temperature over a timestamp range** from sensor data.

### Execution Models

#### Scalar (tuple-at-a-time)
- Process each `(timestamp, temperature)` pair sequentially
- For each tuple:
  - Check if timestamp is within range
  - If yes, add temperature to running sum
- High branching + low CPU utilization

#### SIMD (vector-at-a-time)
- Process multiple tuples per instruction
- Uses vector registers (e.g., 4-wide SIMD)
- Enables data-level parallelism

---

## 2. SIMD Core Idea

SIMD = Single Instruction, Multiple Data

Example (4-wide SIMD):
- 4 × int32 timestamps per register
- 4 × float32 temperatures per register

This allows **4 comparisons or arithmetic operations at once**.

---

## 3. Loading Data into SIMD Registers

We load data in batches:

- `V_load_int32` → loads 4 timestamps
- `V_load_f32` → loads 4 temperature values

Example:
timestamp_vec = [t1, t2, t3, t4]
temp_vec = [v1, v2, v3, v4]


---

## 4. Range Filtering with SIMD Masks

We compute a boolean mask in parallel.

### Step 1: Lower bound check
mask1 = (timestamp_vec >= start_time)


### Step 2: Upper bound check
mask2 = (timestamp_vec <= end_time)


### Step 3: Combine masks
final_mask = mask1 AND mask2


### Result
- `1` → keep element
- `0` → discard element

No branching required.

---

## 5. Applying Mask to Temperatures

### Step 1: Convert mask to float vector

### Step 2: Element-wise multiplication
filtered_temps = temp_vec * mask_vec


Effect:
- In-range values remain unchanged
- Out-of-range values become 0

This enables **branchless conditional selection**.

---

## 6. Accumulation (Vector Sum)

We accumulate results:
sum_vec += filtered_temps


(using SIMD add instruction like `Vadd_f32`)

---

## 7. Horizontal Reduction

After processing all SIMD batches:

Convert vector → scalar:

- Sum all lanes in SIMD register
- Produces final scalar sum

Example concept:
sum = lane0 + lane1 + lane2 + lane3


---

## 8. Handling Remaining Elements (Tail Loop)

If N is not divisible by SIMD width:

- Process remaining elements using scalar loop
- Ensures correctness

Pattern:
> Vector loop + scalar cleanup

---

## 9. SIMD in Hash Join Processing

SIMD can accelerate hash joins:

### Steps:

1. Load probe keys into SIMD registers
2. Compute hash in parallel
3. Perform table lookup (gather operation)
4. Compare keys with stored values using SIMD compare

### Key Challenge:
- Non-contiguous memory access

### Optimization:
- Better memory layout improves gather efficiency

---

## 10. SIMD Instruction Naming

SIMD instruction names encode:

### Format:
operation + vector width + data type


### Examples:

- `vld1q_s32`
  - load 4 × int32 values

- `vaddq_f32`
  - add 4 × float32 values

### Meaning:
- `q` = 128-bit (quad word, typically 4 lanes)
- `s32` = signed 32-bit integer
- `f32` = 32-bit float

### Modern extensions:
- 128-bit (baseline SIMD)
- 256-bit (AVX)
- 512-bit (AVX-512)

---

## 11. Key Takeaways

### Scalar execution
- One tuple per instruction
- Branch-heavy
- Inefficient for large datasets

### SIMD execution
- Multiple tuples per instruction
- Branchless filtering via masks
- Efficient CPU and cache usage

---

## 12. Summary

SIMD vectorized execution:

- Processes multiple tuples per instruction
- Uses masks instead of branches
- Uses vectorized arithmetic for filtering + aggregation
- Requires scalar fallback for tail elements
- Extends to complex operations like hash joins

---

## Core Insight

> SIMD improves query performance by converting control flow (branches) into data flow (vector operations).