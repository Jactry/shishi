/*
 *      Copyright (C) 2002  Fabio Fiorina
 *
 * This file is part of LIBASN1.
 *
 * The LIBTASN1 library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public   
 * License as published by the Free Software Foundation; either 
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */


/*****************************************************/
/* File: structure.c                                 */
/* Description: Functions to create and delete an    */
/*  ASN1 tree.                                       */
/*****************************************************/


#include <int.h>
#include <errors.h>
#include <structure.h>
#include "parser_aux.h"
#include "der.h"
#include <gstr.h>


extern char _asn1_identifierMissing[];


/******************************************************/
/* Function : _asn1_add_node_only                     */
/* Description: creates a new NODE_ASN element.       */ 
/* Parameters:                                        */
/*   type: type of the new element (see TYPE_         */
/*         and CONST_ constants).                     */
/* Return: pointer to the new element.                */
/******************************************************/
node_asn *
_asn1_add_node_only(unsigned int type)
{
  node_asn *punt;

  punt=(node_asn *) _asn1_malloc(sizeof(node_asn));
  if (punt==NULL) return NULL;
  
  punt->left=NULL;
  punt->name=NULL;
  punt->type=type; 
  punt->value=NULL;
  punt->down=NULL;
  punt->right=NULL; 

  return punt;
}


/******************************************************************/
/* Function : _asn1_find_left                                     */
/* Description: returns the NODE_ASN element with RIGHT field that*/
/*              points the element NODE.                          */
/* Parameters:                                                    */
/*   node: NODE_ASN element pointer.                              */
/* Return: NULL if not found.                                     */
/******************************************************************/
node_asn *
_asn1_find_left(node_asn *node)
{
  if((node==NULL) || (node->left==NULL) || 
     (node->left->down==node)) return NULL;

  return node->left;  
}


asn1_retCode
_asn1_create_static_structure(ASN1_TYPE pointer,char* output_file_name,char *vector_name)
{
  FILE *file;  
  node_asn *p;
  unsigned long t;

  file=fopen( output_file_name,"w");
  
  if(file==NULL) return ASN1_FILE_NOT_FOUND;

  fprintf(file,"\n#include \"libtasn1.h\"\n\n");
  fprintf(file,"const ASN1_ARRAY_TYPE %s[]={\n",vector_name);

  p=pointer;

  while(p){
    fprintf(file,"  {");

    if(p->name) fprintf(file,"\"%s\",",p->name);
    else fprintf(file,"0,");
    
   t=p->type;
   if(p->down) t|=CONST_DOWN;
   if(p->right) t|=CONST_RIGHT;
   
   fprintf(file,"%lu,",t);
   
   if(p->value) fprintf(file,"\"%s\"},\n",p->value);
   else fprintf(file,"0},\n");

   if(p->down){
     p=p->down;
   }
   else if(p->right){
     p=p->right;
   }
   else{
     while(1){
       p=_asn1_find_up(p);
       if(p==pointer){
	 p=NULL;
	 break;
       }
       if(p->right){
	 p=p->right;
	 break;
       }
     }
   }
 }

 fprintf(file,"  {0,0,0}\n};\n");

 fclose(file);

 return ASN1_SUCCESS;
}


/**
  * asn1_array2tree - Creates the structures needed to manage the ASN1 definitions.
  * @array: specify the array that contains ASN.1 declarations
  * @definitions: return the pointer to the structure created by *ARRAY ASN.1 declarations
  * @errorDescription : return the error description.
  * Description:
  *
  * Creates the structures needed to manage the ASN1 definitions. ARRAY is a vector created by
  * 'asn1_parser_asn1_file_c' function.
  *
  * Returns:
  *
  *  ASN1_SUCCESS\: structure created correctly. 
  *
  * ASN1_ELEMENT_NOT_EMPTY\: *DEFINITIONS not ASN1_TYPE_EMPTY
  *
  * ASN1_IDENTIFIER_NOT_FOUND\: in the file there is an identifier that is not defined (see ERRORDESCRIPTION for more information).
  *
  *  ASN1_ARRAY_ERROR\: the array pointed by ARRAY is wrong.  
  **/
