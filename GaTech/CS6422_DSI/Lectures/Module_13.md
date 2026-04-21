# SQL Query Parsing with Regex

## Overview
- Transition from **hard-coded query execution** → **dynamic query execution using query strings**
- Queries are parsed at runtime instead of being implemented as fixed functions
- Enables flexibility for:
  - Simple filters (`SELECT ... WHERE`)
  - Aggregations (`SUM`)
  - Grouping (`GROUP BY`)

---

## Key Concept: Query Parsing
- **Query parsing** = extracting structured information from a query string
- Implemented using **Regular Expressions (Regex)**
- Extracted data is stored in a structured format (`QueryComponents`)

---

## Regular Expressions (Regex)
- A **pattern matching tool** used to extract parts of a string
- Used to identify:
  - Selected columns
  - Aggregations (e.g., `SUM`)
  - Grouping clauses
  - Filter conditions (`VAR` clause)

### Example: SUM Regex
- Pattern matches: `sum{3}`
- Components:
  - `sum` → literal match
  - `{}` → literal braces
  - `\d+` → one or more digits
- Extracts:
  - Column index (e.g., `3`)

---

## QueryComponents Struct

Stores parsed query details:

- `selectAttributes`
  - Vector of column indices to retrieve

- `hasSum`
  - Boolean indicating if `SUM` is present

- `sumAttributeIndex`
  - Column index to apply sum

- `hasGroupBy`
  - Boolean indicating presence of grouping

- `groupByAttributeIndex`
  - Column index used for grouping

- `varCondition`
  - Stores filter conditions (bounds, predicates)

---

## Regex Patterns Used
1. **SELECT regex**
   - Extracts selected column indices

2. **SUM regex**
   - Detects aggregation and extracts column index

3. **GROUP BY regex**
   - Identifies grouping and attribute index

4. **VAR regex**
   - Extracts filter conditions (complex predicates)

---

## Query Execution Flow

### 1. Parse Query
- Convert query string → `QueryComponents`

### 2. Build Operator Chain
- Start with:
  - **Scan Operator** (reads table data)

- Add dynamically:
  - **Select Operator**
    - Applies filter predicates
  - **Hash Aggregation Operator**
    - Handles `SUM` and `GROUP BY`

---

## Predicate Construction
- For `VAR` clause:
  - Create **simple predicates** for each condition
  - Combine using **logical AND**
  - Embed into **Select Operator**

---

## Aggregation Handling
- If `SUM` or `GROUP BY` exists:
  - Configure aggregation function
  - Instantiate **Hash Aggregation Operator**
  - Apply grouping + aggregation logic

---

## Execution Model
- Execution starts at **root operator**
- Flow:
  1. Open root operator
  2. Fetch tuples
  3. Print results
- Operators call children recursively:
  - Root → Select → Scan

---

## Example Pipeline

Query:
- Filter: `column1 BETWEEN 2 AND 6`
- Group: `column2`
- Aggregate: `SUM(column1)`

Operator chain:
Hash Aggregation
↑
Select (filter column1)
↑
Scan (read table)


---

## Debugging
- Use **Pretty Print Function**
  - Displays parsed components
  - Helps verify correctness

---

## Key Takeaways
- Regex enables **flexible query parsing**
- Queries are converted into **structured components**
- Components dynamically map to **operator pipelines**
- Improves scalability and flexibility of query execution


# Query Interpretation vs Query Compilation

## Overview
- Current approach: **Query Interpretation**
- Alternative approach: **Query Compilation**
- Goal: Improve performance by reducing execution overhead

---

## Query Interpretation

### Definition
- Executes queries by:
  1. Parsing SQL query
  2. Building an **operator execution plan**
  3. Running the plan

### Execution Flow
1. `parseQuery()`
   - Extracts query components

2. `executeQuery()`
   - Builds operator chain (Scan → Select → Aggregation)
   - Executes operators

---

## Limitations of Query Interpretation

### 1. Function Call Overhead
- Each tuple passes through multiple operators
- Many function calls per tuple
- Expensive at scale (millions of tuples)

---

### 2. Deserialization Overhead
- Tuples are decoded using a **generic schema**
- Less efficient than specialized parsing

---

### 3. Dynamic Dispatch (Polymorphism)
- Operators use abstract classes + pointers
- Requires **virtual table (vtable) lookup**
- Adds overhead for every function call

---

### 4. Memory Overhead
- Frequent creation/destruction of objects
  - e.g., `unique_ptr` for fields
- Causes additional CPU + memory pressure

---

## Query Compilation

### Definition
- Converts SQL query → **hard-coded C function**
- Then compiled → **assembly (machine code)**

---

## How Query Compilation Works

- Replaces operator pipeline with **single compiled function**
- Directly:
  - Scans pages via buffer manager
  - Iterates through tuples
  - Processes raw data

---

## Key Execution Differences

### Instead of:
- Operator chain (Scan → Select → Aggregate)

### It does:
- Everything inline in one function:
  - Scan data
  - Deserialize fields manually
  - Apply filters
  - Perform aggregation
  - Output results

---

## Example Execution Steps

1. Read tuple as raw bytes (`char*`)
2. Deserialize field directly → integer
3. Apply filter (e.g., bounds check)
4. Update group aggregation (e.g., sum)
5. Output final grouped results

---

## Advantages of Query Compilation

### 1. Eliminates Interpretation Overhead
- No operator chaining
- No repeated function calls
- No virtual dispatch

---

### 2. Runs on Bare Metal
- Direct execution of machine code
- Much faster execution

---

### Performance Example
- Compiled query can be **~30× faster** than interpreted query

---

## Drawbacks of Query Compilation

### 1. Compilation Time
- Converting query → machine code takes time
- For short queries:
  - Compilation cost may outweigh benefits

---

### 2. Reduced Flexibility
- Hard-coded logic tied to:
  - Query structure
  - Table schema

- Changes require:
  - **Recompilation**

---

### 3. Recompilation Cost
- Frequent schema/query changes → repeated compilation
- May negate performance gains

---

## Trade-Off Summary

| Aspect              | Interpretation        | Compilation           |
|--------------------|----------------------|----------------------|
| Flexibility        | High                 | Low                  |
| Performance        | Slower               | Much faster          |
| Setup Cost         | Low                  | High (compile time)  |
| Adaptability       | Easy                 | Requires recompiling |

---

## Key Takeaways

- **Query Interpretation**
  - Flexible but slower due to runtime overhead

- **Query Compilation**
  - Faster by removing abstraction layers
  - Executes directly as machine code

- Best approach depends on:
  - Query frequency
  - Execution time
  - Need for flexibility vs performance