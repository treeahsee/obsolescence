# Advanced Query Operators
- The goal is to evolve database predicates from simple, single-column checks to a robust system capable of handling complex query conditions involving multiple columns and logical operators (e.g., Age < 30 AND Salary > 1000).

## More Complex Predicates
``` SQL
SELECT
FROM employees
WHERE salary > 1000 AND age < 30;
```

``` C++
class IPredicate {
    public:
        virtual ~IPredicate() = default;
        virtual bool
        check(const std :: vector<std :: unique_ptr<Field>> &tupleFields) const = 0;
```

### 1. The IPredicate Interface
- To standardize predicate handling, an abstract base class (interface) is introduced.

- Purpose: Provides a uniform way to interact with any condition, whether simple or nested.

- The check() Method: The primary requirement for any derived class. It evaluates a tuple against defined conditions and returns a boolean.

### 2. Simple Predicate Class
- Handles direct comparisons for basic filtering.

- Operators: Supports =, >, !=, etc.

- Operands: Can be direct values (constants like $1000) or indirect references (pointers to specific fields within a tuple).

- Example: Comparing the "Salary" column against a constant value.

### 3. Complex Predicate Class
- Enables the combination of multiple conditions using logical operators.

- Logic Operators: Supports AND and OR enums.

- Composition: Uses a vector of unique pointers to IPredicate objects. This allows for recursive nesting, where a complex predicate can contain both simple predicates and other complex predicates.

- Evaluation Logic:
    - AND: Returns true only if every child predicate evaluates to true.

     - OR: Returns true if at least one child predicate evaluates to true.

``` C++
class ComplexPredicate : public IPredicate {
    std :: vector<std :: unique_ptr<IPredicate>> predicates;
    LogicOperator logic_operator;

    bool
    check(const std :: vector<std :: unique_ptr<Field>> &tupleFields) const override {
    // Evaluate all contained predicates based on the logic operator (AND, OR)
    }
};
```

``` C++
    if (logic_operator == AND) {
        for (const auto &pred : predicates) {
            if (!pred->check(tupleFields)) {
                return false; // All must pass
                }
                return true;
            }
            else if (logic_operator == OR) {
                for (const auto &pred : predicates) {
                    if (pred->check(tupleFields)) {
                        return true; // Any one can pass
                    }
                return false;
            }
        }
    }
```

### 4. Construction Workflow
- Define Simple Units: Create individual SimplePredicate objects for each basic condition.

- Aggregate: Group those units into a ComplexPredicate using the desired logical operator (AND/OR).

- Execution: Pass the final ComplexPredicate to the Select Operator, which uses the check() method to filter tuples during query processing.

### Summary Recap
- By using the Interface pattern, the system treats simple and complex conditions identically. This architecture allows for the construction of arbitrarily deep and complex query logic, enabling the database to process sophisticated filters across multiple data fields.

## Hash Aggregation Operator

### 1. Core Idea
- Aggregation queries compute summaries over groups of data.

- Mental model:
  - Hash aggregation = hashmap
    - Key → group 
    - Value → running aggregate 

``` SQL
SELECT Category_ID,
SUM(Profit) AS Total_Profit
FROM Sales_Data
WHERE MONTH(DATE) = 'JUNE'
GROUP BY Category_ID;
```

``` C++
class HashAggregationOperator : public UnaryOperator {
private:
    std :: vector<size_t> group_by_attrs;
    std :: vector<AggrFunc> aggr_funcs;
    std :: vector<Tuple> output_tuples;
...
};
```

---

### 2. Query Breakdown
- Filtering
  - WHERE clause (e.g., month = June)

- Grouping
  - GROUP BY category_id

- Aggregation
  - SUM(profit), COUNT(*), etc.

---

### 3. Hash Aggregation Operator

#### Components
- Group-by attributes
  - Columns that define the key

- Aggregate functions
  - Operations applied per group

- Hash table
  - Key → group values
  - Value → aggregate result

---

### 4. Execution Methods
- `open()`
  - Initialize empty hash table


``` C++
void HashAggregationOperator :: open() {
    std :: unordered_map<std :: vector<Field>, std :: vector<Field>, FieldVectorHasher>
    hash_table;
    // Build the hash table with the aggregate tuples
    }
    
    bool HashAggregationOperator :: next() {
    // Iterate over the aggregate tuples in the hash table
}
```

- `update_aggregate()`
  - If key exists → update value
  - Else → create new entry
  - Ensures type correctness

``` C++
Field updateAggregate(const AggrFunc &aggrFunc, const Field &currentAggr,
    const Field &newValue) {
    // Check for type consistency
    if (currentAggr.getType() != newValue.getType()) {
        throw std :: runtime_error("Mismatched Field types in aggregation.");

    // Perform the specified aggregation operation
        switch (aggrFunc.func) {
            case AggrFuncType :: SUM:
            return Field(currentAggr.aslnt() + newValue.asInt());
            // other cases ...
        }
    }
}
```

- `next()`
  - Iterate over final aggregated results

---

### 5. Execution Flow
- Initialize hash table

- For each tuple:
  - Compute group key
  - Hash it
  - Update or insert aggregate

- After all tuples:
  - Emit final grouped results

