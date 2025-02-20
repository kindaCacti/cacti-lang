$$
\begin{align}
    [\text{prog}] &\to [\text{statement}]^* \\
    [\text{stmt}] &\to 
    \begin{cases}
        \text{exit}([\text{expr}]); \\
        \text{let ident} = \text{[expr];}\\
        \text{if ([exprbin]) [stmt]}\\
        \text{\{ $\text{[stmtblck]}^*$ \}}\\
        \text{ident = [expr];}\\
        \text{ident:}\\
        \text{goto [ident];}
    \end{cases}\\
    [\text{stmtblck}] &\to
    \begin{cases}
        [\text{stmt}]^*
    \end{cases}\\
    [\text{expr}] &\to
    \begin{cases}
        \text{intlit}\\
        \text{ident}\\
        \text{[oper]}
    \end{cases}\\
    \text{[oper]} &\to
        \text{[arit]}^+\\
    \text{[arit]} &\to
    \begin{cases}
        \text{[operator]}\\
        \text{intlit}\\
        \text{ident}
    \end{cases}\\
    \text{[operator]} &\to
    \begin{cases}
        +\\
        -\\
        *\\
        / \\
    \end{cases}\\
    \text{[exprbin]} &\to
    \begin{cases}
        \text{[expr][binsign][expr]}
    \end{cases}\\
    \text{[binsign]} &\to
    \begin{cases}
        ==\\
        !=\\
        >>\\
        >=\\
        <<\\
        <=\\
    \end{cases}
\end{align}
$$