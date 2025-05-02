PART 1

    Console output:

Trade created successfully AAPL at 150
Trade created successfully GOOGL at 2800
Trade created successfully FB at 350
Trade created successfully NFLX at 500
Trade created successfully TSLA at 400
Trade created successfully AMZN at 520
Trade deleted successfully: AAPL
Trade deleted successfully: AMZN
Trade deleted successfully: TSLA
Trade deleted successfully: NFLX
Trade deleted successfully: FB
Trade deleted successfully: GOOGL

    1 - What happens if you use delete on the array?

When we use delete instead of delete[] on the array, we get a warning at compile time and during the execution, only the first element of the arry is deleted.

    2 - What happens if you forget to call delete at all?

If we forget to call delete at all, the memory is never freed.

    3 - Can you call delete twice on the same pointer?

Calling delete twice on the same pointer will not create any issues at compile time, but the program will crash during is execution, raising the following error:
"manual_memory_management_raii(75484,0x1fda97ac0) malloc: Double free of object 0x11f605d20"

PART 2

See code

PART 3

    1 - Why do we delete the copy constructor?

We don't want to allow copying of a trade object.
Let's say we use the following code :

TradeHandle t1(new Trade("AAPL", 150.0));
TradeHandle t2 = t1;

t1 and t2 point to the same Trade object. When the destructor of one of them is called, the other as no way of knowing it. Next time the TradeHandle object that hasn't been deleted is called, it will have undefined behavior.

    2 - Why is move semantics allowed?

The move semantic is allowed bcause if allows transfer of ownership without copying. Each Trade object remains controlled by one TradeHandle object -> safe.

    3 - What happens if you don't define a destructor?

If we don't define a destructor, the default destructor is called. This will be a problem in our case because TradeHandle manages raw pointers. So TradeHandle won't delete the Trade object is points to. This will cause a memory leak.

3.2 Use your TradeHandle in a test program

Console output:

Trade created successfully AAPL at 150
TradeHandle took ownership of trade 0x147e05d20
Trade created successfully MSFT at 250
TradeHandle took ownership of trade 0x147e05d80
Trade deleted successfully: AAPL
TradeHandle deleted trade 0x0
Trade deleted successfully: MSFT
TradeHandle deleted trade 0x147e05d80

PART 5

    1 - What are the 3 biggest risks of manual memory management?

        * Memory leaks: when we allocate memory with new but forget to delete it. The memory space is lost and cannot be reallocated
        * Double Delete: when we call delete on the same pointer more than once. This leads to  undefined behavior
        * Dangling Pointers: when a pointer is still used after the object it points to has been freed. The leads to undefined behavior.

    2 - What real-world problems could occur in HFT systems due to memory leaks or fragmentation?

        * Memory leaks can lead to software crashes. The HFT system would need to be restarted and thus might miss trades or fail to respond to market events.
        * Allocating and freeing many objects of different sizes creates fragmentation. Over time, if the system has enough total memory, but no contiguous chunk to satisfy a large allocation, allocation failures will occur or the system will spend more time earching for a fit. In low-latency systems, allocators must be predictable and fast. Fragmentation kills that.

    3 - How does RAII reduce the risk of bugs?

RAII binds a resource’s lifetime to the lifetime of an object. It reduces the risk of bug by ensuring there are no memory leaks and thus no memory fragmentation or undefined behavior during the runtime of the program.

    4 - What tradeoffs exist between manual and automatic memory management in performance-critical code?

        * manual memory management(new/delete): Highly optimized (constructor and destructor ar not called unless explicitly stated), very high bug risk (leaks, double deletes, etc)
        * automatic memory management(RAII/smartpointers): Slight overhead(detructor), lower risk of bug (deletion handled automatically)