---

### 6. Multi-Column Grouping
- Group key becomes a vector
  - e.g., `[category_id, month]`

---

### 7. Hashing Strategy
- Start with hash = 0

- For each field:
  - Convert to string
  - Compute hash

- Combine using:
  - XOR
  - Bit shifting

- Goal:
  - Even distribution
  - Fewer collisions
  - Fast lookup/update

---

### 8. Key Takeaways
- Avoids sorting → faster for large datasets

- Works well for:
  - GROUP BY
  - Real-time aggregation

- Core strength:
  - Incremental updates via hash table

## Relational Operator Framework

### 1. Core Idea
- Move from **hard-coded query functions** → **modular operator pipelines**
- Queries are executed as a sequence of operators instead of one monolithic function

- Mental model:
  - Query execution = pipeline of operators
    - Each operator does one job
    - Output of one → input to next

---

### 2. Problem with Hard-Coded Queries
- Example query:
  - Filter rows
  - Group by column
  - Aggregate (SUM)

- Hard-coded approach:
  - Scan table
  - Build index
  - Filter tuples
  - Group data
  - Aggregate values

- Issues:
  - Not flexible
  - Hard to extend for new queries
  - Logic tightly coupled

---

### 3. Operator Pipeline Approach

- Break query into composable operators:

  1. **Scan Operator**
     - Reads tuples from storage (via buffer manager)

  2. **Select Operator**
     - Filters tuples using predicate

  3. **Hash Aggregation Operator**
     - Groups by column(s)
     - Computes aggregates (e.g., SUM)

- Data flows step-by-step through pipeline

---

### 4. Execution Flow

- `executeQuery()` builds the pipeline:

  - Initialize **Scan**
  - Pipe into **Select**
  - Pipe into **Hash Aggregation**

- Each operator:
  - Takes input
  - Produces output
  - Passes to next operator

``` C++
void executeQuery() {

    SelectOperator selectOperator(scanOperator, std :: move(complexPredicate));
    std :: vector<AggrFunc> aggrFuncs{{AggrFuncType :: SUM, 1}};
    std :: vector<size_t> groupByAttrs{0};
    HashAggregationOperator hashAggregationOperator(selectOperator, groupByAttrs,
    aggrFuncs);
    hashAggregationOperator.open();
while (hashAggregationOperator.next()) {
    const auto &aggregatedFields = hashAggregationOperator.getOutput();
    // Output key and aggregated value

    hashAggregationOperator.close();

}
```

---

### 5. Why This Design is Better

- Modular → easy to modify
- Reusable operators
- Enables:
  - Query optimization
  - Automatic query planning
  - Converting SQL → execution plan

---

## Unix Pipeline Analogy

### 6. Shared Philosophy
- Both developed in the 1970s
- Focus:
  - Simplicity
  - Composability
  - Avoid monolithic systems

- Key idea:
  - “Do one thing well”

---

### 7. Operator vs Unix Tools

- Unix tools:
  - `grep` → filter text
- Relational operators:
  - `SELECT` → filter rows

→ Same concept, different domain

---

### 8. Example Unix Pipeline and SQL

```bash
cat file.txt | grep error | sort | uniq -c

```

``` SQL
SELECT ErrorMessage, COUNT(*) AS ErrorCount
FROM LogTable
WHERE ErrorMessage LIKE '%error%'
GROUP BY ErrorMessage
ORDER BY ErrorMessage;
```

## SQL DDL & Create Index Operator
## 1. SQL Sub-languages
SQL has two main components:

### Data Definition Language (DDL)
- Defines or modifies database structure (schema)
- Operations:
  - `CREATE`
  - `ALTER`
  - `DROP`
- Works on database objects:
  - Tables (store tuples)
  - Indices (speed up retrieval)
  - Views (virtual tables for simplified queries)

### Data Manipulation Language (DML)
- Queries and modifies data (tuples) inside tables

---

## 2. Core Database Objects
- **Tables**: primary storage for tuples
- **Indices**: derived structures to speed up lookup
- **Views**: virtual tables that simplify queries and may improve performance

---

## 3. Create Index Operator

### Purpose
- Builds an index on a column or set of columns to speed up data retrieval

### Key Components
- **Column offset (attribute index)**: identifies which column to index
- **Index structure**: typically a hash index in this example

### Example
- Index built on **salary column (offset = 1)**
- Key = salary  
- Value = employee ID  

---

## 4. Execution Flow (BusDB Example)

### `open()` method
- Iterates over all tuples from child operator (e.g., scan)
- For each tuple:
  - Extract key using column offset
  - Insert `(key → value)` into hash index
- Builds the entire index here

### `next()` method
- Empty (no incremental processing needed)

### `get_index()` method
- Returns the constructed hash index
- Allows other operators (e.g., SELECT) to use it

---

## 5. How It’s Used
- `Scan` operator feeds data into `CreateIndex`
- Column offset specifies indexed attribute
- Other operators leverage the built index for faster filtering

---

## 6. Key Takeaway
- The operator framework unifies:
  - DDL (schema/index creation)
  - DML (query execution)
- Enables modular separation of:
  - Scanning data
  - Building indexes
  - Query processing