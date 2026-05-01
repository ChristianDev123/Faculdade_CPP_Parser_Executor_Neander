.data
    n1 DATA 1
    n1n DATA 0
    counter DATA 10
    menos1 DATA 1
    mais1 DATA 1
    tabuada SPACE 3
.text
    ORG 0

    LDA menos1
    NOT
    ADD mais1
    STA menos1
    NOT

    LDA n1
    NOT
    ADD mais1
    STA n1n
    NOT

    loop:
        LDA n1
        ADD n1
        STA n1
        
        LDA counter
        ADD menos1
        JZ
        STA counter
        JMP loop

    floop:

        LDA n1
        ADD n1n
        STA [tabuada]

        LDA [tabuada]
        ADD n1n
        STA [tabuada + 1]

        LDA [tabuada + 1]
        ADD n1n
        STA [tabuada + 1 + 1]

        HLT