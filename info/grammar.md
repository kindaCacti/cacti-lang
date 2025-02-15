$$
\begin{align}
    [\text{prog}] &\to [\text{statement}]^* \\
    [\text{stmt}] &\to 
    \begin{cases}
        exit([\text{expr}]); \\
        let \text{ ident} = [expr];
    \end{cases}\\
    [\text{expr}] &\to
    \begin{cases}
        \text{intlit}\\
        \text{identlit}
    \end{cases}
\end{align}
$$