asn1_retCode
asn1_array2tree(const ASN1_ARRAY_TYPE *array,ASN1_TYPE *definitions,
                 char *errorDescription)
{
  node_asn *p,*p_last=NULL;
  unsigned long k;
  int move;
  asn1_retCode result;


  if(*definitions != ASN1_TYPE_EMPTY)
    return ASN1_ELEMENT_NOT_EMPTY;

  move=UP;

  k=0;
  while(array[k].value || array[k].type || array[k].name){
    p=_asn1_add_node(array[k].type&(~CONST_DOWN));
    if(array[k].name) _asn1_set_name(p,array[k].name);
    if(array[k].value) _asn1_set_value(p,array[k].value,
                                       strlen(array[k].value)+1);

    if(*definitions==NULL) *definitions=p;

    if(move==DOWN) _asn1_set_down(p_last,p);
    else if(move==RIGHT) _asn1_set_right(p_last,p);

    p_last=p;

    if(array[k].type&CONST_DOWN) move=DOWN;
    else if(array[k].type&CONST_RIGHT) move=RIGHT;
    else{
      while(1){
	if(p_last==*definitions) break;
   
	p_last= _asn1_find_up(p_last);

	if(p_last==NULL) break;

	if(p_last->type&CONST_RIGHT){
	  p_last->type&=~CONST_RIGHT;
	  move=RIGHT;
	  break;
	}
      }  /* while */
    }
    k++;
  }  /* while */

  if(p_last==*definitions){
    result=_asn1_check_identifier(*definitions);
    if(result==ASN1_SUCCESS){
      _asn1_change_integer_value(*definitions);
      _asn1_expand_object_id(*definitions);
    }
  }
  else{
    result=ASN1_ARRAY_ERROR;
  }

  if (errorDescription!=NULL) {
   if(result==ASN1_IDENTIFIER_NOT_FOUND) {
     Estrcpy(errorDescription,":: identifier '");
     Estrcat(errorDescription,_asn1_identifierMissing);
     Estrcat(errorDescription,"' not found");
   }
   else
     errorDescription[0]=0;
  }
  
  if(result != ASN1_SUCCESS){
    _asn1_delete_list_and_nodes();
    *definitions=ASN1_TYPE_EMPTY;
  }
  else
    _asn1_delete_list();

  return result;
}


/**
  * asn1_delete_structure - Deletes the structure pointed by *ROOT. 
  * @structure: pointer to the structure that you want to delete.
  * Description:
  * 
  * Deletes the structure *ROOT. 
  * At the end *ROOT is setted to ASN1_TYPE_EMPTY. 
  * 
  * Returns:
  *
  *   ASN1_SUCCESS\: everything OK
  *
  *   ASN1_ELEMENT_NOT_FOUND\: *root==ASN1_TYPE_EMPTY.
  *
  **/
asn1_retCode
asn1_delete_structure(ASN1_TYPE *structure)
{
  node_asn *p,*p2,*p3;

  if(*structure==ASN1_TYPE_EMPTY) return ASN1_ELEMENT_NOT_FOUND;

  p=*structure;
  while(p){
    if(p->down){
      p=p->down;
    }
    else{   /* no down */
      p2=p->right;
      if(p!=*structure){
	p3=_asn1_find_up(p);
	_asn1_set_down(p3,p2);
	_asn1_remove_node(p);
	p=p3;
      }
      else{   /* p==root */
	p3=_asn1_find_left(p);
	if(!p3){
	  p3=_asn1_find_up(p);
	  if(p3) _asn1_set_down(p3,p2);
	  else{
	    if(p->right) p->right->left=NULL;
	  }
	}
	else _asn1_set_right(p3,p2);
	_asn1_remove_node(p);
	p=NULL;
      }
    }
  }

  *structure=ASN1_TYPE_EMPTY;
  return ASN1_SUCCESS;
}



