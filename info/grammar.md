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
        \text{int lit}\\
        \text{ident}\\
        \text{[oper]}
    \end{cases}\\
    \text{[oper]} &\to
        \text{[arit]}^+\\
    \text{[arit]} &\to
    \begin{cases}
        \text{[operator]}\\
        \text{int lit}\\
        \text{ident}
    \end{cases}\\
    \text{[operator]} &\to
    \begin{cases}
        +\\
        *\\
    \end{cases}
\end{align}
$$