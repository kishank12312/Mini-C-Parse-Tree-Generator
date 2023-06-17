from nltk.tree import *

# assign your output (generalied list of the syntax tree) to varaible text
filei = open("generaltree.txt", 'r')
# text = "[program [declaration_statement [int] [set_of_variables [variable [a]] [,] [set_of_variables [variable [b]] [,] [set_of_variables [variable [i]]]]]] [;] [statement_set [statement [read_statement [read] [variable [a]]]] [;] [statement_set [statement [for_loop_statement [for] [(] [assignment_statement [variable [i]] [=] [expression [expression_level_2 [expression_level_3 [expression_level_4 [integer_constant [10]]]]]]] [;] [expression [expression [expression_level_2 [expression_level_3 [expression_level_4 [variable [i]]]]]] [low_precedence_operators [>]] [expression_level_2 [expression_level_3 [expression_level_4 [integer_constant [0]]]]]] [;] [assignment_statement [variable [i]] [=] [expression [expression_level_2 [expression_level_2 [expression_level_3 [expression_level_4 [variable [i]]]]] [med_precedence_operators [-]] [expression_level_3 [expression_level_4 [integer_constant [1]]]]]]] [)] [{] [statement_set [statement [assignment_statement [variable [b]] [=] [expression [expression_level_2 [expression_level_3 [expression_level_3 [expression_level_4 [variable [a]]]] [high_precedence_operators [*]] [expression_level_4 [variable [i]]]]]]]] [;] [statement_set [statement [write_statement [write] [variable [b]]]] [;]]] [}]]] [statement_set [statement [write_statement [write] [variable [b]]]] [;]]]]]"
text = filei.read().strip()
file = open("optree.txt", 'w')

text = text.replace("(", "ob")    #in the syntax tree, 'ob' will display in place of '('
text = text.replace(")", "cb")    #in the syntax tree, 'cb' will display in place of ')'
text = text.replace("[", "(")
text = text.replace("]", ")")

tree = Tree.fromstring(text)
# print(tree)
file.write(TreePrettyPrinter(tree).text())
