#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_TOKENS 10000
#define MAX_LINE_LENGTH 1000
#define CHILD_COUNT 25
int token_count = 0,var_count =0;
int cur_token = 0; 
int cur_tree_index =0;
char* tokens[MAX_TOKENS]; 
char generalised_tree[10000];
char* variable_names[50];
int variable_values[50];
char global_str[20];

typedef struct ParseTreeNode{
    char *value;
    struct ParseTreeNode* child_nodes[CHILD_COUNT];
} ParseTreeNode;

ParseTreeNode* program();
ParseTreeNode* statement_set();
ParseTreeNode* declaration_statement();
ParseTreeNode* statement();
ParseTreeNode* read_statement();
ParseTreeNode* write_statement();
ParseTreeNode* assignment_statement();
ParseTreeNode* for_loop_statement();
ParseTreeNode* expression(int limit);
ParseTreeNode* expression_level_2(int limit);
ParseTreeNode* expression_level_3(int limit);
ParseTreeNode* expression_level_4(int limit);
ParseTreeNode* low_precedence_operators();
ParseTreeNode* med_precedence_operators();
ParseTreeNode* high_precedence_operators();
ParseTreeNode* variable();
ParseTreeNode* set_of_variables();
ParseTreeNode* integer_constant();
ParseTreeNode* terminal(char* name);


void error(char* message){
    printf("%s\n", message);
    exit(1);
}

ParseTreeNode* new_node(char* value){
    ParseTreeNode *node = (ParseTreeNode *)malloc(sizeof(ParseTreeNode));
    node->value = value;
    for(int i=0;i<CHILD_COUNT;i++) {
        node->child_nodes[i] = 0;
    }
    return node;
}

ParseTreeNode* program(){
    ParseTreeNode* cur_node = new_node("program");
    int child_num = 0;
    if(!strcmp(tokens[cur_token],"int")){
        cur_node->child_nodes[child_num++] = declaration_statement();
        cur_node->child_nodes[child_num++] = terminal(";");
        if(cur_token<token_count){
            cur_node->child_nodes[child_num++] = statement_set();
        }
    }
    else{
        cur_node->child_nodes[child_num++] = statement_set();
    }

    return cur_node;
}

ParseTreeNode* statement_set(){
    ParseTreeNode* cur_node = new_node("statement_set");
    int childNumber = 0;
    cur_node->child_nodes[childNumber] =  statement();
    childNumber++;
    if(!strcmp(tokens[cur_token],";")){
        cur_node->child_nodes[childNumber] = terminal(";");
        childNumber++;
    }
    if(cur_token == token_count || !strcmp(tokens[cur_token],"}")){
        return cur_node;
    } 
    else{
        cur_node->child_nodes[childNumber] = statement_set();
        childNumber++;
        return cur_node;        
    }   
}

ParseTreeNode* declaration_statement(){
    ParseTreeNode* cur_node = new_node("declaration_statement");
    int child_num = 0;
    cur_node->child_nodes[child_num] = terminal("int");
    child_num++;
    cur_node->child_nodes[child_num] = set_of_variables();
    child_num++;
    return cur_node;
}

ParseTreeNode* read_statement(){
    ParseTreeNode* cur_node = new_node("read_statement");
    int child_num = 0;
    cur_node->child_nodes[child_num] = terminal("read");
    child_num++;
    cur_node->child_nodes[child_num] = variable();
    child_num++;
    return cur_node;
}

ParseTreeNode* write_statement(){
    ParseTreeNode* cur_node = new_node("write_statement");
    int child_num = 0;
    cur_node->child_nodes[child_num] = terminal("write");
    child_num++;
    char ch = tokens[cur_token][0];
    if(ch>='0' && ch <= '9'){
        cur_node->child_nodes[child_num] = integer_constant();
        child_num++;
    }
    else{
        cur_node->child_nodes[child_num] = variable();
        child_num++;
    }
    return cur_node;
}

