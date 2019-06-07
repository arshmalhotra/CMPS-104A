// CMPS104A
// ASSIGNMENT: ASG5
// NAME 1: Shlok Gharia
// EMAIL 1: sgharia@ucsc.edu
// NAME 2: Arsh Malhotra
// EMAIL 2: amalhot3@ucsc.edu

#include "intermediate.h"

using namespace std;

FILE* out;

vector<unordered_map<const string*, string>> vars;
vector<astree*> gvars;
unordered_map<string, string> ops;

int reg = 1;

string var_search(astree* node) {
   for(int i = vars.size()-1; i >= 0; --i) {
      if(vars[i].find(node->lexinfo) == vars[i].end())
         continue;
      else {
         return vars[i].at(node->lexinfo);
      }
   }
   while(vars.size() <= uint(node->blocknr)) {
      unordered_map<const string*, string> map;
      vars.push_back(map);
   }

   string new_var = "_";
   if(node->blocknr != 0)
      new_var += to_string(node->blocknr)+"_"+*(node->lexinfo);
   else if(node->attributes[ATTR_field]) {
      new_var += *(node->lexinfo);
   }
   else
      new_var += "_"+*(node->lexinfo);

   pair <const string*, string> var (node->lexinfo, new_var);
   vars[node->blocknr].insert(var);
   return new_var;
}

string format_var(astree* node) {
   astree* var;
   string val;
   if(node->symbol == TOK_ARRAY)
      var = node->children[1];
   else
      var = node->children[0];

   attr_bitset attr = var->attributes;

   if(attr[ATTR_void]) {
      val += "void ";
   }
   if (attr[ATTR_int] == 1) {
      val += "int ";
   } if (attr[ATTR_struct] == 1) {
      if(var->parent->symbol == TOK_ARRAY)
         val += "struct s_" + var_search(var) + "* ";
      else
         val += "struct s_"+ node->struct_name +"* ";
   } if (attr[ATTR_string] == 1) {
      val += "char*";
   }

   if(attr[ATTR_array]) {
      val += "* ";
   } else {
      val += " ";
   }

   if(attr[ATTR_field]) {
      val += "f_"+ var->struct_name+var_search(var);
   }

   if(attr[ATTR_param]) {
      val += var_search(var);
   }

   return val;
}

bool find_op(string op) {
   if(ops.find(op) == ops.end()) {
      ops.insert(pair <string, string> (op, op));
      return false;
   }
   return true;
}

string create_reg(astree* node) {
   string res;
   attr_bitset attr = node->attributes;
   if(attr[ATTR_int]) {
      res += "i" + to_string(reg);
   } else if(attr[ATTR_string]) {
      res += "s" + to_string(reg);
   } else if(attr[ATTR_struct]) {
      res += "p" + to_string(reg);
   }
   reg++;
   return res;
}

