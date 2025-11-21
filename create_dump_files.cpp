#include "create_dump_files.h"

const int MAX_COMMAND_LENGTH  = 200;

void Create_log_file (Tree_t* tree, const char* filename, int dump_type, LoadProgress* progress)
{
    assert (tree);
    assert (filename);
    //assert (progress);

    FILE* dot_file = fopen (filename, "w");

    if (!dot_file) return;

    #ifdef DEBUG
        printf ("DEBUG: Creating DOT file, tree->root = %p, tree->size = %d\n", (void*)tree->root, tree->size);
    #endif

    Create_head_log_file (dot_file);

    if (dump_type == DUMP_LOAD && progress && progress->size > 0)
    {
        Create_load_graph (tree, dot_file, progress);
    }
    else
    {
        if (tree->root == NULL)
        {
            fprintf(dot_file, "    empty [label=\"EMPTY TREE\\nRoot: NULL\\nSize: %u\", "
                             "shape=box, color=red, fontcolor=white];\n", (unsigned)tree->size);
        }
        else
        {
            Create_graph_node (tree, dot_file, tree->root);
            Make_arrow (tree, dot_file, tree->root);
        }
    }

    fprintf (dot_file,"}\n}\n");

    fclose (dot_file);

    Create_picture ();
}

void Create_head_log_file (FILE* dot_file)
{
    assert (dot_file);

    #ifdef DEBUG
        printf ("DEBUG: START Writing TO file.....\n");
    #endif

    fprintf (dot_file,"digraph tree{\n");
    fprintf (dot_file, "    charset=\"UTF-8\";\n");

    fprintf (dot_file, "    bgcolor=\"#001f29\"\n");

    fprintf (dot_file, "    rankdir = TB\n");
    fprintf (dot_file, "    nodesep = 0.5;\n");
    fprintf (dot_file, "    ranksep = 0.7;\n");

    fprintf (dot_file, "    node [shape=plaintext, style=filled, fontname=\"Arial\"];\n");

    fprintf (dot_file, "    edge [fontname=\"Arial\"];\n\n");

    fprintf (dot_file, "    labelloc=\"t\";\n");
    fprintf (dot_file,"{\n");
}

void Create_load_graph (Tree_t* tree, FILE* dot_file, LoadProgress* progress)
{
    assert(tree);
    assert(dot_file);
    assert(progress);

    #ifdef DEBUG
        printf ("DEBUG: Creating load graph with %u tracked nodes\n", (unsigned)progress->size);
    #endif

    for (size_t i = 0; i < progress->size; i++)
    {
        Node_t* node = progress->nodes[i].node;
        size_t rank = progress->nodes[i].rank;

        if (node == NULL) continue;

        Create_load_node (tree, dot_file, node, rank);
    }

    size_t max_rank = 0;
    for (size_t i = 0; i < progress->size; i++)
    {
        if (progress->nodes[i].rank > max_rank)
            max_rank = progress->nodes[i].rank;
    }


    for (size_t rank = 0; rank <= max_rank; rank++)
    {
        fprintf (dot_file, "    { rank = same; ");
        for (size_t i = 0; i < progress->size; i++)
        {
            if (progress->nodes[i].rank == rank)
                fprintf(dot_file, "node%p; ", (void*)progress->nodes[i].node);
        }
        fprintf (dot_file, "}\n");
    }

    for (size_t i = 0; i < progress->size; i++)
    {
        Node_t* node = progress->nodes[i].node;
        if (node == NULL) continue;

        Create_load_arrows (tree, dot_file, node);
    }
}

void Create_load_node (Tree_t* tree, FILE* dot_file, Node_t* node, size_t rank)
{
    assert (tree);
    assert (dot_file);
    assert (node);

    const char* fillcolor = "";
    const char* color = "black";

    if (node == tree->root)
    {
        fillcolor = "#5f3035";
        color = "#fdfdfd";
    }
    else if (node->right == NULL && node->left == NULL)
    {
        fillcolor = "#445c00";
        color = "#fdfdfd";
    }
    else
    {
        fillcolor = "#2799a0";
        color = "#fdfdfd";
    }

    fprintf(dot_file,
        "    node%p [label=<<TABLE BORDER='1' CELLBORDER='1' CELLSPACING='0'>"
        "<TR><TD COLSPAN='2'>add: %p</TD></TR>"
        "<TR><TD COLSPAN='2'>rank: %u</TD></TR>"
        "<TR><TD COLSPAN='2'>type: %d</TD></TR>"
        "<TR><TD COLSPAN='2'>value: %d</TD></TR>"
        "<TR><TD>NO</TD><TD>YES</TD></TR>"
        "<TR><TD>%p</TD><TD>%p</TD></TR></TABLE>>, "
        "fillcolor=\"%s\", color=\"%s\", fontcolor=\"%s\"];\n",
        (void*)node, (void*)node, (unsigned)rank, node->type, node->value,
        (void*)node->left, (void*)node->right, fillcolor, color, color);

    #ifdef DEBUG
        printf ("DEBUG: LOADBLOCK Writing TO file.....\n");
    #endif
}

