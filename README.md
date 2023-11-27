# Simplified Lua Compiler



## Gramática

A gramática original está no documento
[analise-sintatica.pdf](assets/analise-sintatica.pdf) dentro do diretório assets.\
Seguem as observações da transformação LL(1):

1. `Params` e `Names` eram redundantes na gramática.
2. `FunctionBody` não era necessário.
3. `Names` e `Name` eu chamei de `Ids` e `id`, respectivamente.
4. `Stmt` tinha múltiplas regras que iam em “`local ...`” e “`for ...`”.
5. `Block`, `Exps`, `Field`, `BinOp`, `Vars`, `Function` e `Names` estavam ok.
6. `Exp` tinha recursão à esquerda.
7. `PrefixExp` e `Var` tinham recursão indireta. Taquei `PrefixExp` em `Var`, que ganhou recursão à esquerda.\
Depois taquei `Var` em `PrefixExp` porque o First de ambos são iguais.
8. O "OPT" virou "?" para facilitar.

```
Block
  ::= (Stmt;)*

Stmt
  ::= Vars = Exps
   |  Function
   |  do Block end
   |  while Exp do Block end
   |  if Exp then Block (elseif Exp then Block ElseIf)* (else Block)? end
   |  return Exps?
   |  break
   |  for id Fors
   |  local Decls

Fors
  ::= (, id)* in Exps do Block end
   |  = Exp, Exp (, Exp)? do Block end

Decls
  ::= Ids = Exps
   |  Function

Exp
  ::= not Exp Exp2
   |  -Exp Exp2
   |  PrefixExp Exp2
   |  Function Exp2
   |  { Fields? } Exp2
   |  nil Exp2
   |  true Exp2
   |  false Exp2
   |  Number Exp2
   |  String Exp2

Exp2
  ::= BinOp Exp Exp2
   |  ɛ

Exps
  ::= Exp (, Exp)*

PrefixExp
  ::= id Var2
   |  ( Exp ) Var2

Field
  ::= [ Exp ] = Exp
   |  id = Exp

Fields
  ::= Field (, Field)*

BinOp
  ::= or | and | < | > | <= | >= | ~= | == | .. | + | - | * | / | ^

Var
  ::= id Var2
   |  ( Exp ) [ Exp ] Var2

Var2
  ::= [ Exp ] Var2
   |  ɛ

Vars
  ::= Var (, Var)*

Function
  ::= function id? ( Ids? ) Block end

Ids
  ::= id (, id)*
```

## First

```
Block      ( break do for function id if local return while ϵ
Stmt       ( break do for function id if local return while
Fors       , in =
Decls      function id
Exp        ( - false function id nil not number string true {
Exp2       or and < > <= >= ~= == .. + - * / ^ ϵ
Exps       ( - false function id nil not number string true { ϵ
PrefixExp  ( id
Field      [ id
Fields     [ id ϵ
BinOp      or and < > <= >= ~= == .. + - * / ^ ϵ
Ids        id
Var        ( id
Var2       [ ϵ
Vars       ( id
Function   function
```

## Follow

```
Block      EOTS else elseif end
Stmt       ;
Fors       ;
Decls      ;
Exp        ) , ] do then or and < > <= >= ~= == .. + - * / ^
Exp2       ) , ] do then or and < > <= >= ~= == .. + - * / ^
Exps       ; do
PrefixExp  ) , ] do then or and < > <= >= ~= == .. + - * / ^
Field      ,
Fields     }
BinOp      ( - false function id nil not number string true {
Ids        ) =
Var        , =
Var2       ) , = ] or and < > <= >= ~= == .. + - * / ^ do then
Vars       =
Function   ) , ] ; do then or and < > <= >= ~= == .. + - * / ^
```

## Implementação

O analisador sintático  está dividido em três arquivos:
[Parser.h](source/Parser.h), [ParserBase.cpp](source/ParserBase.cpp) e
[ParserRules.cpp](source/ParserRules.cpp). Utilizei orientação a objetos, assim
como no trabalho de análise léxica. Então existe uma classe Parser, e ela tem um
Lexer e um Token como campos.

#### Observações Gerais

A Primeira coisa importante a se dizer se trata dos nomes das variáveis no
código. `Exp` virou `Expression`, `Stmt` virou `Statement` e por ai vai.

Um outro ponto é sobre os métodos que implementam as regras. Eu fiz algumas
coisas diferentes de como está na gramática:

1. Criei um procedimento para o padrão `do Block end` chamado `do_statement()`.
Eu chamo ele toda vez que o padrão aparece. O porém é que em teoria deveria
existir uma regra `DoStmt` ou algo assim, mas criá-la deixaria a gramática
maior, então considerei isso somente para a implementação.
2. As implementações de algumas coisas, como as dos comandos for e if, ficaram
meio grandinhas. Por isso eu modulei um pouco mais o código, mesmo não tendo (e
eu não querendo adicionar) as regras `ForStmt` e `IfStmt` na gramática.
3. `Vars` e `Fields` só são chamados dentro de `Stmt` e suas implementações são
pequenas. Então simplesmente os implementei diretamente dentro de `Exp`.

#### Funcionamento

**NOTA:** O executável precisa exatamente de 1 argumento: o caminho (relativo
ou absoluto) do arquivo a analisar.

`parse()` pega o primeiro token e passa a execução para
`Block`. Quando `Block` termina, `parse()` verifica se o token atual é o `EOTS`.
Se for, ótimo. Senão, continua chamando `Block` enquanto não for.

Para tratamento de erro, implementei o método `panic(err_msg, sync_sets)`, que
descarta tokens até que se ache algum dentro de `sync_sets`. No caso de encontrar
`EOTS`, o programa termina.

