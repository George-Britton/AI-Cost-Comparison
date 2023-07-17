# AI Cost Comparison
### A three-tailed application of AI in video games comparing the storage requirements, memory usage, and believability

---------------------------------------

The three child branches of master (FSM, BT and GOAP) are each host to a single application of an artificial intelligence algorithm.

FSM hosts a simple finite-state machine that directs the enemies in this game in the most common application of AI.

BT demonstrates a custom-written (that being not UE4-API) behaviour tree that directs the enemies to act using the combination of composite, decorator, and leaf nodes common in this algorithm.

GOAP contains an implementation of Jeff Orkin's _Goal-Oriented Action Planning_ (Orkin 2003), in which a queue of actions leading to a goal state is creating using the A* algirthm to search the actionbase's preconditions and effects.

<sub>The Framework branch holds the skeleton that each AI is added onto, so that the only difference between each build is the AI included.</sub>

---------------------------------------
References:

Orkin, J. (2003), _‘Applying goal-oriented action planning to games’_, AI game programming wisdom 2, 217–228.