void Create_load_arrows(Tree_t* tree, FILE* dot_file, Node_t* node)
{
    assert(tree);
    assert(dot_file);
    assert(node);

    if (node->right)
    {
        fprintf (dot_file, "    node%p -> node%p [color=\"#adebff\", penwidth=3, label=\"LEFT\", fontcolor=\"#adebff\", fontsize=13, arrowsize=0.8, penwidth=1, weight=3];\n",
                (void*)node, (void*)node->right);
    }

    if (node->left)
    {
        fprintf (dot_file, "    node%p -> node%p [color=\"#ffadb1\", penwidth=3, label=\"RIGHT\"fontcolor=\"#ffadb1\", fontsize=13, arrowsize=0.8, penwidth=2, weight=3];\n",
                (void*)node, (void*)node->left);
    }
}

void Create_graph_node (Tree_t* tree, FILE* dot_file, Node_t* node)
{
    assert (tree);
    assert (dot_file);

    if (node == NULL) return;

    const char* fillcolor = "";
    const char* color = "black";

    if (node == tree->root)
    {
        fillcolor = "#5f3035";
        color = "#fdfdfd";
    }
    else if (node->right == NULL && node->left == NULL)
    {
        fillcolor = "#445c00";
        color = "#fdfdfd";
    }
    else
    {
        fillcolor = "#2799a0";
        color = "#fdfdfd";
    }

    fprintf (dot_file, "    node%p [label=<<TABLE BORDER='1' CELLBORDER='1' CELLSPACING='0'>"
            "<TR><TD COLSPAN='2'>addr: %p</TD></TR>"
            "<TR><TD COLSPAN='2'>type: %d</TD></TR>"
            "<TR><TD COLSPAN='2'>value: %d</TD></TR>"
            "<TR><TD>NO</TD><TD>YES</TD></TR>"
            "<TR><TD>%p</TD><TD>%p</TD></TR></TABLE>>, "
            "fillcolor=\"%s\", color=\"%s\", fontcolor=\"%s\"];\n",
            (void*)node, (void*)node, node->type, node->value, (void*)node->left, (void*)node->right, fillcolor, color, color);

    #ifdef DEBUG
        printf ("DEBUG: BLOCKS Writing TO file.....\n");
    #endif

    Create_graph_node (tree, dot_file, node->left);

    Create_graph_node (tree, dot_file, node->right);
}

void Make_arrow (Tree_t* tree, FILE* dot_file, Node_t* node)
{
    assert (tree);

    assert (dot_file);

    if (node == NULL) return;

    #ifdef DEBUG
        printf("DEBUG: Make_arrow: creating connections...\n");
    #endif

    if (node->right)
    {
        fprintf (dot_file, "    node%p -> node%p [color=\"#adebff\", penwidth=3, label=\"LEFT\", fontcolor=\"#adebff\", fontsize=13, arrowsize=0.8, penwidth=1, weight=3];\n",
                (void*)node, (void*)node->right);
    }

    if (node->left)
    {
        fprintf (dot_file, "    node%p -> node%p [color=\"#ffadb1\", penwidth=3, label=\"RIGHT\"fontcolor=\"#ffadb1\", fontsize=13, arrowsize=0.8, penwidth=2, weight=3];\n",
                (void*)node, (void*)node->left);
    }

    Make_arrow(tree, dot_file, node->left);

    Make_arrow(tree, dot_file, node->right);
}

void Create_picture (void)
{
    static int image_counter = 1;

    char command[MAX_COMMAND_LENGTH] = "";

    snprintf (command, sizeof(command), "dot -Tsvg tree_dump.dot -o imagesDump/tree_dump%d.svg", image_counter);

    system (command);

    #ifdef DEBUG
        printf ("dot -Tsvg tree_dump.dot -o imagesDump/tree_dump%d.svg", image_counter);
    #endif

    printf ("Graph generated: tree_dump%d.svg\n", image_counter);

    image_counter++;
}