string create_stmt(astree* node, bool gvar) {
   string expr, op;

   astree* sym1 = node->children[0];
   string arr, variable;
   switch(sym1->symbol) {
      case TOK_INDEX: {
         expr += traverse_block(node->children[0]->children[0]) + "["
         + traverse_block(node->children[0]->children[1]) + "] = ";
         break;
      }
      case TOK_INT: {
         if(node->blocknr == 0)
            expr += var_search(node->children[0]->children[0]) + " = ";
         else
            expr += "int " +
               var_search(node->children[0]->children[0]) + " = ";
         break;
      }
      case TOK_STRING: {
         if(node->blocknr == 0)
            expr += var_search(node->children[0]->children[0]) + " = ";
         else
            expr += "char* " +
               var_search(node->children[0]->children[0]) + " = ";
         break;
      }
      case TOK_TYPEID: {
         if(node->blocknr == 0)
            expr += var_search(node->children[0]->children[0]) + " = ";
         else
            expr += "struct s_" + sym1->struct_name + "* " +
               var_search(node->children[0]->children[0]) + " = ";
         break;
      }
      case TOK_ARRAY: {
         op = "p"+to_string(reg++);
         if(node->blocknr != 0) {
            if(sym1->children[1]->attributes[ATTR_int]) {
               arr+= "int";
            } else if(sym1->children[1]->attributes[ATTR_string]
                  && sym1->children[1]->attributes[ATTR_array]) {
               arr+= "char*";
            } else if(sym1->children[1]->attributes[ATTR_string]) {
               arr+= "char";
            } else if(sym1->children[1]->attributes[ATTR_struct]) {
               arr+= "struct " + *(sym1->children[0]->lexinfo) + "*";
            }
            arr += "* " + op;
         }
         else {
            arr += var_search(sym1->children[1]);
         }
         expr += arr + " = ";
         break;
      }
      default: {
         cerr << "Some other symbol" << endl;
         cout << parser::get_tname(sym1->symbol) << endl;
         break;
      }
   }

   if(node->children.size() > 1) {
      astree* sym2 = node->children[1];
      switch(sym2->symbol) {
         case TOK_INDEX: {
            expr += traverse_block(node->children[1]->children[0]) +
               "[" + traverse_block(node->children[1]->children[1])
                  +"]";
            break;
         }
         case IDENT: {
            expr += var_search(sym2);
            break;
         }
         case TOK_NULL: {
            expr += " "+*(node->lexinfo)+" 0";
            break;
         }
         case TOK_INT:
         case TOK_INTCON:
         case TOK_STRING:
         case TOK_STRINGCON: {
            expr += *(sym2->lexinfo);
            break;
         }
         case TOK_LEQ:
         case TOK_EQ:
         case TOK_NEQ:
         case TOK_GEQ:
         case '<':
         case '>': {
            expr += compare(node->children[1]);
            break;
         }
         case '+':
         case '-':
         case '*':
         case '%':
         case '/': {
            string symbol1, symbol2;
            symbol1 = traverse_block(node->children[1]->children[0]);
            symbol2 = traverse_block(node->children[1]->children[1]);
            expr += symbol1+" "+*(sym2->lexinfo)+" "+symbol2;
            break;
         }
         case TOK_ANARRAY: {
            expr += "xcalloc ("+*(sym2->children[1]->lexinfo) +
               ", sizeof(" + arr + "))";
            break;
         }
         case TOK_CALL: {
            if(gvar)
               expr += traverse_block(node->children[1]);
            else
               traverse_block(node->children[1]);
            break;
         }
         case TOK_NEW: {
            expr += "xcalloc (1, sizeof (struct s_" +
               sym2->struct_name + "))";
            break;
         }
         case '.': {
            expr += traverse_block(sym2);
            break;
         }
         default: {
            cout << "Something else" << endl;
            cout << " " << *(node->lexinfo)<<" "<<*(sym2->lexinfo);
            break;
         }
      }
   }
   fprintf(out, "%s%s;\n", TAB, expr.c_str());
   return op;
}

string compare(astree* node) {
   string sym1, sym2;

   sym1 = traverse_block(node->children[0]);
   sym2 = traverse_block(node->children[1]);

   return sym1 + " " + *(node->lexinfo) + " " + sym2;
}

