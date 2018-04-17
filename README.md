![image1](pictures/simple_algorithm.png)
![image2](pictures/concurrent_algorithm.png)
## Bad case scenario: multi-thread overhead
All nodes create one big component, parallelism does not help here much.
Graphs show mainly overhead added by parallelism.

### Overhead in simple algorithm scales almost linearly
![image3](pictures/simple_algorithm_cycle.png)

### Overhead in concurrent algorithm scales ~1.5 times on 12 threads compared to 1 thread time.
It's probably because of deterministic work distribution in on_the_fly_scc_union_node::get_node_from_set
![image4](pictures/concurrent_algorithm_cycle.png)
