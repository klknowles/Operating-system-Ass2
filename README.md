# Operating-system-Ass2
This project is to demonstrate the use of semaphores with multiple processes in process synchronization. The idea behind this is that there are four lanes meeting at a junction. Each junction is coming from a different direction, North, South, East, and West. Each process represents a bus that wants to pass through the junction to the other side. Each bus has to check if there is a bus coming from the lane on the right of it and wether or not there is a bus already in the junction before it can move. We accomplish this by having each lane and the junction be represented by a semaphore. For example, if a bus is coming from the north it first has to get the north semaphore because there may be another bus that was there before it, then request the west semaphore and afterwards request the junction semaphore. Each bus process is created from the manager function based on the battern written in the sequence.txt file. The manager function's purpose is also to check for and prevent deadlocks in the system, however this portion has not been done yet but i expect to finish it very soon. 
