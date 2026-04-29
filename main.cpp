#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <stdexcept>
#include <stack>
#include <string>
#include <vector>

class Goods {
private:
    std::string name_;
    std::string type_;
    double weightKg_;

    static void validateText(const std::string& value, const std::string& field) {
        if (value.empty()) {
            throw std::invalid_argument(field + " must not be empty.");
        }
    }

    static void validateWeight(double weightKg) {
        if (weightKg <= 0.0) {
            throw std::invalid_argument("Weight must be greater than 0 kg.");
        }
    }

public:
    Goods(std::string name, std::string type, double weightKg)
        : name_(std::move(name)), type_(std::move(type)), weightKg_(weightKg) {
        validateText(name_, "Name");
        validateText(type_, "Type");
        validateWeight(weightKg_);
    }

    const std::string& name() const { return name_; }
    const std::string& type() const { return type_; }
    double weightKg() const { return weightKg_; }

    void updateWeight(double newWeightKg) {
        validateWeight(newWeightKg);
        weightKg_ = newWeightKg;
    }
};

std::ostream& operator<<(std::ostream& os, const Goods& goods) {
    os << std::left << std::setw(18) << goods.name()
       << std::setw(14) << goods.type()
       << std::right << std::fixed << std::setprecision(1)
       << goods.weightKg() << " kg";
    return os;
}

class AvlInventory {
private:
    struct Node {
        Goods goods;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        int height;

        explicit Node(Goods item) : goods(std::move(item)), height(1) {}
    };

    std::unique_ptr<Node> root_;
    int nodeCount_ = 0;

    static int height(const std::unique_ptr<Node>& node) {
        return node ? node->height : 0;
    }

    static int balanceFactor(const std::unique_ptr<Node>& node) {
        return node ? height(node->left) - height(node->right) : 0;
    }

    static void updateHeight(std::unique_ptr<Node>& node) {
        node->height = 1 + std::max(height(node->left), height(node->right));
    }

    static std::unique_ptr<Node> rotateRight(std::unique_ptr<Node> y) {
        std::unique_ptr<Node> x = std::move(y->left);
        std::unique_ptr<Node> subtree = std::move(x->right);

        x->right = std::move(y);
        x->right->left = std::move(subtree);

        updateHeight(x->right);
        updateHeight(x);
        return x;
    }

    static std::unique_ptr<Node> rotateLeft(std::unique_ptr<Node> x) {
        std::unique_ptr<Node> y = std::move(x->right);
        std::unique_ptr<Node> subtree = std::move(y->left);

        y->left = std::move(x);
        y->left->right = std::move(subtree);

        updateHeight(y->left);
        updateHeight(y);
        return y;
    }

    static std::unique_ptr<Node> rebalance(std::unique_ptr<Node> node) {
        updateHeight(node);
        const int balance = balanceFactor(node);

        if (balance > 1) {
            if (balanceFactor(node->left) < 0) {
                node->left = rotateLeft(std::move(node->left));
            }
            return rotateRight(std::move(node));
        }

        if (balance < -1) {
            if (balanceFactor(node->right) > 0) {
                node->right = rotateRight(std::move(node->right));
            }
            return rotateLeft(std::move(node));
        }

        return node;
    }

    std::unique_ptr<Node> insert(std::unique_ptr<Node> node, Goods item, bool& inserted) {
        if (!node) {
            inserted = true;
            ++nodeCount_;
            return std::make_unique<Node>(std::move(item));
        }

        if (item.name() < node->goods.name()) {
            node->left = insert(std::move(node->left), std::move(item), inserted);
        } else if (item.name() > node->goods.name()) {
            node->right = insert(std::move(node->right), std::move(item), inserted);
        } else {
            throw std::invalid_argument("Duplicate goods name rejected: " + item.name());
        }

        return rebalance(std::move(node));
    }

    const Goods* find(const std::unique_ptr<Node>& node, const std::string& name) const {
        if (!node) {
            return nullptr;
        }
        if (name == node->goods.name()) {
            return &node->goods;
        }
        if (name < node->goods.name()) {
            return find(node->left, name);
        }
        return find(node->right, name);
    }

    Goods* findMutable(const std::unique_ptr<Node>& node, const std::string& name) {
        if (!node) {
            return nullptr;
        }
        if (name == node->goods.name()) {
            return &node->goods;
        }
        if (name < node->goods.name()) {
            return findMutable(node->left, name);
        }
        return findMutable(node->right, name);
    }

