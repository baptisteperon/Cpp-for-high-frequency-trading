Sample console output

--- Performance Report ---
Total Market Ticks Processed: 100000
Total Orders Placed: 94770
Average Tick-to-Trade Latency (ns): 121486560
Maximum Tick-to-Trade Latency (ns): 208168208
Price threholds strategy has produced: 3 orders
Deviation from average strategy has produced: 62057 orders
Simple momentum strategy has produced: 13589 orders
Volatility based strategy has produced: 19121 orders
Total Runtime (ms): 230


1 - Which signal triggered the most orders?

    Signal number 2 (deviation from average) has trigger the most orders (62057 out of 94770 orders)
    Note that signal number 1 has triggered only 3 orders.

2 - What could you optimize further?

    * The process() logic : there is a big flaw in the logic that creates orders. If multiple signals are triggered simultaneously, only the last one is counted. This is why signal number 1 has only triggered 3 orders. To improve this, we should consider tracking buy signal for all strategies separately and applying a priority. 
    * The cost of computing rolling average and volatility : the code recalculates the average and volatility at every tick. The performances could be improved by tracking the rolling sum and sum of squared in LIFO data structures and updating the incremantally using push and pop.
    * Parallelism : treating each instrument id in parallel to speed up execution.

3 - How would your code behave with 10x more data?

    * if (hist.size() > 10) hist.erase(hist.begin()); 
This line will make the code extremely slow when the number of data point increases: each iteration, we perform a shift in the hist vector. We shoul consider using a LIFO data structure like std::deque to store the price history.

    * the size of the orders vectors will also be an issue. With the growing number of orders, we can expect more memory reallocations from std::vector. To fix this, we should consider preallocatong the orders vector with an important size.