ParseTreeNode* assignment_statement(){
    ParseTreeNode* cur_node = new_node("assignment_statement");
    int child_num = 0;
    cur_node->child_nodes[child_num] = variable();
    child_num++;
    cur_node->child_nodes[child_num] = terminal("=");
    child_num++;
    int limit = cur_token;
    int numBrack =0;
    while(strcmp(tokens[limit],";"))
    {
        if((!numBrack && !strcmp(tokens[limit],")"))){
            break;
        }
        numBrack += (!strcmp(tokens[limit],"("));
        numBrack -= (!strcmp(tokens[limit],")"));
        limit++;
    }
    cur_node->child_nodes[child_num++] = expression(limit);
    return cur_node;
}

ParseTreeNode* for_loop_statement(){
    ParseTreeNode* cur_node = new_node("for_loop_statement");
    int child_num = 0;
    cur_node->child_nodes[child_num] = terminal("for");
    child_num++;
    cur_node->child_nodes[child_num] = terminal("(");
    child_num++;
    cur_node->child_nodes[child_num] = assignment_statement();
    child_num++;
    cur_node->child_nodes[child_num] = terminal(";");
    child_num++;
    int limit = cur_token;
    while(strcmp(tokens[limit],";")){
        limit++;
    }
    cur_node->child_nodes[child_num] = expression(limit);
    child_num++;
    cur_node->child_nodes[child_num] = terminal(";");
    child_num++;
    cur_node->child_nodes[child_num] = assignment_statement();
    child_num++;
    cur_node->child_nodes[child_num] = terminal(")");
    child_num++;
    cur_node->child_nodes[child_num] = terminal("{");
    child_num++;
    cur_node->child_nodes[child_num] = statement_set();
    child_num++;
    cur_node->child_nodes[child_num] = terminal("}");
    child_num++;
    return cur_node;
}

ParseTreeNode* statement(){
    ParseTreeNode* cur_node = new_node("statement");
    if(!strcmp(tokens[cur_token],"read")){
        cur_node->child_nodes[0] = read_statement();
        return cur_node;
    }
    else if(!strcmp(tokens[cur_token],"write")){
        cur_node->child_nodes[0] = write_statement();
        return cur_node;
    }
    else if(!strcmp(tokens[cur_token],"for")){
        cur_node->child_nodes[0] = for_loop_statement();
        return cur_node;
    }
    else{
        for(int i=0;tokens[cur_token][i];i++){
            if(tokens[cur_token][i]<'a' || tokens[cur_token][i]>'z') error("Error Occured");
        }
        cur_node->child_nodes[0] = assignment_statement();
        return cur_node;
    }
}

ParseTreeNode* expression(int limit){
    ParseTreeNode* cur_node = new_node("expression");
    int bracket_count = 0;
    int initialCur = cur_token;
    int rightmost_op=-1;
    while(strcmp(tokens[cur_token],";") && cur_token<limit){
        if(!strcmp(tokens[cur_token],"(")){
             bracket_count++;
        }
        if(!strcmp(tokens[cur_token],")")){
             bracket_count--;
        }
        if((!strcmp(tokens[cur_token],">") || !strcmp(tokens[cur_token],"==")) && bracket_count == 0){
            rightmost_op = cur_token;
        }
        cur_token++;
    }
    cur_token = initialCur;
    if(rightmost_op==-1){
        cur_node->child_nodes[0] = expression_level_2(limit);
        return cur_node;
    }
    cur_node->child_nodes[0] = expression(rightmost_op);
    cur_node->child_nodes[1] = low_precedence_operators();
    cur_node->child_nodes[2] = expression_level_2(limit);
    return cur_node;
}

ParseTreeNode* expression_level_2(int limit){
    ParseTreeNode* cur_node = new_node("expression_level_2");
    int bracket_count = 0;
    int initialCur = cur_token;
    int rightmost_op=-1;
    while(strcmp(tokens[cur_token],";") && cur_token<limit){
        if(!strcmp(tokens[cur_token],"(")){
            bracket_count++;
        }
        if(!strcmp(tokens[cur_token],")")){
            bracket_count--;
        }
        if((!strcmp(tokens[cur_token],"+") || !strcmp(tokens[cur_token],"-")) && bracket_count == 0){
            rightmost_op = cur_token;
        }
        cur_token++;
    }
    cur_token = initialCur;
    if(rightmost_op==-1){
        cur_node->child_nodes[0] = expression_level_3(limit);
        return cur_node;
    }
    cur_node->child_nodes[0] = expression_level_2(rightmost_op);
    cur_node->child_nodes[1] = med_precedence_operators();
    cur_node->child_nodes[2] = expression_level_3(limit);
    return cur_node;
}

ParseTreeNode* expression_level_3(int limit){
    ParseTreeNode* cur_node = new_node("expression_level_3");
    int bracket_count = 0;
    int initialCur = cur_token;
    int rightmost_op=-1;
    while(strcmp(tokens[cur_token],";") && cur_token<limit){
        if(!strcmp(tokens[cur_token],"(")){
            bracket_count++;
        }
        if(!strcmp(tokens[cur_token],")")){
            bracket_count--;
        }
        if((!strcmp(tokens[cur_token],"*") || !strcmp(tokens[cur_token],"/")) && bracket_count == 0){
            rightmost_op = cur_token;
        }
        cur_token++;
    }
    cur_token = initialCur;
    if(rightmost_op==-1){
        cur_node->child_nodes[0] = expression_level_4(limit);
        return cur_node;
    }
    cur_node->child_nodes[0] = expression_level_3(rightmost_op);
    cur_node->child_nodes[1] = high_precedence_operators();
    cur_node->child_nodes[2] = expression_level_4(limit);
    return cur_node;
}

ParseTreeNode* expression_level_4(int limit){
    ParseTreeNode* cur_node = new_node("expression_level_4");
    if(!strcmp(tokens[cur_token],"(")){
        cur_node->child_nodes[0] = terminal("(");
        cur_node->child_nodes[1] = expression(limit);
        cur_node->child_nodes[2] = terminal(")");
    }
    else if (tokens[cur_token][0]>='0' && tokens[cur_token][0]<='9'){
        cur_node->child_nodes[0] = integer_constant();
    }
    else{
        cur_node->child_nodes[0] = variable();
    } 

    return cur_node;
    
    
}

ParseTreeNode* low_precedence_operators(){
    ParseTreeNode* cur_node = new_node("low_precedence_operators");
    cur_node->child_nodes[0] = terminal(tokens[cur_token]);
    return cur_node;
}

ParseTreeNode* med_precedence_operators(){
    ParseTreeNode* cur_node = new_node("med_precedence_operators");
    cur_node->child_nodes[0] = terminal(tokens[cur_token]);
    return cur_node;
}

ParseTreeNode* high_precedence_operators(){
    ParseTreeNode* cur_node = new_node("high_precedence_operators");
    cur_node->child_nodes[0] = terminal(tokens[cur_token]);
    return cur_node;
}

ParseTreeNode* variable(){
    if(!strcmp(tokens[cur_token], "read") || !strcmp(tokens[cur_token], "write") || !strcmp(tokens[cur_token], "for") || !strcmp(tokens[cur_token], "int")){
        error("Variable names cannot be keywords.");
    }
    
    ParseTreeNode* cur_node = new_node("variable");
    cur_node->child_nodes[0] = terminal(tokens[cur_token]);
    return cur_node;
}

ParseTreeNode* set_of_variables(){
    ParseTreeNode* cur_node = new_node("set_of_variables");
    int child_num =0;
    cur_node->child_nodes[child_num++] = variable();
    if(!strcmp(tokens[cur_token],",")){
        cur_node->child_nodes[child_num++] = terminal(",");
        cur_node->child_nodes[child_num++] = set_of_variables();
    }
    return cur_node;
}

ParseTreeNode* integer_constant(){
    ParseTreeNode* cur_node = new_node("integer_constant");
    cur_node->child_nodes[0] = terminal(tokens[cur_token]);
    return cur_node;
}

ParseTreeNode* terminal(char* term){
    ParseTreeNode* node = new_node(term);
    cur_token++;
    return node;
}


// char* string_convert(int num){
//     if(num==0){
//         return "0";
//     }
//     int digits = 0;
//     int num2 = num;
//     while(num2){
//         digits++;
//         num2/=10;
//     }
//     char* ans = malloc(sizeof(char)*(digits+1));
//     memset(ans,0,sizeof(ans));
//     num2 = num;
//     int i=digits-1;
//     while(num2){
//         char digit = '0'+(num2%10);
//         ans[i] = digit;
//         num2/=10;
//         i--;
//     }
//     return ans;

// }

void string_convert(int number){
    if(number<0){
        error("Integer Overflow");
    }
    memset(global_str,0,sizeof(global_str));
    if(number==0){
        global_str[0] = '0';
        return;
    }
    int digits = 0;
    int numcopy = number;
    while(numcopy){
        digits++;
        numcopy/=10;
    }
    
    int i=digits-1;
    while(number){
        char digit = '0'+(number%10);
        global_str[i] = digit;
        number/=10;
        i--;
    }
}

int variable_lookup(char* variable_string){
    if(variable_string[0]>='0' && variable_string[0]<='9'){
        return -1;
    }
    int i = 0;
    while(variable_names[i]){
        if(!strcmp(variable_names[i],variable_string)){
            return i;
        }
        i++;
    }
    error("Variable undeclared.");
}

char* expression_handler(int l, int r){
    char* string1[MAX_TOKENS], *string2[MAX_TOKENS];
    memset(string1, 0, sizeof(string1));
    memset(string2, 0, sizeof(string2));
    int i = l;
    int start=0; 
    int j=0;
    int bracket_count = 0;
    while(i<r){
        if(!strcmp(tokens[i], "(") && bracket_count == 0){
            bracket_count++;
            start = i+1;
        }
        else if(!strcmp(tokens[i], "(") && bracket_count != 0){
            bracket_count++;
        }
        else if(!strcmp(tokens[i], ")") && bracket_count == 1){
            strcpy(string1[j],expression_handler(start, i));
            bracket_count--;
            j++;
        }
        else if(!strcmp(tokens[i], ")") && bracket_count != 1){
            bracket_count--;
        }
        else if(bracket_count==0){
            string1[j] = malloc(sizeof(tokens[i]));
            strcpy(string1[j], tokens[i]);
            j++;
        }
        i++;
    }

    i=0;
    j=0;

    while(string1[i]){
        if(!strcmp(string1[i],"*")){
            int operand1 = variable_lookup(string2[j-1]);
            int operand2 = variable_lookup(string1[i+1]);
            if(operand1==-1) {
                operand1 = atoi(string2[j-1]);
            }
            else {
                operand1 = variable_values[operand1];
            }
            if(operand2==-1) {
                operand2 = atoi(string1[i+1]);
            }
            else {
                operand2 = variable_values[operand2];
            }
            // string2[j-1] = string_convert(operand1*operand2);
            string_convert(operand1*operand2);
            strcpy(string2[j-1],global_str);
            i++;
        }
        else if(!strcmp(string1[i],"/")){
            int operand1 = variable_lookup(string2[j-1]);
            int operand2 = variable_lookup(string1[i+1]);
            if(operand1==-1){ 
                operand1 = atoi(string2[j-1]);
            }
            else {
                operand1 = variable_values[operand1];
            }
            if(operand2==-1){
                operand2 = atoi(string1[i+1]);
            }
            else {
                operand2 = variable_values[operand2];
            }
            // string2[j-1] = string_convert(operand1/operand2);
            string_convert(operand1/operand2);
            strcpy(string2[j-1],global_str);
            i++;
        }
        else{
            string2[j] = malloc(sizeof(string1[i]));
            strcpy(string2[j], string1[i]);
            j++;
        }
        i++;
    }

    i=0;
    j=0;
    memset(string1, 0, sizeof(string1));

    while(string2[i]){
        if(!strcmp(string2[i],"+")){
            int operand1 = variable_lookup(string1[j-1]);
            int operand2 = variable_lookup(string2[i+1]);
            if(operand1==-1) {
                operand1 = atoi(string1[j-1]);
            }    
            else {
                operand1 = variable_values[operand1];
            }    
            if(operand2==-1) {
                operand2 = atoi(string2[i+1]);
            }    
            else {
                operand2 = variable_values[operand2];
            }    
            // string1[j-1] = string_convert(operand1+operand2);
            string_convert(operand1+operand2);
            strcpy(string1[j-1],global_str);
            i++;
        }
        else if(!strcmp(string2[i],"-")){
            int operand1 = variable_lookup(string1[j-1]);
            int operand2 = variable_lookup(string2[i+1]);
            if(operand1==-1) {
                operand1 = atoi(string1[j-1]);
            }
            else {
                operand1 = variable_values[operand1];
            }
            if(operand2==-1) {
                operand2 = atoi(string2[i+1]);
            }
            else {
                operand2 = variable_values[operand2];
            }
            // string1[j-1] = string_convert(operand1-operand2);
            string_convert(operand1-operand2);
            strcpy(string1[j-1],global_str);
            i++;
        }
        else{
            string1[j] = malloc(sizeof(string2[i]));
            strcpy(string1[j], string2[i]);
            j++;
        }
        i++;
    }

    memset(string2, 0, sizeof(string2));

    i=0;j=0;

    while(string1[i]){
        if(!strcmp(string1[i],"==")){
            int operand1 = variable_lookup(string2[j-1]);
            int operand2 = variable_lookup(string1[i+1]);
            if(operand1==-1){
                operand1 = atoi(string2[j-1]);
            }
            else{
                operand1 = variable_values[operand1];
            }
            if(operand2==-1){
                operand2 = atoi(string1[i+1]);
            }
            else{
                operand2 = variable_values[operand2];
            }
            // string2[j-1] = string_convert(operand1==operand2);
            string_convert(operand1==operand2);
            strcpy(string2[j-1],global_str);
            i++;
        }
        else if(!strcmp(string1[i],">")){
            int operand1 = variable_lookup(string2[j-1]);
            int operand2 = variable_lookup(string1[i+1]);
            if(operand1==-1) {
                operand1 = atoi(string2[j-1]);
            }
            else {
                operand1 = variable_values[operand1];
            }
            if(operand2==-1) {
                operand2 = atoi(string1[i+1]);
            }
            else {
                operand2 = variable_values[operand2];
            }
        //    string2[j-1] = string_convert(operand1>operand2);
        string_convert(operand1>operand2);
            strcpy(string2[j-1],global_str);
            i++;
        }
        else{
            string2[j] = malloc(sizeof(string1[i]));
            strcpy(string2[j], string1[i]);
            j++;
        }
        i++;
    }
    int result_ind = variable_lookup(string2[0]);
    if(result_ind!=-1){
        // char* result_str;
        // result_str = string_convert(variable_values[result_ind]);
        string_convert(variable_values[result_ind]);
        return global_str;
    }
    else{
        return string2[0];
    }
}