    void inorder(const std::unique_ptr<Node>& node, std::vector<Goods>& out) const {
        if (!node) {
            return;
        }
        inorder(node->left, out);
        out.push_back(node->goods);
        inorder(node->right, out);
    }

    static bool isBalanced(const std::unique_ptr<Node>& node) {
        if (!node) {
            return true;
        }
        const int balance = balanceFactor(node);
        return balance >= -1 && balance <= 1 &&
               isBalanced(node->left) &&
               isBalanced(node->right);
    }

public:
    bool add(Goods item) {
        bool inserted = false;
        root_ = insert(std::move(root_), std::move(item), inserted);
        return inserted;
    }

    const Goods& get(const std::string& name) const {
        if (name.empty()) {
            throw std::invalid_argument("Search name must not be empty.");
        }
        const Goods* result = find(root_, name);
        if (!result) {
            throw std::out_of_range("Goods not found: " + name);
        }
        return *result;
    }

    void updateWeight(const std::string& name, double newWeightKg) {
        Goods* result = findMutable(root_, name);
        if (!result) {
            throw std::out_of_range("Cannot update missing goods: " + name);
        }
        result->updateWeight(newWeightKg);
    }

    int size() const { return nodeCount_; }
    int height() const { return height(root_); }
    bool balanced() const { return isBalanced(root_); }

    std::vector<Goods> listByName() const {
        std::vector<Goods> items;
        inorder(root_, items);
        return items;
    }
};

struct SortMetrics {
    long long comparisons = 0;
    long long swaps = 0;
};