node_asn *
_asn1_copy_structure3(node_asn *source_node)
{
  node_asn *dest_node,*p_s,*p_d,*p_d_prev;
  int len,len2,move;

  if(source_node==NULL) return NULL;

  dest_node=_asn1_add_node_only(source_node->type);

  p_s=source_node;
  p_d=dest_node;

  move=DOWN;

  do{
    if(move!=UP){
      if(p_s->name) _asn1_set_name(p_d,p_s->name);
      if(p_s->value){
	switch(type_field(p_s->type)){
	case TYPE_OCTET_STRING: case TYPE_BIT_STRING: case TYPE_GENERALSTRING: 
	case TYPE_INTEGER:    
	  len2=-1;
	  len=_asn1_get_length_der(p_s->value,&len2);
	  _asn1_set_value(p_d,p_s->value,len+len2);
	  break;
	default:
	  _asn1_set_value(p_d,p_s->value,strlen(p_s->value)+1);
	}
      }
      move=DOWN;
    }
    else move=RIGHT;

    if(move==DOWN){
      if(p_s->down){
	p_s=p_s->down;
	p_d_prev=p_d;      
	p_d=_asn1_add_node_only(p_s->type);
	_asn1_set_down(p_d_prev,p_d);
      }
      else move=RIGHT;
    }
  
    if(p_s==source_node) break;

    if(move==RIGHT){
      if(p_s->right){
	p_s=p_s->right;
	p_d_prev=p_d;
	p_d=_asn1_add_node_only(p_s->type);
	_asn1_set_right(p_d_prev,p_d);
      }
      else move=UP;
    }
    if(move==UP){
      p_s=_asn1_find_up(p_s);
      p_d=_asn1_find_up(p_d);
    }
  }while(p_s!=source_node);

  return dest_node;
}


node_asn *
_asn1_copy_structure2(node_asn *root,const char *source_name)
{
  node_asn *source_node;

  source_node=_asn1_find_node(root,source_name);
  
  return _asn1_copy_structure3(source_node);

}


asn1_retCode 
_asn1_type_choice_config(node_asn *node)
{
  node_asn *p,*p2,*p3,*p4;
  int move;
 
  if(node==NULL) return ASN1_ELEMENT_NOT_FOUND;

  p=node;
  move=DOWN;

  while(!((p==node) && (move==UP))){
    if(move!=UP){
      if((type_field(p->type)==TYPE_CHOICE) &&
	 (p->type&CONST_TAG)){
	p2=p->down;
	while(p2){
	  if(type_field(p2->type)!=TYPE_TAG){
	    p2->type|=CONST_TAG;
	    p3=_asn1_find_left(p2);
	    while(p3){
	      if(type_field(p3->type)==TYPE_TAG){
		p4=_asn1_add_node_only(p3->type);
		_asn1_set_value(p4,p3->value,strlen(p3->value)+1);
		_asn1_set_right(p4,p2->down);
		_asn1_set_down(p2,p4);
	      }
	      p3=_asn1_find_left(p3);
	    }
	  }
	  p2=p2->right;
	}
	p->type&=~(CONST_TAG);
	p2=p->down;
	while(p2){
	  p3=p2->right;
	  if(type_field(p2->type)==TYPE_TAG) asn1_delete_structure(&p2);
	  p2=p3;
	}
      }
      move=DOWN;
    }
    else move=RIGHT;
    
    if(move==DOWN){
      if(p->down) p=p->down;
      else move=RIGHT;
    }
    
    if(p==node) {move=UP; continue;}
    
    if(move==RIGHT){
      if(p->right) p=p->right;
      else move=UP;
    }
    if(move==UP) p=_asn1_find_up(p);
  }
  
  return ASN1_SUCCESS;
}


asn1_retCode 
_asn1_expand_identifier(node_asn **node,node_asn *root)
{
  node_asn *p,*p2,*p3;
  char name2[MAX_NAME_SIZE+2];
  int move;
 
  if(node==NULL) return ASN1_ELEMENT_NOT_FOUND;

  p=*node;
  move=DOWN;

  while(!((p==*node) && (move==UP))){
    if(move!=UP){
      if(type_field(p->type)==TYPE_IDENTIFIER){
	_asn1_str_cpy(name2, sizeof(name2), root->name);
	_asn1_str_cat(name2, sizeof(name2), ".");
	_asn1_str_cat(name2, sizeof(name2), p->value);
	p2=_asn1_copy_structure2(root,name2);
	if(p2==NULL){
	  return ASN1_IDENTIFIER_NOT_FOUND;
	}
	_asn1_set_name(p2,p->name);
	p2->right=p->right;
	p2->left=p->left;
	if(p->right) p->right->left=p2;
	p3=p->down;
	if(p3){
	  while(p3->right) p3=p3->right;
	  _asn1_set_right(p3,p2->down);
	  _asn1_set_down(p2,p->down);
	}
	
	p3=_asn1_find_left(p);
	if(p3) _asn1_set_right(p3,p2);
	else{
	  p3=_asn1_find_up(p);
	  if(p3) _asn1_set_down(p3,p2);
	  else {
	    p2->left=NULL;
	  }
	}

	if(p->type & CONST_SIZE) p2->type|=CONST_SIZE;
	if(p->type & CONST_TAG) p2->type|=CONST_TAG;
	if(p->type & CONST_OPTION) p2->type|=CONST_OPTION;
	if(p->type & CONST_DEFAULT) p2->type|=CONST_DEFAULT;
	if(p->type & CONST_SET) p2->type|=CONST_SET;
	if(p->type & CONST_NOT_USED) p2->type|=CONST_NOT_USED;

	if(p==*node) *node=p2;
	_asn1_remove_node(p);
	p=p2;
	move=DOWN;
	continue;
      }
      move=DOWN;
    }
    else move=RIGHT;
    
    if(move==DOWN){
      if(p->down) p=p->down;
      else move=RIGHT;
    }
    
    if(p==*node) {move=UP; continue;}
    
    if(move==RIGHT){
      if(p->right) p=p->right;
      else move=UP;
    }
    if(move==UP) p=_asn1_find_up(p);
  }

  return ASN1_SUCCESS;
}