string function_call(astree* node) {

   vector<string> vreg;

   if(!node->attributes[ATTR_function])
      return traverse_block(node);

   for(uint i = 1; i < node->children.size(); ++i) {
         vreg.push_back(function_call(node->children[i]));
   }
   string reg;

   if(node->parent->symbol != TOK_VAR) {
      if(node->attributes[ATTR_void]) {
         fprintf(out, "%s__%s(", TAB,
            (node->children[0]->lexinfo)->c_str());
      } else if(node->attributes[ATTR_int]) {
         reg = create_reg(node);
         fprintf(out, "%sint %s = __%s (", TAB, reg.c_str(),
            (node->children[0]->lexinfo)->c_str());
      } else if(node->attributes[ATTR_string]) {
         reg = create_reg(node);
         fprintf(out, "%schar* %s = __%s (", TAB, reg.c_str(),
            (node->children[0]->lexinfo)->c_str());
      } else if(node->attributes[ATTR_struct]) {
         reg = create_reg(node);
         fprintf(out, "%sstruct* s_%s = __%s (", TAB, reg.c_str(),
            (node->children[0]->lexinfo)->c_str());
      }
   } else {
      reg += "__" + *(node->children[0]->lexinfo)+"(";
      if(vreg.size() > 0)
         reg += vreg[0];

      for(uint i = 2; i < node->children.size(); ++i) {
            reg += ", "+ vreg[i-1];
      }
      reg += ")";
      return reg;
   }

   if(vreg.size() > 0)
      fprintf(out, "%s", vreg[0].c_str());

   for(uint i = 2; i < node->children.size(); ++i) {
      fprintf(out, ", %s", vreg[i-1].c_str());
   }
   fprintf(out, ");\n");

   return reg;
}

