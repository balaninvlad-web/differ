#ifndef CREATE_LATEX_DUMP_H
#define CREATE_LATEX_DUMP_H

int CreateLaTeXDumpFile (char* tex_filename, Tree_t* tree, Tree_t* diff_tree);
void Create_head_html (FILE** tex_file, char* tex_filename);
void Add_expression_to_latex (FILE* tex_file, Tree_t* tree, const char* title);
void Add_derivative_to_latex (FILE* tex_file, Tree_t* tree, const char* variable);
void Close_latex_file (FILE* tex_file);
void print_tree_latex (FILE* file, Node_t* node);

#endif
