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
        \text{ident}\\
        \text{[expr]} \text{[operator]} \text{[expr]}\\
    \end{cases}\\
    \text{[operator]} &\to
    \begin{cases}
        +\\
        *\\
    \end{cases}
\end{align}
$$