void Create_dump_files (Tree_t* tree, const char* file, const char* func, int line, const char* reason, ...)
{
    assert (tree);
    assert (file);
    assert (func);
    assert (reason);

    char formatted_reason[300] = {};
    va_list args = {};
    va_start (args, reason);
    vsnprintf (formatted_reason, sizeof(formatted_reason), reason, args);
    va_end (args);

    int dump_type = DUMP_NORMAL;
    const char* buffer = NULL;
    size_t position = 0;
    LoadProgress* progress = NULL;

    dump_type = va_arg (args, int);

    if (dump_type == DUMP_LOAD)
    {
        buffer = va_arg (args, const char*);
        position = va_arg (args, size_t);
        progress = va_arg (args, LoadProgress*);
    }
    va_end(args);

    #ifdef DEBUG
        printf("DEBUG: Dump called from %s:%d (%s): %s\n", file, line, func, formatted_reason);
    #endif

    char graph_filename[50] = {};
    char dot_filename[50] = {};
    static int dump_counter = 1;

    #ifdef DEBUG
        printf ("DEBUG: Create_dump_files STARTED for %s\n", func);
    #endif

    if (dump_counter == 1)
    {
        system ("mkdir -p imagesDump");
    }

    #ifdef DEBUG
        printf ("=== Make_html_file called #%d ===\n", dump_counter);
    #endif

    snprintf (dot_filename, sizeof(dot_filename), "tree_dump.dot");

    snprintf (graph_filename, sizeof(graph_filename), "tree_dump%d.svg", dump_counter);

    Create_html_file (tree, dump_counter, graph_filename, func, formatted_reason, dump_type, buffer, position, progress);

    #ifdef DEBUG
        printf ("DEBUG: Creating DOT file: %s\n", dot_filename);
    #endif

    Create_log_file(tree, dot_filename, dump_type, progress);

    #ifdef DEBUG
        printf ("Create_dump_files COMPLETED\n");
    #endif

    dump_counter++;
}

void Create_html_file (Tree_t* tree, int dump_counter, const char* graph_filename, const char* func, const char* formatted_reason, ...)
{
    static FILE* html_file = NULL;

    if (html_file == NULL)
    {
        Create_head_html (&html_file);
    }

    fprintf (html_file, "<div class='dump'>\n");
    fprintf (html_file, "       <h2>Dump %d Called from: <font color=white>%s</font></h2>\n", dump_counter, func);
    fprintf (html_file, "       <h2>Dump %d Called because: <font color=white>%s</font></h2>\n\n", dump_counter, formatted_reason);

    va_list args;
    va_start(args, formatted_reason);

    int dump_type = DUMP_NORMAL;
    const char* buffer = NULL;
    size_t position = 0;
    LoadProgress* progress = NULL;

    dump_type = va_arg (args, int);

    if (dump_type == DUMP_LOAD)
    {
        buffer = va_arg (args, const char*);
        position = va_arg (args, size_t);
        progress = va_arg (args, LoadProgress*);
    }
    va_end (args);

    if (dump_type == DUMP_LOAD && buffer != NULL)
    {
        PrintBuffer (html_file, buffer, position);

        if (progress != NULL)
        {
            fprintf (html_file, "<div class='progress-info'>\n");
            fprintf (html_file, "   <h4>Load Progress: %u nodes, current rank: %u</h4>\n",
                    (unsigned)progress->size, (unsigned)progress->current_rank);
            fprintf (html_file, "</div>\n\n");
        }
    }

    if (tree == NULL)
    {
        printf ("ERROR: tree is NULL\n");
        fprintf (html_file, "   <p><b>ERROR: List is NULL</b></p>\n");
        fprintf (html_file, "</div>\n\n");
        dump_counter++;
        return;
    }

    fprintf (html_file, "       <h3>Graph Visualization</h3>\n");

    fprintf (html_file, "       <img src='imagesDump/%s' alt='Graph %d' width='1500'>\n", graph_filename, dump_counter);

    fprintf (html_file, "</div>\n\n");

    fflush (html_file);

    #ifdef DEBUG
        printf ("HTML dump #%d completed\n", dump_counter);
    #endif
}