/**
  * asn1_create_element - Creates a structure of type SOURCE_NAME.
  * @definitions: pointer to the structure returned by "parser_asn1" function 
  * @source_name: the name of the type of the new structure (must be inside p_structure).
  * @element: pointer to the structure created. 
  * Description:
  *
  * Creates a structure called DEST_NAME of type SOURCE_NAME.
  *
  * Returns:
  *
  *  ASN1_SUCCESS\: creation OK
  *
  *  ASN1_ELEMENT_NOT_FOUND\: SOURCE_NAME isn't known
  * 
  * Example: using "pkix.asn"
  *  result=asn1_create_structure(cert_def,"PKIX1.Certificate",&cert);
  **/
asn1_retCode
asn1_create_element(ASN1_TYPE definitions,const char *source_name,
		    ASN1_TYPE *element)
{
  node_asn *dest_node;
  int res;

  dest_node=_asn1_copy_structure2(definitions,source_name);
 
  if(dest_node==NULL) return ASN1_ELEMENT_NOT_FOUND;

  _asn1_set_name(dest_node,"");

  res=_asn1_expand_identifier(&dest_node,definitions);
  _asn1_type_choice_config(dest_node);

  *element=dest_node;

  return res;
}


/**
  * asn1_print_structure - Prints on the standard output the structure's tree
  * @out: pointer to the output file (e.g. stdout).
  * @structure: pointer to the structure that you want to visit.
  * @name: an element of the structure
  * 
  * Prints on the standard output the structure's tree starting from the NAME element inside
  * the structure *POINTER. 
  **/
