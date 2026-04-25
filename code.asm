.data
    n1 DATA 1
    counter DATA 10
    menos1 DATA 1
    tabuada SPACE 10
.text
    ORG 0

    LDA menos1
    NOT
    ADD menos1
    STA menos1
    NOT

    LDA n1
    STA [tabuada+counter]
    LDA counter
    ADD menos1
    STA counter

    loop:
        LDA [tabuada + counter + 1]
        ADD n1
        STA [tabuada + counter]
        
        LDA counter
        ADD menos1
        JZ floop
        JMP loop

        STA counter
    floop:
        HLT
