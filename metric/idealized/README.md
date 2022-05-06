# Models of core/periphery structures

Stephen P. Borgatti, Martin G. Everett

## Real to Idealized Approximation Score

$$
\rho{} = \sum\limits_{i, j} a_{ij} \delta{}_{ij}
$$

Where $A$ is the adjacency matrix of the graph and $\delta{}$ is the pattern matrix of the core-periphery structure to approximate.

## Discrete Model

### Equation 2

$$
\delta{}_{ij} = \left\{ \begin{array}{c l}
  1 & \text{if } c_{i} = \text{CORE } \text{or } c_{j} = \text{CORE} \\
  0 & \text{otherwise} \\
\end{array} \right\}
$$

A generalization of the maximally centralized graph shown by Freeman and evidenced by the star graph.

Or in plain terms, any node in the graph should have connections to every $\text{CORE}$ in the graph.

### Equation 3

$$
\delta{}_{ij} = \left\{ \begin{array}{c l}
  1 & \text{if } c_{i} = \text{CORE } \text{and } c_{j} = \text{CORE} \\
  0 & \text{otherwise} \\
\end{array} \right\}
$$

The only edges present ideally in this pattern are ones that connect cores to one another.

### Equation 4

$$
\delta{}_{ij} = \left\{ \begin{array}{c l}
  1 & \text{if } c_{i} = \text{CORE } \text{and } c_{j} = \text{CORE} \\
  0 & \text{if } c_{i} = \text{PERIPHERY } \text{and } c_{j} = \text{PERIPHERY} \\
  . & \text{otherwise} \\
\end{array} \right\}
$$

Pattern which maximizes density in the core and minimizes density in the periphery. The density of core-to-periphery and periphery-to-core ties is a specified intermediate value between, $. \in{} \left[ 0, 1 \right]$.

## Continuous Model

If the values are constrained to $\left\{ 0, 1 \right\}$, this is equivalent to the [Discrete Model](#discrete-model)

### Equation 5

$$
\delta_{ij} = c_{i}c_{j}
$$

Where $C$ is a vector of nonnegative values indicating the degree of coreness of each node, such as the weights of a weighted graph.

## Empirical Estimate Variant

### Equation 6

$$
\begin{array}{l}
  \delta_{ij} = f(c_{i}c_{j}) \\
  f(c_{i}c_{j}) = \left\{ \begin{array}{c l}
    1 & \text{if } c_{i}c_{j} > t \\
    0 & \text{otherwise} \\
  \end{array} \right\}
\end{array}
$$

Variant which defines a threshold value, $t$, to dichotomize the pattern matrix (which reduces to [Equation 3](#equation-3) and [Equation 4](#equation-4)).

## Priori Hypotheses Variant

### Equation 7

$$
\text{Pr}(a_{ij} = 1) = \frac{e^{\alpha{} + \beta{}c_{i}c_{j}}}{1 + e^{\alpha{} + \beta{}c_{i}c_{j}}}
$$

Probabilistic variant with $f(c_{i}c_{j}) \equiv{} \text{Pr}(a_{ij = 1})$ where $\alpha{}$ and $\beta{}$ are parameters to be estimated. Not recommended (no reason to choose this) unless there is a theory to how ties are formed.
