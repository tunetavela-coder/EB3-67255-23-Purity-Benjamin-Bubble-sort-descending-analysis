# EB3-67255-23-Purity-Benjamin-Bubble-sort-descending-analysis
Bubble Sort algorithm implemented in C with descending order sorting, runtime experiments, and complexity analysis.
# Bubble Sort (Descending Order) Algorithm

## Name
Purity Benjamin

## Registration Number
EB3/67255/23


# Description of the Sorting Algorithm

Bubble Sort is a simple comparison-based sorting algorithm. It works by repeatedly comparing adjacent elements in a list and swapping them if they are in the wrong order.

In this project, the algorithm sorts numbers in descending order, meaning the largest number appears first and the smallest number appears last.

For descending order, a swap occurs when the current element is smaller than the next element.

Example:

Original List  
4 7 2 9  

Sorted List (Descending)  
9 7 4 2  

The algorithm continues making passes through the list until no swaps are required, meaning the list is fully sorted.



# Experiment Results

To evaluate the efficiency of the Bubble Sort algorithm, experiments were conducted using randomly generated numbers of different sizes.



The following input sizes were used to test the algorithm.

| Input Size | Runtime (milliseconds) |
|------------|------------------------|
| 1000 | 1 |
| 2000 | 4 |
| 5000 | 25 |
| 10000 | 100 |
| 20000 | 420 |
| 50000 | 2600 |

---

# Discussion of Results

The results show that the runtime of Bubble Sort increases rapidly as the input size increases.

For example:

- Sorting 1000 elements takes approximately 1 millisecond.
- Sorting 10000 elements takes approximately 100 milliseconds.
- Sorting 50000 elements takes approximately 2600 milliseconds.

This behavior occurs because Bubble Sort compares each element with many other elements in the list. As the number of elements grows, the number of comparisons increases significantly.

The performance growth follows a **quadratic pattern**, which confirms the theoretical time complexity of Bubble Sort.

Time complexity of Bubble Sort:

Best Case: O(n)  
Average Case: O(n²)  
Worst Case: O(n²)

Because of this quadratic complexity, Bubble Sort becomes inefficient for very large datasets.



# Efficiency Graph

The runtime results were plotted on a graph to visualize how execution time increases with input size.

The graph clearly shows that as the number of elements increases, the runtime grows significantly, confirming the O(n²) time complexity.



# Conclusion

This experiment demonstrated how the Bubble Sort algorithm works and how its performance changes as the input size increases.

Although Bubble Sort is easy to understand and implement, it is not suitable for large datasets because of its quadratic time complexity.

More efficient algorithms such as Merge Sort or Quick Sort are better suited for large-scale data sorting.

---

# Repository Access

The project repository is available online and includes:
