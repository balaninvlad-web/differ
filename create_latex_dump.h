#ifndef CREATE_LATEX_DUMP_H
#define CREATE_LATEX_DUMP_H

int CreateLaTeXDumpFile (const char* tex_filename, Tree_t* tree, Tree_t* Difftree);
void Create_head_tex (FILE** latex_file, const char* tex_filename);
void Add_expression_to_latex (FILE* tex_file, Tree_t* tree, const char* title);
void Add_derivative_to_latex (FILE* tex_file, Tree_t* tree, const char* variable);
void Close_latex_file (FILE* tex_file);
void print_tree_latex (FILE* file, Node_t* node);
int CreateLaTeXDumpPdf (const char* tex_filename);
void print_unary_operator (FILE* file, const char* op_name, Node_t* operand);
void print_operand_with_brackets (FILE* file, Node_t* operand, bool for_multiplication);

#endif