string traverse_block(astree* node) {

   switch(node->symbol) {

      case TOK_TYPEID: {
         return node->struct_name;
      }
      case TOK_VAR: {
         string decl = create_stmt(node, false);
         return decl;
         break;
      }
      case TOK_BLOCK: {
         for(uint i = 0; i < node->children.size(); ++i) {
            traverse_block(node->children[i]);
         }
         break;
      }
      case TOK_WHILE: {
         fprintf(out, "while_%zd_%zd_%zd:;\n",
            node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
         string operand;
         if(node->children[0]->attributes[ATTR_lval]) {
            operand = var_search(node->children[0]);
         } else {
            operand = "i" + to_string(reg++);
            fprintf(out, "%sint %s = %s;\n", TAB, operand.c_str(),
               traverse_block(node->children[0]).c_str());
            traverse_block(node->children[0]);
         }
         fprintf(out, "%s", TAB);
         fprintf(out, "if(!%s) goto break_%zd_%zd_%zd;\n",
            operand.c_str(), node->lloc.filenr,
            node->lloc.linenr, node->lloc.offset);

         traverse_block(node->children[1]);

         fprintf(out, "%s", TAB);
         fprintf(out, "goto while_%zd_%zd_%zd;\n",
            node->lloc.filenr, node->lloc.linenr, node->lloc.offset);

         fprintf(out, "break_%zd_%zd_%zd:;\n",
            node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
         break;
      }
      case TOK_IFELSE: {
         string operand;
         if(node->children[0]->attributes[ATTR_lval]) {
            operand = traverse_block(node->children[0]);;
         } else {
            operand = "i" + to_string(reg++);
            fprintf(out, "%sint %s = %s;\n", TAB, operand.c_str(),
               traverse_block(node->children[0]).c_str());
         }
         fprintf(out, "%sif(!%s) goto else_%zd_%zd_%zd;\n", TAB,
            operand.c_str(), node->lloc.filenr,
            node->lloc.linenr, node->lloc.offset);
         fprintf(out, "%s", TAB);
         traverse_block(node->children[1]);
         fprintf(out, "goto fi_%zd_%zd_%zd;\n",
            node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
         fprintf(out, "else_%zd_%zd_%zd:;\n",
            node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
         fprintf(out, "%s", TAB);
         traverse_block(node->children[2]);
         fprintf(out, "fi_%zd_%zd_%zd:;\n",
            node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
         break;
      }
      case TOK_IF: {
         string operand;
         if(node->children[0]->attributes[ATTR_lval]) {
            operand = traverse_block(node->children[0]);;
         } else {
            operand = "i" + to_string(reg++);
            fprintf(out, "%sint %s = %s;\n", TAB, operand.c_str(),
               traverse_block(node->children[0]).c_str());
         }
         fprintf(out, "%sif(!%s) goto fi_%zd_%zd_%zd;\n", TAB,
            operand.c_str(), node->lloc.filenr,
            node->lloc.linenr, node->lloc.offset);
         traverse_block(node->children[1]);
         fprintf(out, "fi_%zd_%zd_%zd:;\n",
            node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
         break;
      }
      case TOK_RET: {
         if (node->children[0]->attributes[ATTR_const]) {
            fprintf(out, "%sreturn %s;\n", TAB,
               (node->children[0]->lexinfo)->c_str());
         } else if(node->children[0]->attributes[ATTR_variable]) {
            fprintf(out, "%sreturn %s;\n", TAB,
               var_search(node->children[0]).c_str());
         } else {
            fprintf(out, "%sreturn %s;\n", TAB,
               traverse_block(node->children[0]).c_str());
         }
         break;
      }
      case TOK_RETURNVOID: {
         fprintf(out, "%sreturn;\n", TAB);
         break;
      }

      case TOK_CALL: {
         return function_call(node);
         break;
      }

      case TOK_NEG:
      case TOK_POS:
      case TOK_EXC: {
         return *(node->lexinfo)+traverse_block(node->children[0]);
      }
      case TOK_LEQ:
      case TOK_EQ:
      case TOK_NEQ:
      case TOK_GEQ:
      case '<':
      case '>': {
         return compare(node);
      }
      case '=': {
         astree* node1 = node->children[0];
         astree* node2 = node->children[1];
         fprintf(out, "%s%s = %s;\n", TAB,
            traverse_block(node1).c_str(),
            traverse_block(node2).c_str());
         break;
      }
      case '.': {
         string vreg, type;
         if(node->attributes[ATTR_int]) {
            type = "int ";
         } else if(node->attributes[ATTR_string]
               && node->attributes[ATTR_array]) {
            type = "char *";
         } else if(node->attributes[ATTR_string]) {
            type = "char ";
         } else if(node->attributes[ATTR_struct]) {
            type = "struct s_" + node->struct_name + " *";
         }

         vreg = "a"+to_string(reg++);
         fprintf(out, "%s%s*%s = &%s->f_%s_%s;\n", TAB,
            type.c_str(), vreg.c_str(),

         traverse_block(node->children[0]).c_str(),
            (node->children[0]->struct_name).c_str(),
            (node->children[1]->lexinfo)->c_str());

         return "(*"+vreg+")";
      }
      case '+':
      case '-':
      case '*':
      case '%':
      case '/': {
         string sym1 = var_search(node->children[0]);
         string sym2 = var_search(node->children[1]);

         sym1 = traverse_block(node->children[0]);
         sym2 = traverse_block(node->children[1]);
         string vreg = create_reg(node);

         fprintf(out, "%sint %s = ", TAB, vreg.c_str());
         fprintf(out, "%s %s %s;\n", sym1.c_str(),
            (node->lexinfo)->c_str(), sym2.c_str());

         return vreg;
      }
      case TOK_CHARCON:
      case TOK_INTCON:
      case TOK_STRINGCON: {
         return *(node->lexinfo);
      }
      case TOK_NULL:{
         return "0";
      }
      case IDENT: {
         return var_search(node);
      }
      case TOK_ANARRAY: {
         astree* sym1 = node->children[0];
         string arr;

         if(sym1->attributes[ATTR_int]) {
            arr+= "int";
         } else if(sym1->attributes[ATTR_string]
               && sym1->attributes[ATTR_array]) {
            arr+= "char*";
         } else if(sym1->attributes[ATTR_string]) {
            arr+= "char";
         } else if(sym1->attributes[ATTR_struct]) {
            arr+= "struct " + *(sym1->lexinfo) + "*";
         }

         return "xcalloc ("+traverse_block(node->children[1]) +
            ", sizeof(" + arr + "))";
      }
      case TOK_INDEX: {
         string vreg, type;
         if(node->attributes[ATTR_int]) {
            type = "int *";
         } else if(node->attributes[ATTR_string]
               && node->attributes[ATTR_array]) {
            type = "char **";
         } else if(node->attributes[ATTR_string]) {
            type = "char *";
         } else if(node->attributes[ATTR_struct]) {
            type = "struct s_" + node->children[0]->struct_name +
            " **";
         }

         vreg = "a"+to_string(reg++);
         fprintf(out, "%s%s%s = &%s[%s];\n", TAB, type.c_str(),
            vreg.c_str(), traverse_block(node->children[0]).c_str(),
            traverse_block(node->children[1]).c_str());

         return "(*"+vreg+")";;
      }
      default: break;
   }

   return "";
}

string format_args(astree* node) {
   string args;
   for(uint i = 0; i < node->children.size(); ++i) {
      args += TAB+format_var(node->children[i]);
      if(i < node->children.size()-1)
         args += ",\n";
   }
   return args;
}

void print_function(astree* node) {
   astree* var;
   if(node->children[0]->symbol == TOK_ARRAY) {
      var = node->children[0]->children[1];
   } else {
      var = node->children[0]->children[0];
   }
   string func = format_var(node->children[0]);
   fprintf(out, "%s__%s (\n", func.c_str(), (var->lexinfo)->c_str());

   string arg_pack = format_args(node->children[1]);
   fprintf(out, "%s)\n{\n", arg_pack.c_str());
   for(uint i = 0; i < node->children[2]->children.size(); ++i) {
      traverse_block(node->children[2]->children[i]);
   }
   fprintf(out, "}\n\n");
}

void print_struct(astree* node) {
   if(node->struct_name == "")
      return;

   fprintf(out, "struct s_%s {\n", (node->struct_name).c_str());

   // if(structs.find(node->struct_name) == structs.end())
   //    return;
   if(node->children.size() > 1) {
      astree* list = node->children[1];
      string var;
      for(uint i = 0; i < list->children.size(); ++i) {
         var = format_var(list->children[i]);
         fprintf(out, "%s%s;\n", TAB, var.c_str());
      }
   }
   fprintf(out, "};\n");
}

void print_string (astree* node) {
   fprintf(out, "char* %s = %s;\n",
      (node->children[0]->children[0]->lexinfo)->c_str(),
      (node->children[1]->lexinfo)->c_str());
}

void print_var(astree* node) {
   astree* var;
   if(node->children[0]->symbol == TOK_ARRAY) {
      var = node->children[0]->children[1];
   } else {
      var = node->children[0]->children[0];
   }

   string list = format_var(node->children[0]);
   string name = var_search(var);
   fprintf(out, "%s %s;\n\n", list.c_str(), name.c_str());
}

bool traverse(astree* node) {
   fprintf(out, "#define __OCLIB_C__\n#include \"oclib.oh\"\n\n");

   for(uint i = 0; i < node->children.size(); ++i) {
      if(node->children[i]->symbol == TOK_STRUCT) {
         print_struct(node->children[i]);
      }
   }

   for(uint i = 0; i < string_stack.size(); ++i) {
      print_string(string_stack[i]);
   }

   for(uint i = 0; i < node->children.size(); ++i) {
      if(node->children[i]->symbol == TOK_VAR) {
         if(node->children[i]->children[0]->symbol != TOK_STRING) {
            print_var(node->children[i]);
            gvars.push_back(node->children[i]);
         }
      }
   }

   for(uint i = 0; i < node->children.size(); ++i) {
      if(node->children[i]->symbol == TOK_FUNC) {
         print_function(node->children[i]);
      }
   }


   fprintf(out, "void __ocmain (void)\n{\n");
   for(uint i = 0; i < gvars.size(); ++i) {
      create_stmt(gvars[i], true).c_str();
   }
   string status;
   for(uint i = 0; i < node->children.size(); ++i) {
      if(node->children[i]->symbol != TOK_FUNC
         && node->children[i]->symbol != TOK_PROTO
         && node->children[i]->symbol != TOK_VAR
         && node->children[i]->symbol != TOK_STRUCT) {
         status = traverse_block(node->children[i]);
         if(status == "ERROR") return false;
      }
   }
   fprintf(out, "}\n");
   return true;
}
