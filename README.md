# Mini - C Parse tree and output generator
This project is a parse tree and output generator for a simplified version of the C language. The context free grammar for the language is below


> `program` -> `declaration_statement`";"`statement_set` | `statement_set` | `declaration_statement`";"
> 
> `statement_set` -> `statement`";"`statement_set` | `statement`";"
> 
> `statement` -> `read_statement` | `write_statement` | `assignment_statement` | `for_loop_statement`
> 
> `declaration_statement` -> "int" `set_of_variables`
> 
> `read_statement` -> "read" `variable`
> 
> `write_statement` -> "write" `variable` | "write" `integer_constant`
> 
> `assignment_statement` -> `variable`"="`expression`
> 
> `for_loop_statement` -> "for""("`assignment_statement`";"`expression`";"`assignment_statement`")""{"`statement_set`"}"
> 
> `expression` -> `expression`  `low_precedence_operator`  `expression_level_2` | `expression_level_2`
> 
> `expression_level_2` -> `expression_level_2`  `med_precedence_operator`  `expression_level_3` | `expression_level_3`
> 
> `expression_level_3` -> `expression_level_3` `high_precedence_operator`  `expression_level_4` | `expression_level_4`
> 
> `expression_level_4` -> "("`expression`")" | `integer_constant` | `variable`
> 
> `low_precedence_operator` -> "`"|"=="
> 
> `med_precedence_operator` -> "+"|"-"
> 
> `high_precedence_operator` -> "*"|"/"
> 
> `variable` -> "{a - z}+ - {'for','read','write','int'}"
> 
> `integer_constant` -> "{0 - 9}+"
> 
> `set_of_variables` -> `variable` | `variable`","`set_of_variables`

## Usage

 - Add your input program in `input.txt`
 - Run `main.c`
 - The output parse tree is stored in `generaltree.txt` in a generalised tree format
 - Run `tree.py` to visualize the tree, which will be available in `optree.txt`
