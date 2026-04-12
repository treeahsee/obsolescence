# Query Execution
- Operators are the fundamentals building blocks of query execution
    - Scan operator
    - Abstract Base Class
    - Predicate
    - Select Operator

## Modular Query Execution
- Modular query execution give us flexibility and isolation
- Scan Operator
- Select Operator
- Group by Operator

## Operator Class
``` C++
class Operator {
    public:
        virtual ~Operator() = default;
        virtual void open() = 0;
        virtual bool next() = 0;
        virtual std :: vector<std :: unique_ptr<Field>>getOutput() = 0;
        virtual void close() = 0;
};
```
- open initializes the operator
- next progresses to next tuble
- getOutput returns the tuple
- close cleans up resources

## Scan Operator
``` C++
class ScanOperator : public Operator {
    private:
        size_t currentPageIndex = 0;
        std :: unique_ptr<SlottedPage> currentPage;
        size_t currentSlotIndex = 0;
        std :: unique_ptr<Tuple> currentTuple;
        BufferManager &bufferManager;
        // Initialization and tuple management code here ...
};
```
- open preepares first page for tuple extraction

``` C++
void ScanOperator :: open() {
    currentPagelndex = 0; // Reset page index
    currentSlotIndex = 0; // Reset slot index
    loadNextTuple();
```

- next sequentially moves through slots, loading tuples

``` C++
bool ScanOperator :: next() {
    if (!currentPage)
        return false; // No more pages available
    loadNextTuple(); // load next tuple from page into currentTuple
    return currentTuple != nullptr;
}
```

``` C++
void loadNextTuple() {
while (currentPagelndex < bufferManager.getNumPages()) {
Slot *slot_array = reinterpret_cast<Slot *>(currentPage->page_data.get());
while (currentSlotIndex < MAX_SLOTS) {
// Extract tuple from current slot

currentSlotIndex++;

}
currentPageIndex++; currentSlotIndex = 0;

// If we've reached here, no more tuples are available
currentTuple.reset();

}
```

- getOutput returns fields of current tuple

``` C++
std :: vector<std :: unique_ptr<Field>> getOutput() override {
    if (currentTuple) {
        return std :: move(currentTuple->fields);
    }
    return {}; // Return an empty vector if no tuple is available
}
```

- close release resources and perform cleanup

``` C++
void ScanOperator :: close() override {
    currentPage.reset();
    currentTuple.reset();
}
```

## Abstract Base Class
``` C++
class Shape {
    protected:
    float x, y; // Location of the shape
    public:
    Shape(float x, float y, string color) : x(x), y(y), color(color) {}
    // Pure virtual function for drawing the shape
    virtual void draw() const = 0;
    // Virtual function for moving the shape
    virtual void move(float newX, float newY) {
    x = newX;

    y = newY;
    cout << "Moved to(" << x << "," << y << ")." << endl;

    }
};
```

## Derived Class
``` C++
class Circle : public Shape {
    private:
        float radius;
    public:
        // Constructor for Circle initializes Shape and radius
        Circle(float x, float y, string color, float radius)
        : Shape(x, y, color), radius(radius) {}
        // Implementation of the pure virtual draw function
        void draw() const override {
        cout << "Drawing a circle at (" << x << ", " << y << ") with radius "
        << radius << " and color " << color << "." << endl;
}
```

## Inheritance
- Derived classes cna override base class
- Polymorphism dervived classes can override base class methods with different implementations
- polymorphic example
    ``` C++
    int main() {
        Shape *shapes[2];
        shapes[0] = new Circle();
        shapes[1] = new Square();

    for (int i = 0; i < 2; ++i) {
        shapes[i]->draw(); // Polymorphic call}
        return 0;

    }
    ```
## Predicate
``` SQL
SELECT
    FROM employees
WHERE salary > 1000; -- Predicate
```
- predicate is 3 components
    - left and right fields or the fields that are being compared and the predicate such as greater than

``` C++
class Predicate {
    public:
        std :: unique_ptr<Field> left_field;
        std :: unique_ptr<Field> right_field;
        PredicateType predicate_type;
```

``` C++
enum class PredicateType {
    EQ, // Equal
    NE, // Not Equal
    GT, // Greater Than
    GE, // Greater Than or Equal
    LT, // Less Than
    LE // Less Than or Equal
};
```

``` C++
bool checkPredicate() const {
    switch (left_field->getType()) {
    case INT: {
        int left_val = left_field->aslnt();
        int right_val = right_field->aslnt();
        return compare(left_val, right_val);
}
```

``` C++
template <typename T>
bool compare(const T &left_val, const T &right_val) const {
    switch (predicate_type) {
    case PredicateType :: EQ: return left_val == right_val;
    case PredicateType :: NE: return left_val != right_val;
    case PredicateType :: GT: return left_val > right_val;
    case PredicateType :: GE: return left_val >= right_val;
    case PredicateType :: LT: return left_val < right_val;
    case PredicateType :: LE: return left_val <= right_val;
    default: std :: cerr << "Invalid predicate type\n"; return false;
}
```

``` C++
class Predicate {
public:
    enum OperandType { DIRECT, INDIRECT };
    struct Operand {
    std :: unique_ptr<Field> directValue;
    size_t index;
    OperandType type;
    Operand(std :: unique_ptr<Field> value)
        : directValue(std :: move(value)), type(DIRECT) {}
    Operand(size_t idx) : index(idx), type(INDIRECT) {}

};
```
## Select Operator
``` C++
class SelectOperator : public UnaryOperator {
    private:
        Predicate predicate; // Condition to evaluate on each tuple
        bool has_next; // Indicator if there's a next tuple satisfying the predicate
        std :: vector<std :: unique_ptr<Field>>currentOutput; // Current tuple
};
```

``` C++
bool next() override {
    while (input->next()) {
        const auto &output = input->getOutput();
            if (predicate.checkPredicate(output)) {
            currentOutput = duplicateFields(output);
            has_next = true;
            return true;
            }
        has_next = false;
        currentOutput.clear();
        return false;
}
```