void Create_head_html (FILE** html_file)
{
    #ifdef DEBUG
        printf ("DEBUG: Creating HTML file...\n");
    #endif

    *html_file = fopen ("All_dumps.html", "w");

    if (*html_file == NULL)
    {
        printf("ERROR: Cannot create HTML file\n");
        return;
    }

    fprintf (*html_file, "<!DOCTYPE html>\n");
    fprintf (*html_file, "<html lang='ru'>\n");
    fprintf (*html_file, "<head>\n");
    fprintf (*html_file, "<meta charset='UTF-8'>\n");
    fprintf (*html_file, "<title>TREE Dumps</title>\n");
    fprintf (*html_file, "<style>\n");

    fprintf (*html_file, "  body { background-color: #001f29; color: #ffffff; font-size: 20px; }\n");

    fprintf (*html_file, "  table { border-collapse: collapse; margin: 10px; }\n");
    fprintf (*html_file, "  th, td { border: 1px solid #0077a3; padding: 5px; }\n");
    fprintf (*html_file, "  th { background-color: #00415a; }\n");

    fprintf (*html_file, "      img { max-width: 100%%; height: auto; margin: 10px; }\n");

    fprintf (*html_file, "  .dump { border: 2px solid #0099cc; padding: 15px; margin: 10px; }\n\n");

    fprintf (*html_file, "      h1 { color: #00ccff; font-size: 36px; }\n");
    fprintf (*html_file, "      h2 { color: #00b8e6; font-size: 28px; }\n");
    fprintf (*html_file, "      h3 { color: #66d9ff; font-size: 30px; }\n");
    fprintf (*html_file, "      h4 { color: #66d9ff; font-size: 30px; }\n\n");

    fprintf (*html_file, "  pre { color: #b3ecff; background-color: #001a21; padding: 10px; }\n");

    fprintf (*html_file, ".parsing-section { margin: 20px 0; padding: 15px; background: #002b36; border: 1px solid #005a73; }\n");
    fprintf (*html_file, ".buffer-display { font-family: 'Courier New', monospace; font-size: 25px; background: #001f29; padding: 10px; border-radius: 5px; }\n");
    fprintf (*html_file, ".getted-text { color: #888888; }\n");
    fprintf (*html_file, ".cursor { color: #ff4444; font-weight: bold; }\n");
    fprintf (*html_file, ".notgetted-text { color: #ffffff; }\n");
    fprintf (*html_file, ".buffer-stats { margin-top: 10px; color: #66d9ff; font-size: 20px; }\n");
    fprintf (*html_file, "  table { border-collapse: collapse; margin: 10px; font-size: 18px; }\n");
    fprintf (*html_file, "  th, td { border: 1px solid #0077a3; padding: 8px; }\n");

    fprintf (*html_file, "</style>\n");
    fprintf (*html_file, "</head>\n");
    fprintf (*html_file, "<body>\n");
    fprintf (*html_file, "<h1 style='color: red;'>ALL LIST DUMPS</h1>\n");

    #ifdef DEBUG
        printf ("HTML file created successfully\n");
    #endif
}

void PrintBuffer (FILE* html_file, const char* buffer_start, size_t position)
{
    fprintf(html_file, "\n\t<div class='parsing-section'>\n");
    fprintf(html_file, "\t<h4>Buffer:</h4>\n");
    fprintf(html_file, "\t<div class='buffer-display'>\n");

    size_t show_len = strlen(buffer_start);
    if (show_len > 500) show_len = 500;

    fprintf(html_file, "\t<span class='getted-text'>%.*s</span>",
            (int)position, buffer_start);
    fprintf(html_file, "\t<span class='cursor'>|</span>");
    fprintf(html_file, "\t<span class='notgetted-text'>%.*s</span>",
            (int)(show_len - position), buffer_start + position);
    fprintf(html_file, "\t</div>\n");

    size_t len = strlen(buffer_start);
    if (len > 0)
    {
        int percent = (int)((position * 100) / len);
        fprintf(html_file, "\t<div class='buffer-stats'>Position: %u/%u (%d%%)</div>\n",
               (unsigned)position, (unsigned)len, percent);
    }

    fprintf(html_file, "\t</div>\n");
}

void Close_html_file(void)
{
    static FILE* html_file = NULL;

    if (html_file != NULL)
    {
        fprintf(html_file, "</body>\n</html>\n");
        fclose(html_file);
        html_file = NULL;
        printf("HTML file closed successfully\n");
    }
}
