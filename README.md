# Simplified Lua Compiler



## Gramática

A gramática original está no documento
[analise-sintatica.pdf](assets/analise-sintatica.pdf) dentro do diretório assets.\
Segue as observações da transformação LL(1)

1. `Params` e `Names` eram redundantes na gramática.
2. `FunctionBody` não era necessário.
3. `Names` e `Name` eu chamei de `Ids` e `id`, respectivamente.
4. `Stmt` tinha múltiplas regras que iam em “local ...” e “for ...”.
5. `Block`, `Exps`, `Field`, `BinOp`, `Vars`, `Function` e `Names` estavam ok.
6. `Exp` tinha recursão à esquerda.
7. `PrefixExp` e `Var` tinham recursão indireta. Taquei `PrefixExp` em `Var`, que ganhou recursão à esquerda.\
Depois taquei Var em PrefixExp porque o first de ambos são iguais.
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

Exps
  ::= Exp (, Exp)*

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

PrefixExp
  ::= id Var2
   |  ( Exp ) Var2

Fields
  ::= Field (, Field)*

Field
  ::= [ Exp ] = Exp
   |  id = Exp

BinOp
  ::= or | and | < | > | <= | >= | ~= | == | .. | + | - | * | / | ^

Vars
  ::= Var (, Var)*

Var
  ::= id Var2
   |  ( Exp ) [ Exp ] Var2

Var2
  ::= [ Exp ] Var2
   |  ɛ

Function
  ::= function id? ( Ids? ) Block end

Ids
  ::= id (, id)*
```

## First e Follow

...

## Implementação

O analisador sintático (que eu chamei de parser) está dividido em três arquivos:
[Parser.h](source/Parser.h), [ParserBase.cpp](source/ParserBase.cpp) e
[ParserRules.cpp](source/ParserRules.cpp). Utilizei orientação a objetos, assim
como no trabalho de análise léxica. Então existe uma classe Parser, e ela
tem um Lexer e um Token como campos.

#### Observações Gerais

A Primeira coisa importante a se dizer se trata dos nomes das variáveis no
código. `Exp` virou `Expression`, `Stmt` virou `Statement` e por ai vai.

Um outro ponto é sobre os métodos que implementam as regras. Eu fiz duas coisas
diferentes de como está na gramática:

1. Criei um procedimento para o padrão `do Block end` chamado `do_statement()`.
Eu chamo ele toda vez que o padrão aparece. O porém é que em teoria deveria
existir uma regra `DoBLock` ou algo assim, mas criá-la deixaria a gramática
maior, então considerei isso somente na implementação.
2. A implementação dos comandos for e if ficaram grandinhas, por isso eu
modulei `Stmt` um pouco.
3. `Vars` só é chamado dentro de `Stmt` e sua implementação é pequena.
Então eu não separei `Vars` em um procedimento. Também fiz isso com `Fields`,
chamado somente dentro de `Exp`.

#### Funcionamento

O executável precisa de exatamente 1 argumento adicional: o nome do arquivo a
ser analisado. O programa principal está dentro de um bloco try-catch para
tratar um possível erro de arquivo.