SortMetrics bubbleSortDescending(std::vector<double>& values) {
    SortMetrics metrics;
    const std::size_t n = values.size();

    for (std::size_t pass = 0; pass < n; ++pass) {
        bool swapped = false;
        for (std::size_t i = 0; i + 1 < n - pass; ++i) {
            ++metrics.comparisons;
            if (values[i] < values[i + 1]) {
                std::swap(values[i], values[i + 1]);
                ++metrics.swaps;
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
    return metrics;
}

int partitionDescending(std::vector<double>& values, int low, int high, SortMetrics& metrics) {
    const double pivot = values[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        ++metrics.comparisons;
        if (values[j] >= pivot) {
            ++i;
            std::swap(values[i], values[j]);
            ++metrics.swaps;
        }
    }

    std::swap(values[i + 1], values[high]);
    ++metrics.swaps;
    return i + 1;
}

void quickSortDescending(std::vector<double>& values, int low, int high, SortMetrics& metrics) {
    if (low < high) {
        const int pivot = partitionDescending(values, low, high, metrics);
        quickSortDescending(values, low, pivot - 1, metrics);
        quickSortDescending(values, pivot + 1, high, metrics);
    }
}

std::optional<Goods> linearSearchByName(const std::vector<Goods>& items, const std::string& name) {
    for (const Goods& item : items) {
        if (item.name() == name) {
            return item;
        }
    }
    return std::nullopt;
}

void printDivider(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

void demonstrateQueue() {
    printDivider("FIFO truck queue example");
    std::queue<std::string> loadingBay;
    loadingBay.push("Truck A - fresh food");
    loadingBay.push("Truck B - medicine");
    loadingBay.push("Truck C - electronics");

    while (!loadingBay.empty()) {
        std::cout << "Loading now: " << loadingBay.front() << '\n';
        loadingBay.pop();
    }
}

void demonstrateStack() {
    printDivider("LIFO stack example for process control");
    std::stack<std::string> callStack;
    callStack.push("main()");
    callStack.push("loadInventory()");
    callStack.push("insertAVLNode()");
    callStack.push("rebalance()");

    while (!callStack.empty()) {
        std::cout << "Returning from: " << callStack.top() << '\n';
        callStack.pop();
    }
}

void demonstrateSorting() {
    printDivider("Sorting 12 cargo weights descending");
    std::vector<double> weights{72.5, 18.0, 105.2, 41.7, 90.0, 12.4,
                                66.8, 33.3, 120.0, 58.9, 7.6, 84.1};
    std::vector<double> bubble = weights;
    std::vector<double> quick = weights;

    const SortMetrics bubbleMetrics = bubbleSortDescending(bubble);
    SortMetrics quickMetrics;
    quickSortDescending(quick, 0, static_cast<int>(quick.size()) - 1, quickMetrics);

    std::cout << "Bubble sort: ";
    for (double weight : bubble) {
        std::cout << weight << ' ';
    }
    std::cout << "\nBubble metrics: " << bubbleMetrics.comparisons
              << " comparisons, " << bubbleMetrics.swaps << " swaps\n";

    std::cout << "QuickSort:   ";
    for (double weight : quick) {
        std::cout << weight << ' ';
    }
    std::cout << "\nQuickSort metrics: " << quickMetrics.comparisons
              << " comparisons, " << quickMetrics.swaps << " swaps\n";
}

void demonstrateInventory() {
    printDivider("AVL warehouse inventory");
    AvlInventory inventory;

    const std::vector<Goods> sampleItems{
        {"Insulin Box", "Medicine", 12.0},
        {"Laptop Carton", "Electronics", 18.5},
        {"Rice Sack", "Food", 50.0},
        {"Steel Bolts", "Hardware", 27.4},
        {"Water Filter", "Appliance", 9.8},
        {"Oxygen Kit", "Medical", 16.1},
        {"Printer Ink", "Office", 4.2},
        {"Solar Lamp", "Electronics", 6.7},
        {"Blankets", "Relief", 22.0}
    };

    for (const Goods& item : sampleItems) {
        inventory.add(item);
    }

    std::cout << "Inventory count: " << inventory.size() << '\n';
    std::cout << "Tree height: " << inventory.height() << '\n';
    std::cout << "AVL balanced: " << (inventory.balanced() ? "yes" : "no") << "\n\n";

    for (const Goods& item : inventory.listByName()) {
        std::cout << item << '\n';
    }

    std::cout << "\nSearch result: " << inventory.get("Rice Sack") << '\n';
    inventory.updateWeight("Rice Sack", 48.5);
    std::cout << "After weight update: " << inventory.get("Rice Sack") << '\n';

    printDivider("Robust error handling");
    try {
        inventory.add(Goods("", "Unknown", 10.0));
    } catch (const std::exception& error) {
        std::cout << "Rejected invalid goods: " << error.what() << '\n';
    }

    try {
        inventory.updateWeight("Rice Sack", -3.0);
    } catch (const std::exception& error) {
        std::cout << "Rejected invalid weight: " << error.what() << '\n';
    }

    try {
        inventory.get("Missing Cargo");
    } catch (const std::exception& error) {
        std::cout << "Handled missing lookup: " << error.what() << '\n';
    }
}

void benchmarkLookup() {
    printDivider("AVL lookup vs linear search");
    constexpr int itemCount = 10000;
    constexpr int lookupRepeats = 2000;

    AvlInventory inventory;
    std::vector<Goods> linearItems;
    linearItems.reserve(itemCount);

    for (int i = 0; i < itemCount; ++i) {
        Goods item("Cargo-" + std::to_string(i), "Benchmark", 1.0 + (i % 90));
        inventory.add(item);
        linearItems.push_back(item);
    }

    const std::string target = "Cargo-9999";

    auto avlStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < lookupRepeats; ++i) {
        (void)inventory.get(target);
    }
    auto avlStop = std::chrono::high_resolution_clock::now();

    auto linearStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < lookupRepeats; ++i) {
        (void)linearSearchByName(linearItems, target);
    }
    auto linearStop = std::chrono::high_resolution_clock::now();

    const auto avlMicros = std::chrono::duration_cast<std::chrono::microseconds>(avlStop - avlStart).count();
    const auto linearMicros = std::chrono::duration_cast<std::chrono::microseconds>(linearStop - linearStart).count();

    std::cout << "Items: " << itemCount << ", repeated lookups: " << lookupRepeats << '\n';
    std::cout << "AVL lookup time: " << avlMicros << " microseconds\n";
    std::cout << "Linear search time: " << linearMicros << " microseconds\n";
    std::cout << "AVL height after insertions: " << inventory.height() << '\n';
    std::cout << "AVL balanced after insertions: " << (inventory.balanced() ? "yes" : "no") << '\n';
}

int main() {
    try {
        demonstrateQueue();
        demonstrateStack();
        demonstrateSorting();
        demonstrateInventory();
        benchmarkLookup();
    } catch (const std::exception& error) {
        std::cerr << "Fatal application error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