void
asn1_print_structure(FILE *out,ASN1_TYPE structure,const char *name,int mode)
{
  node_asn *p,*root;
  int k,indent=0,len,len2,len3;

  if(out==NULL) return;

  root=_asn1_find_node(structure,name);   

  if(root==NULL) return;

  p=root;
  while(p){
    if(mode == ASN1_PRINT_ALL){
      for(k=0;k<indent;k++)fprintf(out," ");
      fprintf(out,"name:");
      if(p->name) fprintf(out,"%s  ",p->name);
      else fprintf(out,"NULL  ");
    }
    else{
      switch(type_field(p->type)){
      case TYPE_CONSTANT:
      case TYPE_TAG:
      case TYPE_SIZE:
	break;
      default:
	for(k=0;k<indent;k++)fprintf(out," ");
	fprintf(out,"name:");
	if(p->name) fprintf(out,"%s  ",p->name);
	else fprintf(out,"NULL  ");
      }
    }

    if(mode != ASN1_PRINT_NAME){
      switch(type_field(p->type)){
      case TYPE_CONSTANT:
	if(mode == ASN1_PRINT_ALL)
	  fprintf(out,"type:CONST");break;
      case TYPE_TAG:
	if(mode == ASN1_PRINT_ALL)
	  fprintf(out,"type:TAG");break;
      case TYPE_SIZE:
	if(mode == ASN1_PRINT_ALL)
	  fprintf(out,"type:SIZE");break;
      case TYPE_DEFAULT:
	fprintf(out,"type:DEFAULT");break;
      case TYPE_NULL:
	fprintf(out,"type:NULL");break;
      case TYPE_IDENTIFIER:
	fprintf(out,"type:IDENTIFIER");break;
      case TYPE_INTEGER:
	fprintf(out,"type:INTEGER");break;
      case TYPE_ENUMERATED:
	fprintf(out,"type:ENUMERATED");break;
      case TYPE_TIME:
	fprintf(out,"type:TIME");break;
      case TYPE_BOOLEAN:
	fprintf(out,"type:BOOLEAN");break;
      case TYPE_SEQUENCE:
	fprintf(out,"type:SEQUENCE");break;
      case TYPE_BIT_STRING:
	fprintf(out,"type:BIT_STR");break;
      case TYPE_OCTET_STRING:
	fprintf(out,"type:OCT_STR");break;
      case TYPE_GENERALSTRING:
	fprintf(out,"type:GENERALSTRING");break;
      case TYPE_SEQUENCE_OF:
	fprintf(out,"type:SEQ_OF");break;
      case TYPE_OBJECT_ID:
	fprintf(out,"type:OBJ_ID");break;
      case TYPE_ANY:
	fprintf(out,"type:ANY");break;
      case TYPE_SET:
	fprintf(out,"type:SET");break;
      case TYPE_SET_OF:
	fprintf(out,"type:SET_OF");break;
      case TYPE_CHOICE:
	fprintf(out,"type:CHOICE");break;
      case TYPE_DEFINITIONS:
	fprintf(out,"type:DEFINITIONS");break;
      default:
	break;
      }
    }

    if((mode == ASN1_PRINT_NAME_TYPE_VALUE) ||
       (mode == ASN1_PRINT_ALL)){
      switch(type_field(p->type)){
      case TYPE_CONSTANT:
	if(mode == ASN1_PRINT_ALL)
	  if(p->value) fprintf(out,"  value:%s",p->value);
	break;
      case TYPE_TAG:
	if(mode == ASN1_PRINT_ALL)
	  fprintf(out,"  value:%s",p->value);
	break;
      case TYPE_SIZE:
	if(mode == ASN1_PRINT_ALL)
	  if(p->value) fprintf(out,"  value:%s",p->value);
	break;
      case TYPE_DEFAULT:
	if(p->value) fprintf(out,"  value:%s",p->value);
	else if(p->type & CONST_TRUE)  fprintf(out,"  value:TRUE");
	else if(p->type & CONST_FALSE) fprintf(out,"  value:FALSE");
	break;
      case TYPE_IDENTIFIER:
	if(p->value) fprintf(out,"  value:%s",p->value);
	break;
      case TYPE_INTEGER:
	if(p->value){
	  len2=-1;
	  len=_asn1_get_length_der(p->value,&len2);
	  fprintf(out,"  value:0x");
	  for(k=0;k<len;k++) fprintf(out,"%02x",(p->value)[k+len2]);
	}
	break;
      case TYPE_ENUMERATED:
	if(p->value){
	  len2=-1;
	  len=_asn1_get_length_der(p->value,&len2);
	  fprintf(out,"  value:0x");
	  for(k=0;k<len;k++) fprintf(out,"%02x",(p->value)[k+len2]);
	}
	break;
      case TYPE_TIME:
	if(p->value) fprintf(out,"  value:%s",p->value);
	break;
      case TYPE_BOOLEAN:
	if(p->value){
	  if(p->value[0]=='T') fprintf(out,"  value:TRUE");
	  else if(p->value[0]=='F') fprintf(out,"  value:FALSE");
	}
	break;
      case TYPE_BIT_STRING:
	if(p->value){
	  len2=-1;
	  len=_asn1_get_length_der(p->value,&len2);
	  fprintf(out,"  value(%i):",(len-1)*8-(p->value[len2]));
	  for(k=1;k<len;k++) fprintf(out,"%02x",(p->value)[k+len2]);
	}
	break;
      case TYPE_OCTET_STRING:
	if(p->value){
	  len2=-1;
	  len=_asn1_get_length_der(p->value,&len2);
	  fprintf(out,"  value:");
	  for(k=0;k<len;k++) fprintf(out,"%02x",(p->value)[k+len2]);
	}
	break;
      case TYPE_GENERALSTRING:
	if(p->value){
	  len2=-1;
	  len=_asn1_get_length_der(p->value,&len2);
	  fprintf(out,"  value:");
	  for(k=0;k<len;k++) fprintf(out,"%02x",(p->value)[k+len2]);
	}
	break;
      case TYPE_OBJECT_ID:
	if(p->value) fprintf(out,"  value:%s",p->value);
	break;
      case TYPE_ANY:
	if(p->value){
	  len3=-1;
	  len2=_asn1_get_length_der(p->value,&len3);
	  fprintf(out,"  value:");
	  for(k=0;k<len2;k++) fprintf(out,"%02x",(p->value)[k+len3]);
	}
	break;
      case TYPE_SET:
      case TYPE_SET_OF:
      case TYPE_CHOICE:
      case TYPE_DEFINITIONS:
      case TYPE_SEQUENCE_OF:
      case TYPE_SEQUENCE:
      case TYPE_NULL:
	break;
      default:
	break;
      }
    }

    if(mode==ASN1_PRINT_ALL){
      if(p->type&0x1FFFFF00){
	fprintf(out,"  attr:");
	if(p->type & CONST_UNIVERSAL) fprintf(out,"UNIVERSAL,");
	if(p->type & CONST_PRIVATE) fprintf(out,"PRIVATE,");
	if(p->type & CONST_APPLICATION) fprintf(out,"APPLICATION,");
	if(p->type & CONST_EXPLICIT) fprintf(out,"EXPLICIT,");
	if(p->type & CONST_IMPLICIT) fprintf(out,"IMPLICIT,");
	if(p->type & CONST_TAG) fprintf(out,"TAG,");
	if(p->type & CONST_DEFAULT) fprintf(out,"DEFAULT,");
	if(p->type & CONST_TRUE) fprintf(out,"TRUE,");
	if(p->type & CONST_FALSE) fprintf(out,"FALSE,");
	if(p->type & CONST_LIST) fprintf(out,"LIST,");
	if(p->type & CONST_MIN_MAX) fprintf(out,"MIN_MAX,");
	if(p->type & CONST_OPTION) fprintf(out,"OPTION,");
	if(p->type & CONST_1_PARAM) fprintf(out,"1_PARAM,");
	if(p->type & CONST_SIZE) fprintf(out,"SIZE,");
	if(p->type & CONST_DEFINED_BY) fprintf(out,"DEF_BY,");
	if(p->type & CONST_GENERALIZED) fprintf(out,"GENERALIZED,");
	if(p->type & CONST_UTC) fprintf(out,"UTC,");
	if(p->type & CONST_SET) fprintf(out,"SET,");
	if(p->type & CONST_NOT_USED) fprintf(out,"NOT_USED,");
	if(p->type & CONST_ASSIGN) fprintf(out,"ASSIGNMENT,");
      }
    }

    if(mode == ASN1_PRINT_ALL){
      fprintf(out,"\n");
    }
    else{
      switch(type_field(p->type)){
      case TYPE_CONSTANT:
      case TYPE_TAG:
      case TYPE_SIZE:
	break;
      default:
	fprintf(out,"\n");
      }
    }

    if(p->down){
      p=p->down;
      indent+=2;
    }
    else if(p==root){
      p=NULL;
      break;
    }
    else if(p->right) p=p->right;
    else{
      while(1){
	p=_asn1_find_up(p);
	if(p==root){
	  p=NULL;
	  break;
	}
	indent-=2;
	if(p->right){
	  p=p->right;
	  break;
	}
      }
    }
  }
}



/**
  * asn1_number_of_elements - Counts the number of elements of a structure.
  * @element: pointer to the root of an ASN1 structure. 
  * @name: the name of a sub-structure of ROOT.
  * @num: pointer to an integer where the result will be stored 
  * Description:
  *
  * Counts the number of elements of a sub-structure called NAME with names equal to "?1","?2", ...
  *
  * Returns:
  *
  *  ASN1_SUCCESS: creation OK
  *  ASN1_ELEMENT_NOT_FOUND: NAME isn't known
  *  ASN1_GENERIC_ERROR: pointer num equal to NULL
  *
  **/
asn1_retCode 
asn1_number_of_elements(ASN1_TYPE element,const char *name,int *num)
{
  node_asn *node,*p;

  if(num==NULL) return ASN1_GENERIC_ERROR;

  *num=0;

  node=_asn1_find_node(element,name);
  if(node==NULL) return ASN1_ELEMENT_NOT_FOUND;

  p=node->down;

  while(p){
    if((p->name) && (p->name[0]=='?')) (*num)++; 
    p=p->right;
  }

  return ASN1_SUCCESS;
}









