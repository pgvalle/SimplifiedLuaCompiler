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

## Observações da Implementação