int simulate_program(int cur_token){
    int j=0;
    while(tokens[cur_token]){
        if(!strcmp(tokens[cur_token],"}")){
            return cur_token;
        }
        else if(!strcmp(tokens[cur_token],"int")){
            cur_token++;
            while(strcmp(tokens[cur_token],";")){
                if(!strcmp(tokens[cur_token],",")){
                    cur_token++;
                    continue;
                }
                variable_names[j] = malloc(sizeof(tokens[cur_token]));
                strcpy(variable_names[j], tokens[cur_token]);
                cur_token++;
                j++;
            }
            cur_token++;
        }
        else if(!strcmp(tokens[cur_token],"read")){
            cur_token++;
            int input;
            scanf("%d", &input);
            variable_values[variable_lookup(tokens[cur_token])] = input;
            cur_token+=2;
        }
        else if(!strcmp(tokens[cur_token],"write")){
            cur_token++;
            int variable_ind = variable_lookup(tokens[cur_token]);
            if(variable_ind==-1){
                printf("%s",tokens[cur_token]); 
            }
            else {
                printf("%d\n",variable_values[variable_ind]);
            }
            cur_token+=2;
        }
        else if(!strcmp(tokens[cur_token],"for")){
            cur_token+=2;int term1 = cur_token;cur_token+=2;int e1start = cur_token;
            while(strcmp(tokens[cur_token],";")){
                cur_token++;
            }
            int e1end = cur_token;cur_token++;int e2start = cur_token;
            while(strcmp(tokens[cur_token],";")){
                cur_token++;
            }
            int e2end = cur_token;cur_token++;int term2 = cur_token;cur_token+=2;int e3start = cur_token;
            while(strcmp(tokens[cur_token],")")){
                cur_token++;
            }
            int e3end = cur_token;cur_token+=2;int progStart = cur_token;
            variable_values[variable_lookup(tokens[term1])] = atoi(expression_handler(e1start, e1end));
            int check = 0;
            while(atoi(expression_handler(e2start, e2end))){
                check++;
                cur_token = simulate_program(progStart);
                variable_values[variable_lookup(tokens[term2])] = atoi(expression_handler(e3start, e3end));
            }
            if(!check){
                while(strcmp(tokens[cur_token],"}")){
                    cur_token++;
                }
                cur_token+=2;
            }
            else {
                cur_token+=2;
            }
        }
        else{ 
            int val = variable_lookup(tokens[cur_token]);
            cur_token+=2;int start = cur_token;
            while(strcmp(tokens[cur_token],";")) {
                cur_token++;
            }
            variable_values[val] = atoi(expression_handler(start,cur_token));
            cur_token++;
        }
    }
    return cur_token;
     
}

