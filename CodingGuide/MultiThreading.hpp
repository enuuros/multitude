/** Part ofthe coding guide for the Multitude library collection. */


   
/*! \mainpage threadingguide Multithreading guide

Multithreading poses some caveats that one should take into account,
when writing code that is to be used in a threaded environment. We
assume that you have a fairly standard SMP (symmetric multiprocessing)
environment, where each CPU (or CPU core) sees the same main memory.


\section threadingmemory Managing Memory

\subsection threadingfreemalloc Avoid malloc/free in Threaded Code

Memory management functions, such as malloc/free and new/delete need
to use a global memory buffer, that is used for the allocations. The
memory allocation routines need to synchronize their access to the
area so that multipel threads are not working on the same area
simultaneously. 

In practice this means that there is global mutex that protects the
memory area. If the application has several threads that use memory in
an aggressive way, then the global mutex can become a bottle-neck.

To overcme this problem, it is best to (if possible) use big chunks of
memory, rather than small units, for example vectors instead of
lists. Further, it is better to keep memory allocated than to
repeatedly alloce/free the memory. This is one of the cases where
Radiant::VectorStorage is a handy utility.

\subsection threadingpageaccess Avoid Page Access Conflicts

An often-forgotten problem with multithreading applications is
conflict with memory page ownership. When CPU 1 (or CPU core) needs to
use memory at location X which is on page Y, then the content of X is
cached and page Y is locked to the CPU 1. If CPU 2 needs anything from
the same memory page, then it needs to get the page Y from CPU 1, and
CPU 1 needs to flush out of its cache anything on page Y.

In a multi-core CPU the cores may even share the cache, so this is
neceassarily a huge problem - page ownership is transferred very
quickly. How-ever if the cores do not share the cache, then one can
expect real problems. This is the case with Intel Quad-Core family,
which are basically composed of two dual-core CPUs. If the
computational load exceeds the limits of the two cores, then one can
end up having real problems.

The solution to this problem is to have - for each thread - the
relevant data on separate memory pages. Fo course there is no way to
explicitly tell "malloc" or "new" that now you would like to have the
memory allcations to come from a specific page. Luckily most memory
allocators try to allocate data so that each thread receives data from
unique memory pages, which it implicitly "owns". To take advantage of
this feature, you need to allocate memory from the thread that is
going to use that memory.

With most applications this is not visible to the end-user as few
applications can utilise more than one CPU core effeciently. How-ever,
with more complex applications one needs to exercise greater care.

BTW: Problems like this are one reason why in the Cell architecture
the eight parallel processors (SPUs) need to do explicit access to the
main memory with DMA. That way page access problems go away.


*/