void construct_parse_tree(ParseTreeNode* n){
    generalised_tree[cur_tree_index] = '[';
    cur_tree_index++;
    int j =0;
    while(n->value[j]){
        generalised_tree[cur_tree_index] = n->value[j];
        cur_tree_index++;
        j++;
    }
    for(int child =0;n->child_nodes[child];child++){
        generalised_tree[cur_tree_index] = ' ';
        cur_tree_index++;
        construct_parse_tree(n->child_nodes[child]);
    }
    generalised_tree[cur_tree_index] = ']';
    cur_tree_index++;
}

void tokenize(char* filename){
    FILE* in_file = fopen(filename, "r");
    
    char line[MAX_TOKENS];
    char token[100];
    int j=0;
    
    while (fgets(line, MAX_LINE_LENGTH, in_file)) {
        for (int i = 0; i < strlen(line); ++i) {
            if(line[i] == '(' || line[i] == ')' || line[i] == ','  || line[i] == ';' || line[i]=='=' || line[i] == '>' || line[i] == '+' || line[i] == '*' || line[i] == '/' || line[i] == '-')
            {
                if(!token[0]){
                    token[0] = line[i];
                    if(line[i]=='=' && i+1<strlen(line) && line[i+1]=='=') {
                        token[1] = line[i+1];i++;
                    }
                    tokens[token_count] = malloc(sizeof(token));
                    strcpy(tokens[token_count], token);
                    memset(token, 0, strlen(token));
                    token_count++;
                    j=0;
                    continue;
                }
                tokens[token_count] = malloc(sizeof(token));
                strcpy(tokens[token_count], token);
                memset(token, 0, strlen(token));
                token[0] = line[i];
                if(line[i]=='=' && i+1<strlen(line) && line[i+1]=='=') {
                    token[1] = line[i+1];
                    i++;
                }
                tokens[token_count+1] = malloc(sizeof(token));
                strcpy(tokens[token_count+1], token);
                memset(token, 0, strlen(token));
                token_count+=2; 
                j=0;	    
            }	    
            else if (line[i] == ' ' || line[i] == '\t' || line[i] == 10 /* ASCII 10 = Line Feed */ || line[i] == '\n')
            {
                if(!token[0]) continue; 
                tokens[token_count] = malloc(sizeof(token));
                strcpy(tokens[token_count], token);  
                memset(token, 0, strlen(token));
                token_count++; 
                j=0; 
            }
            else
            {
                token[j]=line[i];
                j++; 
            }
        }
    } 
     
}

int main(){
    tokenize("input.txt");
    // for (int i = 0; i < token_count; ++i)
    // {
    //     if(tokens[i]) printf("%s|", tokens[i]);
    // }
    fflush(stdout);
    ParseTreeNode* root = program();
    construct_parse_tree(root);
    // printf("\nThe obtained parse tree in generalised format is:\n\n");  
    FILE *fle = fopen("generaltree.txt", "w");
    for(int i=0;generalised_tree[i];i++){
        fprintf(fle,"%c",generalised_tree[i]);
    }
    printf("\n\n");
    printf("Simulating program: \n\n");
    simulate_program(0);
    return 0;
}