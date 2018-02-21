/* libdescrip - Vertical grid descriptor library for FORTRAN programming
 * Copyright (C) 2016  Direction du developpement des previsions nationales
 *                     Centre meteorologique canadien
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation,
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "vgrid.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "rpnmacros.h"

#define STR_INIT(str,len) if(len>1) memset(str,' ',len-1); if(len>0) str[len-1] = '\0'

// Constants
#define MAX_DESC_REC 10000      //maximum number of descriptor records in a single file
#define MAX_VKIND    100
#define ZNAME "!!"              //name of the vertical coodinate
// Macros
#define FREE(x) if(x) { free(x); x=NULL; }

// Options
static int ALLOW_SIGMA = 0;

// Validity table for self
#define VALID_TABLE_SIZE 9

static int ptop_out_8_valid [VALID_TABLE_SIZE] = {    0,    0,    0,    0,    0,    0,    0, 5004, 5005};
static int ptop_8_valid     [VALID_TABLE_SIZE] = {    0, 1002, 1003,    0, 5001, 5002, 5003, 5004,    0};
static int pref_8_valid     [VALID_TABLE_SIZE] = {    0,    0,    0, 1003, 5001, 5002, 5003, 5004, 5005};
static int rcoef1_valid     [VALID_TABLE_SIZE] = {    0,    0,    0, 1003, 5001, 5002, 5003, 5004, 5005};
static int rcoef2_valid     [VALID_TABLE_SIZE] = {    0,    0,    0,    0,    0, 5002, 5003, 5004, 5005};
static int a_m_8_valid      [VALID_TABLE_SIZE] = { 1001, 1002, 1003, 2001, 5001, 5002, 5003, 5004, 5005};
static int b_m_8_valid      [VALID_TABLE_SIZE] = { 1001, 1002, 1003, 2001, 5001, 5002, 5003, 5004, 5005};
static int a_t_8_valid      [VALID_TABLE_SIZE] = {    0,    0,    0,    0,    0, 5002, 5003, 5004, 5005};
static int a_t_8_valid_get  [VALID_TABLE_SIZE] = { 1001, 1002,    0, 2001, 5001, 5002, 5003, 5004, 5005};
static int b_t_8_valid      [VALID_TABLE_SIZE] = {    0,    0,    0,    0,    0, 5002, 5003, 5004, 5005};
static int b_t_8_valid_get  [VALID_TABLE_SIZE] = { 1001, 1002,    0, 2001, 5001, 5002, 5003, 5004, 5005};
static int ip1_m_valid      [VALID_TABLE_SIZE] = { 1001, 1002, 1003, 2001, 5001, 5002, 5003, 5004, 5005};
static int ip1_t_valid      [VALID_TABLE_SIZE] = {    0,    0,    0,    0,    0, 5002, 5003, 5004, 5005};
static int ip1_t_valid_get  [VALID_TABLE_SIZE] = { 1001, 1002,    0, 2001, 5001, 5002, 5003, 5004, 5005};
static int ref_name_valid   [VALID_TABLE_SIZE] = { 1001, 1002, 1003,    0, 5001, 5002, 5003, 5004, 5005};
static int dhm_valid        [VALID_TABLE_SIZE] = {    0,    0,    0,    0,    0,    0,    0,    0, 5005};
static int dht_valid        [VALID_TABLE_SIZE] = {    0,    0,    0,    0,    0,    0,    0,    0, 5005};
static int is_in_logp       [VALID_TABLE_SIZE] = {    0,    0,    0,    0,    0, 5002, 5003, 5004, 5005};

static int c_encode_vert_5002_5003_5004_5005(vgrid_descriptor **self, char update);
static int fstd_init(vgrid_descriptor *VGrid);
static vgrid_descriptor* c_vgd_construct();
static int c_encode_vert_1001(vgrid_descriptor **self,int nk);
static int c_encode_vert_1002(vgrid_descriptor **self,int nk);
static int c_encode_vert_2001(vgrid_descriptor **self,int nk);
static int c_encode_vert_5001(vgrid_descriptor **self,int nk);
static int c_encode_vert_5002_5003_5004_5005(vgrid_descriptor **self, char update);

static int is_valid(vgrid_descriptor *self, int *table_valid)
{
  int k;
  for( k = 0; k < VALID_TABLE_SIZE; k++){
    if(self->vcode == table_valid[k]){
      return(1);
    }
  }
  return 0;
}

int Cvgd_is_valid(vgrid_descriptor *self, char *valid_table_name)
{
  if(! self){
    printf("(Cvgd) ERROR in Cvgd_is_valid, vgrid descriptor not constructed\n");
    return(0);
  }
  if( strcmp(valid_table_name, "SELF") == 0 ){
    return(self->valid);
  } else if( strcmp(valid_table_name, "ptop_out_8_valid") == 0 ){
    return(is_valid(self,              ptop_out_8_valid));
  } else if( strcmp(valid_table_name, "ptop_8_valid")     == 0 ){
    return(is_valid(self,              ptop_8_valid));
  } else if( strcmp(valid_table_name, "pref_8_valid")     == 0 ){
    return(is_valid(self,              pref_8_valid));
  } else if( strcmp(valid_table_name, "rcoef1_valid")     == 0 ){
    return(is_valid(self,              rcoef1_valid));
  } else if( strcmp(valid_table_name, "rcoef2_valid")     == 0 ){
    return(is_valid(self,              rcoef2_valid));
  } else if( strcmp(valid_table_name, "a_m_8_valid")      == 0 ){
    return(is_valid(self,              a_m_8_valid));
  } else if( strcmp(valid_table_name, "b_m_8_valid")      == 0 ){
    return(is_valid(self,              b_m_8_valid));
  } else if( strcmp(valid_table_name, "a_t_8_valid")      == 0 ){
    return(is_valid(self,              a_t_8_valid));
  } else if( strcmp(valid_table_name, "a_t_8_valid_get")  == 0 ){
    return(is_valid(self,              a_t_8_valid_get));
  } else if( strcmp(valid_table_name, "b_t_8_valid")      == 0 ){
    return(is_valid(self,              b_t_8_valid));
  } else if( strcmp(valid_table_name, "b_t_8_valid_get")  == 0 ){
    return(is_valid(self,              b_t_8_valid_get));
  } else if( strcmp(valid_table_name, "ip1_m_valid")      == 0 ){
    return(is_valid(self,              ip1_m_valid));
  } else if( strcmp(valid_table_name, "ip1_t_valid")      == 0 ){
    return(is_valid(self,              ip1_t_valid));
  } else if( strcmp(valid_table_name, "ip1_t_valid_get")  == 0 ){
    return(is_valid(self,              ip1_t_valid_get));
  } else if( strcmp(valid_table_name, "ref_name_valid")   == 0 ){
    return(is_valid(self,              ref_name_valid));
  } else if( strcmp(valid_table_name, "dhm_valid")        == 0 ){
    return(is_valid(self,              dhm_valid));
  } else if( strcmp(valid_table_name, "dht_valid")        == 0 ){
    return(is_valid(self,              dht_valid));
  } else if( strcmp(valid_table_name, "is_in_logp")        == 0 ){
    return(is_valid(self,              is_in_logp));
  } else {
    printf("(Cvgd) Warning : in Cvgd_is_valid, valid_table_name '%s' does not exist\n",valid_table_name);
    return(0);
  }
}

static int is_required_double(vgrid_descriptor *self, double *ptr, int *table_valid, char *message) {
  if( is_valid(self,table_valid)) {
    if (! ptr) {
      printf("(Cvgd) ERROR: %s is a required constructor entry\n", message);
      return(0);
    }
  } else {
    if (ptr) {
      printf("(Cvgd) ERROR: %s is not a required constructor entry\n", message);
      return(0);
    }
  }
  return(1);
}
static int is_required_float(vgrid_descriptor *self, float *ptr, int *table_valid, char *message) {
  if( is_valid(self,table_valid)) {
    if (! ptr) {
      printf("(Cvgd) ERROR: %s is a required constructor entry\n", message);
      return(0);
    }
  } else {
    if (ptr) {
      printf("(Cvgd) ERROR: %s is not a required constructor entry\n", message);
      return(0);
    }
  }
  return(1);
}

static int my_alloc_int(int **vec, int size, char *message){
  *vec = malloc ( size * sizeof(int) );
  if(! *vec){    
    printf("%s %d\n",message, size);
    return(VGD_ERROR);
  }
  return(VGD_OK);
}

static int my_alloc_float(float **vec, int size, char *message){
  *vec = malloc ( size * sizeof(float) );
  if(! *vec){    
    printf("%s %d\n",message, size);
    return(VGD_ERROR);
  }
  return(VGD_OK);
}

static int my_alloc_double(double **vec, int size, char *message){
  *vec = malloc ( size * sizeof(double) );
  if(! *vec){    
    printf("%s %d\n",message, size);
    return(VGD_ERROR);
  }
  return(VGD_OK);
}

static int is_bigendian() {
  int n = 1;
  // little endian if true
  if(*(char *)&n == 1) {
    return 1;
  } else {
    return 0;
  }
}

static double reverseDouble (char *c) {
    double d;
    char *p = (char *)&d;
    p[0] = c[7];
    p[1] = c[6];
    p[2] = c[5];
    p[3] = c[4];
    p[4] = c[3];
    p[5] = c[2];
    p[6] = c[1];
    p[7] = c[0];
    return d;
}

static void flip_transfer_d2c(char *name, double val_8) {
  union {
    double d;
    char c[sizeof(double)];
  } u;
  u.d = val_8;
  if(! is_bigendian()) {
    u.d = reverseDouble(u.c);
  }
  strncpy(name, u.c, 4);
}

static void flip_transfer_c2d(char *name, double *val_8) {
  // TODO, pas certain que cela soit ok.
  // les valeurs de u.d semblent etranges
  union {
    double d;
    char c[sizeof(double)];
  } u;
  strcpy(u.c,"ABCDEFGH");
  strncpy(u.c, name, 4);
  if(! is_bigendian()) {
    u.d = reverseDouble(u.c);
  }
  *val_8 = u.d;
}

static int max_int(int *vec, int ni) {
  int i, ind = 0;
  for( i = 1; i < ni; i++){
    if( vec[i] > vec[ind] )
      ind = i;
  }
  return(vec[ind]);
}

static double c_get_error(char *key, int quiet) {
  if (! quiet) {
    printf("(Cvgd) ERROR in c_get_error, attempt to retrieve invalid key %s\n",key);
  }
  return(VGD_MISSING);
}

void Cvgd_table_shape(vgrid_descriptor *self, int **tshape) {
  (*tshape)[0] = self->table_ni;
  (*tshape)[1] = self->table_nj;
  (*tshape)[2] = self->table_nk;
}

static int c_table_update(vgrid_descriptor **self) {
  switch((*self)->vcode) {
  case 5002:
  case 5003:
  case 5004:
  case 5005:
    if( c_encode_vert_5002_5003_5004_5005(self, 1) == VGD_ERROR ) {
      printf("(Cvgd) ERROR in c_table_update, cannot encode\n");
      return(VGD_ERROR);
    }
    break;
  default:
    printf("(Cvgd) ERROR in c_table_update, unsupported Vcode %d",(*self)->vcode);
    return(VGD_ERROR);
  }	
  return(VGD_OK);  
}

static void my_copy_double(double *aa, double **bb, int ind){
  while (ind--) {
    (*bb)[ind] = aa[ind];
  }
}

static void my_copy_int(int *aa, int **bb, int ind){
  while (ind--) {
    (*bb)[ind] = aa[ind];
  }
}

static int same_vec_i(int *vec1, int n1, int *vec2, int n2) {
  int i;
  if(vec1) {
    if (vec2) {
      if ( n1 == n2 ) {
	for(i = 0; i < n1; i++) {
	  if ( vec1[i] != vec2[i] ) return(-1);
	}
      } else {
	// Vectors are not the same size.
	return(-2);
      }
    } else {
      // vec2 not allocated
      return(-3);
    }
  }
  // Vector are the same or are not allocated.
  return(0);
}

static int same_vec_r8(double *vec1, int n1, double *vec2, int n2) {
  if(vec1) {
    if (vec2) {
      if ( n1 == n2 ) {	
	//for(i = 0; i < n1; i++) {
	//  if ( vec1[i] != vec2[i] ) return(-1);
	//}
	if( memcmp( vec1, vec2, n1*sizeof(double)/sizeof(char) ) ) 
	  return(-1);
      } else {
	// Vectors are not the same size.
	return(-2);
      }
    } else {
      // vec2 not allocated
      return(-3);
    }
  }
  // Vector are the same or are not allocated.
  return(0);
}

static int c_convip_Level2IP(float level, int kind) {

  int    mode=2,flag=0, IP; 
  char   format;
  
  // Convertir niveau reel en ip1a
  f77name(convip)(&IP,&level,&kind,&mode,&format,&flag);
    
  return(IP);
}

static int c_convip_Level2IP_old_style(float level, int kind) {

  int    mode=3,flag=0, IP; 
  char   format; 
  
  // Convertir niveau reel en ip1a
  f77name(convip)(&IP,&level,&kind,&mode,&format,&flag);
    
  return(IP);
}

static float c_convip_IP2Level(int IP,int *kind) {

   int    mode=-1,flag=0;
   float  level=0.0;
   char   format;

   /*Convertir en niveau reel*/
    f77name(convip)(&IP,&level,kind,&mode,&format,&flag);

   return(level);
}

static void decode_HY(VGD_TFSTD_ext var, double *ptop_8, double *pref_8, float *rcoef){
  // In consultation with Vivian Lee, with decode explicitly instead of using f77 read_decode_hyb
  int kind;
  *ptop_8 = c_convip_IP2Level(var.ip1, &kind) * 100.;
  *pref_8 = var.ig1 * 100.;
  *rcoef = var.ig2/1000.f;
}

static int my_fstprm(int key,VGD_TFSTD_ext *ff) {
  //var->ip1 = 62;
  STR_INIT(ff->typvar,VGD_MAXSTR_TYPVAR);
  STR_INIT(ff->nomvar,VGD_MAXSTR_NOMVAR);
  STR_INIT(ff->etiket,VGD_MAXSTR_ETIKET);
  STR_INIT(ff->grtyp, VGD_MAXSTR_GRTYP);
 
  if( c_fstprm(key,
	       &ff->dateo,  &ff->deet,   &ff->npas, 
	       &ff->ni,     &ff->nj,     &ff->nk,
	       &ff->nbits,  &ff->datyp,  
	       &ff->ip1,    &ff->ip2,    &ff->ip3,
	        ff->typvar,  ff->nomvar,  ff->etiket,
	        ff->grtyp,  &ff->ig1,    &ff->ig2,    &ff->ig3, &ff->ig4,
	       &ff->swa,    &ff->lng,    &ff->dltf,   &ff->ubc,
	       &ff->extra1, &ff->extra2, &ff->extra3) < 0 ) {
    printf("(Cvgd) ERROR: cannot fstprm for fstkey %d\n",key);
    return(VGD_ERROR);
  }
  return(VGD_OK);
}

static int correct_kind_and_version(int key, int kind, int version, VGD_TFSTD_ext *var, int *status) {
  
  *status=0;
  if( my_fstprm(key, var) == VGD_ERROR ) {
    printf("(Cvgd) ERROR in correct_kind_and_version, with my_fstprm on key %d\n",key);
    return(VGD_ERROR);
  }
  if(kind != -1 && version != -1) {
    if(var->ig1 != kind*1000 + version ) {
      return(VGD_OK);
    }
  } else {
    if(kind != -1) {
      // Get kind from fst vcode (ig1)
      if((int)round( var->ig1 / 1000.) != kind) {
	return(VGD_OK);
      }
    }
    if(version != -1) {
      // Get version from fst vcode (ig1)
      if(var->ig1-(int)round(var->ig1 / 1000.) != version) {
	return(VGD_OK);
      }
    }
  }
  // If we reach this point, we have a match
  *status = 1;
  return(VGD_OK);

}

static int C_load_toctoc(vgrid_descriptor *self, VGD_TFSTD_ext var, int key) {

  int table_size, istat, ni, nj, nk;

  self->table_ni = var.ni;
  self->table_nj = var.nj;
  self->table_nk = var.nk;

  table_size = self->table_ni * self->table_nj * self->table_nk;
  self->table = malloc ( table_size * sizeof(double) );
  if(! self->table ) {
    printf("(Cvgd) ERROR in C_load_toctoc, cannot allocate table of bouble of size %d\n",table_size );
    return(VGD_ERROR);
  }
  istat = c_fstluk(self->table, key, &ni, &nj, &nk);
  if(istat < 0) {
    printf("(Cvgd) ERROR in C_load_toctoc, problem with fstluk\n");
    free(self->table);
    return(VGD_ERROR);
  }
  self->kind             = (int) self->table[0];
  self->version          = (int) self->table[1];
  if(fstd_init(self) == VGD_ERROR) {
    printf("(Cvgd) ERROR in C_load_toctoc, problem creating record information\n");
    return(VGD_ERROR);
  }
  self->rec.dateo        = var.dateo;
  self->rec.deet         = var.deet;
  self->rec.npas         = var.npas;
  self->rec.nbits        = var.nbits;
  self->rec.datyp        = var.datyp;
  self->rec.ip1          = var.ip1;
  self->rec.ip2          = var.ip2;
  self->rec.ip3          = var.ip3;
  strcpy(self->rec.typvar, var.typvar);
  strcpy(self->rec.nomvar, var.nomvar);
  strcpy(self->rec.etiket, var.etiket);
  strcpy(self->rec.grtyp,  var.grtyp);
  self->rec.ig1          = var.ig1;
  self->rec.ig2          = var.ig2;
  self->rec.ig3          = var.ig3;
  self->rec.ig4          = var.ig4;

  return(VGD_OK);
}

int Cvgd_vgdcmp(vgrid_descriptor *vgd1, vgrid_descriptor *vgd2) {

  int nt1, nt2;
  // Check each element of the structure (except FST attributes) for equality
  if (vgd1->vcode != vgd2->vcode)                   return(-1);
  if (vgd1->kind != vgd2->kind)                     return(-2);
  if (vgd1->version != vgd2->version)               return(-3);
  if (strcmp(vgd1->ref_name, vgd2->ref_name) != 0 ) return(-4);
  if (memcmp(&(vgd1->ptop_8),&(vgd2->ptop_8), sizeof(double)/sizeof(char) ))return(-5);
  if (memcmp(&(vgd1->pref_8),&(vgd2->pref_8), sizeof(double)/sizeof(char) ))return(-6);
  if (memcmp(&(vgd1->rcoef1),&(vgd2->rcoef1), sizeof(float) /sizeof(char) ))return(-7);
  if (memcmp(&(vgd1->rcoef2),&(vgd2->rcoef2), sizeof(float) /sizeof(char) ))return(-8);

   // Check pointer associations and values
  if(same_vec_i (vgd1->ip1_m, vgd1->nl_m, vgd2->ip1_m, vgd2->nl_m) != 0) return (-9);
  if(same_vec_i (vgd1->ip1_t, vgd1->nl_t, vgd2->ip1_t, vgd2->nl_t) != 0) return (-10);
  if(same_vec_r8(vgd1->a_m_8, vgd1->nl_m, vgd2->a_m_8, vgd2->nl_m) != 0) return (-11);
  if(same_vec_r8(vgd1->b_m_8, vgd1->nl_m, vgd2->b_m_8, vgd2->nl_m) != 0) return (-12);
  if(same_vec_r8(vgd1->a_t_8, vgd1->nl_t, vgd2->a_t_8, vgd2->nl_t) != 0) return (-13);
  if(same_vec_r8(vgd1->b_t_8, vgd1->nl_t, vgd2->b_t_8, vgd2->nl_t) != 0) return (-14);

  nt1 = vgd1->table_ni * vgd1->table_nj * vgd1->table_nk;
  nt2 = vgd2->table_ni * vgd2->table_nj * vgd2->table_nk;
  if(same_vec_r8(vgd1->table, nt1, vgd2->table, nt2 )            != 0) return (-15);

  return(0);
}

static double c_comp_diag_a_height(double pref_8, float height) {
  float RGASD       =    0.287050000000E+03;
  float GRAV        =    0.980616000000E+01;
  float TCDK        =    0.273150000000E+03;
  return log(pref_8) - GRAV*height/(RGASD*TCDK);
}
static double c_comp_diag_a_ip1(double pref_8, int ip1) {
  float RGASD       =    0.287050000000E+03;
  float GRAV        =    0.980616000000E+01;
  float TCDK        =    0.273150000000E+03;
  int kind;
  return log(pref_8) - GRAV * c_convip_IP2Level(ip1,&kind) / (RGASD*TCDK);
}

/*----------------------------------------------------------------------------
 * Nom      : <VDG_FindIp1Idx>
 * Creation : Avril 2015 - E. Legault-Ouellet - CMC/CMOE
 *
 * But      : Trouver l'index d'un ip1 dans une liste d'ip1
 *
 * Parametres :
 *  <Ip1>   : Paramètres de l'application
 *  <Lst>   : La référence verticale
 *  <Size>  : Header RPN
 *
 * Retour   : L'index de l'ip1 dans la liste ou -1 si pas trouvé
 *
 * Remarques :
 *
 *----------------------------------------------------------------------------
 */
static int VGD_FindIp1Idx(int Ip1,int *Lst,int Size) {
   int idx=0;
   while( Size-- ) {
      if( *Lst++ == Ip1 )
         return idx;
      ++idx;
   }

   return(-1);
}

int Cvgd_print_desc(vgrid_descriptor *self, int sout, int convip) {
  int k, ip1, kind;
  if(! self ) {
    printf("In Cvgd_print_desc: vgrid structure not constructed\n");
    return(VGD_ERROR);
  } else {
    if(! self->valid) {
      printf("In Cvgd_print_desc: vgrid structure is not valid\n");
      return(VGD_ERROR);
    }
    if(sout != -1){
      printf("In Cvgd_print_desc : please implement sout option = %d\n",sout);
      return(VGD_ERROR);
    }
    if(convip != -1){
      //TODO
      printf("In Cvgd_print_desc : please implement convip option %d in Cvgd_print_desc\n",sout);
      return(VGD_ERROR);
    }
    
    // Dump general descriptor information
    printf("-- Vertical Grid Descriptor Information --\n");
    printf("  Vcode=%d\n",self->vcode);
    
    //printf("  Descriptor Nomvar: %s\n",trim(self%rec%nomvar)
    printf("  level kind =%2d, level version = %3d\n", self->kind ,self->version);
    if( is_valid(self, ptop_8_valid) )
      printf("  ptop=%f Pa\n",self->ptop_8);
    if( is_valid(self, pref_8_valid) )
      printf("  pref=%f Pa\n",self->pref_8);
    if( is_valid(self, rcoef1_valid) )
      printf("  rcoef1=%f\n",self->rcoef1);
    if( is_valid(self, rcoef2_valid) )
      printf("  rcoef2=%f\n",self->rcoef2);
    if( is_valid(self,ref_name_valid) )
      printf("  Surface field nomvar %s\n",self->ref_name);
    
    switch(self->vcode) {
    case 1001:
      printf("  Number of sigma levels %d\n",self->nk);
      printf("  Equation to compute hydrostatic pressure (pi): pi = B * P0*100\n");
      break;
    case 1002:
      printf("  Number of eta levels %d\n", self->nl_m );
      break;
    case 2001:
      printf("  Number of pressure levels %d\n", self->nl_m );
      printf("  Equation to compute hydrostatic pressure (pi): pi = A + B * P0*100\n");
      break;
    case 1003:
      printf("  Number of hybrid normalized levels %d\n", self->nl_m );
      printf("  Equation to compute hydrostatic pressure (pi): pi = A + B * P0*100\n");
      break;
    case 5001:
      printf("  Number of hybrid levels (momentum levels) %d\n", self->nl_m );
      printf("  Equation to compute hydrostatic pressure (pi): ln(pi) = A + B * ln(P0*100/pref)\n");
      break;
    case 5002:
    case 5003:
      printf("  Number of hybrid levels %d\n", self->nl_m );
      printf("  Equation to compute hydrostatic pressure (pi): pi = A + B * P0*100\n");
      break;
    case 5004:
      printf("Cvgd_print_desc 5004 TODO!!!!!!!!! \n");
      return(VGD_ERROR);
      break;
    case 5005:
      printf("  Number of hybrid levels (momentum/thermo levels) %d\n", self->nl_m );
      ip1=self->ip1_m[self->nl_m-1];
      printf("  Diagnostic momentum level (ip1=%d) at %f m Above Ground Level\n",ip1,c_convip_IP2Level(ip1,&kind));
      ip1=self->ip1_t[self->nl_t-1];
      printf("  Diagnostic thermo   level (ip1=%d) at %f m Above Ground Level\n",ip1,c_convip_IP2Level(ip1,&kind));
      printf("  Equation to compute hydrostatic pressure (pi): ln(pi) = A + B * ln(P0*100/pref)\n");
      break;
    default:
      printf("(Cvgd) ERROR in Cvgd_print_desc, invalid kind or version: kind=%d, version=%d\n",self->kind,self->version);
      return(VGD_ERROR);
    }

    if(convip != -1){
      printf("Cvgd_print_desc TODO!!!!!!!!! ");
      return(VGD_ERROR); 
    }
     
    // TODO add format
    
    if (is_valid(self, ip1_m_valid) ) {
      printf("  Momentum levels ip1, A, B:\n");
      for ( k = 0; k < self->nl_m; k++) {
	printf("%d %f %f\n",self->ip1_m[k],self->a_m_8[k],self->b_m_8[k]);
      }
    }
    if (is_valid(self, ip1_t_valid) ) {
      printf("  Thermodynamic levels ip1, A, B:\n");
      for ( k = 0; k < self->nl_t; k++) {
	printf("%d %f %f\n",self->ip1_t[k],self->a_t_8[k],self->b_t_8[k]);
      }
    }
    
    return(VGD_OK);
  }
}

int Cvgd_print_vcode_description(int vcode){
  
  // Create horizontal rule
  char *hr = {"-------------------------------------------------------"};

  if(vcode == 1001 || vcode == -1){
    printf("%s\nVcode 1001, kind 1, version 1\n",hr);
    printf("  Sigma levels\n");
  }
  if(vcode == 1002 || vcode == -1) {
    printf("%s\nVcode 1002, kind 1, version 2\n", hr);
    printf("   Eta levels\n");
  }
  if(vcode == 1003 || vcode == -1){
    printf("%s\nVcode 1003, kind 1, version 3\n", hr);
    printf("   Hybrid normalized levels\n");
  }
  if(vcode == 2001 || vcode==-1){
    printf("%s\nVcode 2001, kind 2, version 1\n", hr);
    printf("   Pressure levels\n");
  }
  if(vcode == 5001 || vcode == -1){
    printf("%s\nVcode 5001, kind 5, version 1\n", hr);
    printf("   Hybrid levels, unstaggered\n");
  }
  if(vcode == 5002 || vcode==-1){
    printf("%s\nVcode 5002, kind 5, version 2\n", hr);
    printf("   Hybrid staggered levels, nk momentum levels, nk+1 thermo levels\n");
    printf("   First level at top is a thermo level\n");
  }
  if(vcode == 5003 || vcode==-1){
    printf("%s\nVcode 5003, kind 5, version 3\n", hr);
    printf("   Hybrid staggered levels, nk momentum levels, nk+1 thermo levels\n");
    printf("   First level at top is a thermo level\n");
    printf("   Last thermo level is unstaggered (tlift)\n");
  }
  if(vcode == 5004 || vcode == -1){
    printf("%s\nVcode 5004, kind 5, version 4\n", hr);
    printf("   Hybrid staggered levels, same number of momentum and themro levels\n");
    printf("   First level at top is a momentum level\n");
  }
  if(vcode == 5005 || vcode == -1){
      printf("%s\nVcode 5005, kind 5, version 5\n",  hr);
      printf("   Hybrid staggered levels, same number of momentum and themro levels\n");
      printf("   First level at top is a momentum level\n");
      printf("   Diag level heights (m AGL) encoded\n");
  }

  return(VGD_OK);

}

static int C_compute_pressure_1001_1002_8(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, double *levels, double *sfc_field, int in_log) {
  char proc_name[] = "C_compute_pressure_1001_1002_8";
#define REAL_8 1
#include "BODY_C_compute_pressure_1001_1002.hc"
#undef REAL_8 
}

static int C_compute_pressure_1001_1002(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, float *levels, float *sfc_field, int in_log) {
  char proc_name[] = "C_compute_pressure_1001_1002";
#undef REAL_8
#include "BODY_C_compute_pressure_1001_1002.hc"
}

static int C_compute_pressure_2001_8(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, double *levels, int in_log) {
  char proc_name[] = "C_compute_pressure_2001_8";
#define REAL_8 1
#include "BODY_C_compute_pressure_2001.hc"
#undef REAL_8
}
static int C_compute_pressure_2001(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, float *levels, int in_log) {
  char proc_name[] = "C_compute_pressure_2001";
#undef REAL_8
#include "BODY_C_compute_pressure_2001.hc"
}

static int C_compute_pressure_1003_5001_8(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, double *levels, double *sfc_field, int in_log, int dpidpis ){
  char proc_name[] = "C_compute_pressure_1003_5001_8";
#define REAL_8 1
#include "BODY_C_compute_pressure_1003_5001.hc"
#undef REAL_8
}

static int C_compute_pressure_1003_5001(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, float *levels, float *sfc_field, int in_log, int dpidpis ){
  char proc_name[] = "C_compute_pressure_1003_5001";
#undef REAL_8
#include "BODY_C_compute_pressure_1003_5001.hc"
}

static int C_compute_pressure_5002_5003_5004_5005_8(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, double *levels, double *sfc_field, int in_log, int dpidpis) {
  char proc_name[] = "C_compute_pressure_5002_5003_5004_5005_8";
#include "BODY_C_compute_pressure_5002_5003_5004_5005.hc"
}

static int C_compute_pressure_5002_5003_5004_5005(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, float *levels, float *sfc_field, int in_log, int dpidpis) {
  char proc_name[] = "C_compute_pressure_5002_5003_5004_5005";
#include "BODY_C_compute_pressure_5002_5003_5004_5005.hc"
}

int Cvgd_levels_8(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, double *levels_8, double *sfc_field_8, int in_log) {
  if(Cvgd_diag_withref_8(self, ni, nj, nk, ip1_list, levels_8, sfc_field_8, in_log, 0) == VGD_ERROR )
    return(VGD_ERROR);
  return(VGD_OK);
}

int Cvgd_levels(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, float *levels, float *sfc_field, int in_log) {
  if(Cvgd_diag_withref(self, ni, nj, nk, ip1_list, levels, sfc_field, in_log, 0) == VGD_ERROR )
    return(VGD_ERROR);
  return(VGD_OK);
}

int Cvgd_diag_withref_8(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, double *levels_8, double *sfc_field_8, int in_log, int dpidpis) {
  char proc_name[] = "Cvgd_diag_withref_8";
  char double_interface = 1;
  // The following pointers will never be used but they are needed to compile
  float *levels = NULL, *sfc_field = NULL;
#define REAL_8 1
#include "BODY_Cvgd_diag_withref.hc"
#undef REAL_8
}

int Cvgd_diag_withref(vgrid_descriptor *self, int ni, int nj, int nk, int *ip1_list, float *levels, float *sfc_field, int in_log, int dpidpis) {
  char proc_name[] = "Cvgd_diag_withref";
  char double_interface = 0;
  // The following pointers will never be used but they are needed to compile
  double *levels_8 = NULL, *sfc_field_8 = NULL;
#undef REAL_8
#include "BODY_Cvgd_diag_withref.hc"
}

/*----------------------------------------------------------------------------
 * Nom      : <c_vgd_construct>
 * Creation : Avril 2015 - E. Legault-Ouellet - CMC/CMOE
 *
 * But      : Initialise et retourne une structure de type vgrid_descriptor
 *
 * Parametres :
 *
 * Retour   : Une structure initialisée de type vgrid_descriptor
 *
 * Remarques :
 *
 *----------------------------------------------------------------------------
 */
static vgrid_descriptor* c_vgd_construct() {

   vgrid_descriptor *vgrid = malloc(sizeof(vgrid_descriptor));

   if( vgrid ) {
      vgrid->ptop_8        = VGD_MISSING;
      vgrid->pref_8        = VGD_MISSING;      
      vgrid->table         = NULL;
      vgrid->table_ni      = 0;
      vgrid->table_nj      = 0;
      vgrid->table_nk      = 0;
      vgrid->a_m_8         = NULL;
      vgrid->b_m_8         = NULL;
      vgrid->a_t_8         = NULL;
      vgrid->b_t_8         = NULL;
      vgrid->ip1_m         = NULL;
      vgrid->ip1_t         = NULL;      
      vgrid->nl_m          = 0;
      vgrid->nl_t          = 0;
      vgrid->dhm           = VGD_MISSING;
      vgrid->dht           = VGD_MISSING;
      vgrid->ref_name      = strdup("None");
      vgrid->rcoef1        = VGD_MISSING;
      vgrid->rcoef2        = VGD_MISSING;
      vgrid->nk            = 0;
      vgrid->ip1           = 0;
      vgrid->ip2           = 0;
      vgrid->unit          = 0;
      vgrid->vcode         = 0;
      vgrid->kind          = 0;
      vgrid->version       = 0;
      vgrid->match_ipig    = 0;
      vgrid->valid         = 0;

      vgrid->rec.fstd_initialized = 0;
      vgrid->rec.dateo = 0;
      vgrid->rec.deet = 0;
      vgrid->rec.npas = 0;
      vgrid->rec.nbits = -64;
      vgrid->rec.datyp = 0;
      vgrid->rec.datyp = 0;
      vgrid->rec.ip1 = 0;
      vgrid->rec.ip2 = 0;
      vgrid->rec.ip3 = 0;
      vgrid->rec.ig1 = 0;
      vgrid->rec.ig2 = 0;
      vgrid->rec.ig3 = 0;
      vgrid->rec.ig4 = 0;
      strcpy(vgrid->rec.typvar,"  ");
      strcpy(vgrid->rec.nomvar,"    ");
      strcpy(vgrid->rec.etiket,"            ");
      strcpy(vgrid->rec.grtyp," ");
   }

   return(vgrid);
}

static void c_vgd_free_abi(vgrid_descriptor **self) {
  if( *self ) {
    // Thermo pointers may be pointing to momentum for certain Vcode, only nullify them if this is the case.
    if( (*self)->a_t_8 == (*self)->a_m_8 ) {
      (*self)->a_t_8 = NULL;
    } else {
      FREE((*self)->a_t_8);      
    }
    if( (*self)->b_t_8 == (*self)->b_m_8 ) {
      (*self)->b_t_8 = NULL;
    } else {
      FREE((*self)->b_t_8);
    }
    if( (*self)->ip1_t == (*self)->ip1_m ) {
      (*self)->ip1_t = NULL;
    } else {
      FREE((*self)->ip1_t);
    }
    FREE((*self)->a_m_8);
    FREE((*self)->b_m_8);
    FREE((*self)->ip1_m);
  }
}

void Cvgd_free(vgrid_descriptor **self) {
   if( *self ) {
      FREE((*self)->table);
      c_vgd_free_abi(self);
      FREE((*self)->ref_name);      
      free(*self);
      *self = NULL;
   }
}

/*----------------------------------------------------------------------------
 * Nom      : <Cvgd_set_vcode_i>
 * Creation : Avril 2015 - E. Legault-Ouellet - CMC/CMOE
 *
 * But      : Set and check the vertical code
 *
 * Parametres :
 *    <VGrid>  : The grid structure
 *    <Kind>   : Kind of the vertical coord
 *    <Version>: Version of the vertical coord
 *
 * Retour   :
 *
 * Remarques :
 *
 *----------------------------------------------------------------------------
 */
int Cvgd_set_vcode_i(vgrid_descriptor *VGrid,int Kind,int Version) {

   if( Kind>MAX_VKIND || Kind<0 || Version>999 || Version<0 ) {
      fprintf(stderr,"(Cvgd) ERROR in Cvgd_set_vcode_i, invalid kind or version kind=%d, version=%d\n",Kind,Version);
      return(VGD_ERROR);
   }
   VGrid->vcode = Kind*1000 + Version;
   return(VGD_OK);
}

/*----------------------------------------------------------------------------
 * Nom      : <Cvgd_set_vcode>
 * Creation : Avril 2015 - E. Legault-Ouellet - CMC/CMOE
 *
 * But      : Set and check the vertical code
 *
 * Parametres :
 *    <VGrid>  : The grid structure
 *
 * Retour   :
 *
 * Remarques :
 *
 *----------------------------------------------------------------------------
 */
int Cvgd_set_vcode(vgrid_descriptor *VGrid) {

   if( !VGrid->table ) {
      fprintf(stderr,"(Cvgd) ERROR: Cvgd_set_vcode called before constructor\n");
      return(VGD_ERROR);
   }

   return Cvgd_set_vcode_i(VGrid,VGrid->kind,VGrid->version);
}

/*----------------------------------------------------------------------------
 * Nom      : <fstd_init>
 * Creation : Avril 2015 - E. Legault-Ouellet - CMC/CMOE
 *
 * But      : Initialize common elements of the fstd record
 *
 * Parametres :
 *    <VGrid>  : The grid structure
 *
 * Retour   :
 *
 * Remarques :
 *
 *----------------------------------------------------------------------------
 */
static int fstd_init(vgrid_descriptor *VGrid) {

   int err;
  
   VGD_TFSTD *h = &VGrid->rec;

   if( h->fstd_initialized )
      return(VGD_OK);

   h->ig2=h->ig3=h->ig4=0;

   err=Cvgd_set_vcode(VGrid);

   switch(VGrid->vcode) {
      case 1001:
         strcpy(h->etiket,"ETA_GEMV3");
         break;
      case 1002:
         strcpy(h->etiket,"ETA_GEMV3");
         h->ig2=(int)round(VGrid->ptop_8*10.0);
         break;
      case 2001:
         strcpy(h->etiket,"PRESSURE");
         break;
      case 1003:
         strcpy(h->etiket,"HYBNORM_GEM3");
         h->ig2=(int)round(VGrid->ptop_8*10.0);
         h->ig3=(int)roundf(VGrid->rcoef1*100.0f);
         break;
      case 5001:
         strcpy(h->etiket,"HYB_GEMV3");
         h->ig2=(int)round(VGrid->ptop_8*10.0);
         h->ig3=(int)roundf(VGrid->rcoef1*100.0f);
         break;
      case 5002:
      case 5003:
      case 5004:
         strcpy(h->etiket,"STG_CP_GEMV4");
         h->ig2=(int)round(VGrid->ptop_8*10.0);
         h->ig3=(int)roundf(VGrid->rcoef1*100.0f);
         h->ig4=(int)roundf(VGrid->rcoef2*100.0f);
         break;
      case 5005:
         strcpy(h->etiket,"STG_CP_GEMV4");
         h->ig2=0;
         h->ig3=(int)roundf(VGrid->rcoef1*100.0f);
         h->ig4=(int)roundf(VGrid->rcoef2*100.0f);
         break;
      default:
         fprintf(stderr,"(Cvgd) ERROR in fstd_init, invalid kind or version: kind=%d, version=%d\n",VGrid->kind,VGrid->version);
         return(VGD_ERROR);
   }

   strcpy(h->nomvar,"!!");
   strcpy(h->typvar,"X");
   strcpy(h->grtyp,"X");

   h->dateo       = 0;
   h->deet        = 0;
   h->npas        = 0;
   h->datyp       = 5;
   h->nbits       = 64;
   h->ip3         = 0;
   h->ig1         = VGrid->vcode;
   h->fstd_initialized = 1;

   return(VGD_OK);
}

int Cvgd_new_build_vert(vgrid_descriptor **self, int kind, int version, int nk, int ip1, int ip2, double *ptop_8, double *pref_8, float *rcoef1, float *rcoef2, 
		     double *a_m_8, double *b_m_8, double *a_t_8, double *b_t_8, int *ip1_m, int *ip1_t, int nl_m, int nl_t)
{
  char cvcode[5];
  int errorInput = 0, ier, missingInput;
  
  if(*self){
    Cvgd_free(self);
  }
  *self = c_vgd_construct();
  if(! *self){
    printf("(Cvgd) ERROR in Cvgd_new_build_vert, null pointer returned by c_vgd_construct\n");
    return (VGD_ERROR);
  }

  // Initializations
  (*self)->valid      = 1;
  (*self)->kind       = kind;
  (*self)->version    = version;
  (*self)->unit       = -1;
  (*self)->match_ipig = 1;
  (*self)->nk         = nk;
  (*self)->nl_m       = nl_m;
  (*self)->nl_t       = nl_t;
  (*self)->rec.ip1    = ip1;
  (*self)->rec.ip2    = ip2;
  strcpy((*self)->rec.nomvar,"!!  ");
  if(Cvgd_set_vcode_i(*self, kind, version) == VGD_ERROR)  {
    printf("(Cvgd) ERROR in Cvgd_new_build_vert, problem with Cvgd_set_vcode_i");
    return (VGD_ERROR);
  }
  (*self)->rec.ig1   = (*self)->vcode;

  // Check for required inputs
  if( is_valid( *self, ptop_8_valid) ) {
    if(ptop_8) {
      (*self)->ptop_8 = *ptop_8;
    } else {
      printf("(Cvgd) ptop_8 is a required constructor entry\n");
      missingInput = 1;
    }
  }
  if(is_valid( *self, pref_8_valid)) {
    if(pref_8){
      (*self)->pref_8 = *pref_8;
    } else {
      printf("(Cvgd) pref_8 is a required constructor entry\n");
      missingInput = 1;
    }
  }
  if(is_valid( *self, rcoef1_valid)) {
    if(rcoef1){
      (*self)->rcoef1 = *rcoef1;
    } else {
      printf("(Cvgd) rcoef1 is a required constructor entry\n");
      missingInput = 1;
    }
  }
  if(is_valid( *self, rcoef2_valid)) {
    if(rcoef2){
      (*self)->rcoef2 = *rcoef2;
    } else {
      printf("rcoef2 is a required constructor entry\n");
      missingInput = 1;
    }
  }

  if(is_valid( *self, a_m_8_valid)) {
    if(a_m_8){
      free((*self)->a_m_8);
      (*self)->a_m_8 = malloc( nl_m * sizeof(double) );
      if(! (*self)->a_m_8){ 
	printf("(Cvgd) ERROR in Cvgd_new_build_vert, problem allocating a_m_8\n");
	return(VGD_ERROR);
      }
      my_copy_double(a_m_8, &((*self)->a_m_8), nl_m);
    } else {
      printf("(Cvgd) a_m_8 is a required constructor entry\n");
      errorInput = 1;
    }
  }
  if(is_valid( *self, b_m_8_valid)) {
    if(b_m_8){
      free((*self)->b_m_8);
      (*self)->b_m_8 = malloc( nl_m * sizeof(double) );
      if(! (*self)->b_m_8) {
	printf("(Cvgd) ERROR in Cvgd_new_build_vert, problem allocating b_m_8\n");
	return(VGD_ERROR);
      }
      my_copy_double(b_m_8, &((*self)->b_m_8), nl_m);
    } else {
      printf("(Cvgd) b_m_8 is a required constructor entry\n");
      errorInput = 1;
    }
  }
  if(is_valid( *self, a_t_8_valid)) {
    if(a_t_8){
      free((*self)->a_t_8);
      (*self)->a_t_8 = malloc( nl_t * sizeof(double) );
      if(! (*self)->a_t_8) {
	printf("(Cvgd) ERROR in Cvgd_new_build_vert, problem allocating a_t_8\n");
	return(VGD_ERROR);
      }
      my_copy_double(a_t_8, &((*self)->a_t_8), nl_t);
    } else {
      printf("(Cvgd) a_t_8 is a required constructor entry\n");
      errorInput = 1;
    }
  } else if ( is_valid( *self, a_t_8_valid_get) ) {
    (*self)->a_t_8 = (*self)->a_m_8;
    (*self)->nl_t = (*self)->nl_m;
  }
  if(is_valid( *self, b_t_8_valid)) {
    if(b_t_8){
      free((*self)->b_t_8);
      (*self)->b_t_8 = malloc( nl_t * sizeof(double) );
      if(! (*self)->b_t_8) {
	printf("(Cvgd) ERROR in Cvgd_new_build_vert, problem allocating b_t_8\n");
	return(VGD_ERROR);
      }
      my_copy_double(b_t_8, &((*self)->b_t_8), nl_t);
    } else {
      printf("(Cvgd) b_t_8 is a required constructor entry\n");
      errorInput = 1;
    }
  } else if ( is_valid( *self, b_t_8_valid_get) ) {
    // a_t_8 is not a valid component put may be get in which case we return the momentum values
    (*self)->b_t_8 = (*self)->b_m_8;
    (*self)->nl_t = (*self)->nl_m;
  }
  if(is_valid( *self, ip1_m_valid)) {
    if(ip1_m){
      free((*self)->ip1_m);
      (*self)->ip1_m = malloc( nl_m * sizeof(double) );
      if(! (*self)->ip1_m) {
	printf("(Cvgd) ERROR in Cvgd_new_build_vert, problem allocating ip1_m in Cvgd_new_build_vert\n");
	return(VGD_ERROR);
      }
      my_copy_int(ip1_m, &((*self)->ip1_m), nl_m);
    } else {
      printf("(Cvgd) ip1_m is a required constructor entry\n");
      errorInput = 1;
    }
  }
  if(is_valid( *self, ip1_t_valid)) {
    if(ip1_t){
      free((*self)->ip1_t);
      (*self)->ip1_t = malloc( nl_t * sizeof(double) );
      if(! (*self)->ip1_t) {
	printf("(Cvgd) ERROR: in Cvgd_new_build_vert, problem allocating ip1_t\n");
	return(VGD_ERROR);
      }
      my_copy_int(ip1_t, &((*self)->ip1_t), nl_t);
    } else {
      printf("(Cvgd) ip1_t is a required constructor entry\n");
      errorInput = 1;
    }
  } else if ( is_valid( *self, ip1_t_valid_get) ) {
    // ip1_t is not a valid compnent put may be get in which case we return the momentum values
    (*self)->ip1_t = (*self)->ip1_m;
    (*self)->nl_t = (*self)->nl_m;
  }
  if (errorInput > 0) {
    return (VGD_ERROR);
  }  
  // Fill table with version-specific encoder
  switch((*self)->vcode) {
  case 1001:
    strcpy(cvcode,"1001");
    ier = c_encode_vert_1001(self,nk);
    break;
  case 1002:
    strcpy(cvcode,"1002");
    ier = c_encode_vert_1002(self,nk);
    break;
  case 2001:
    ier = c_encode_vert_2001(self,nk);
    break;
  case 1003:
    printf("(Cvgd) ERROR: New build for Vcode 1003 not supported by this package,  please contact Andre Plante to add this feature.\n");
    return(VGD_ERROR);
    break;
  case 5001:
    strcpy(cvcode,"5001");
    ier = c_encode_vert_5001(self,nk);
    break;
  case 5002:
    strcpy(cvcode,"5002");
    ier = c_encode_vert_5002_5003_5004_5005(self, 0);
    break;
  case 5003:
    strcpy(cvcode,"5003");
    ier = c_encode_vert_5002_5003_5004_5005(self, 0);
    break;
  case 5004:
    strcpy(cvcode,"5004");
    ier = c_encode_vert_5002_5003_5004_5005(self, 0);
    break;
  case 5005:
    strcpy(cvcode,"5005");
    ier = c_encode_vert_5002_5003_5004_5005(self, 0);
    break;
  default:
    fprintf(stderr,"(Cvgd) ERROR in Cvgd_new_build_vert, invalid kind or version : kind=%d, version=%d\n",kind,version);
    return(VGD_ERROR);
  }

  if(ier == VGD_ERROR) {
    printf("(Cvgd) ERROR in Cvgd_new_build_vert, problem with encode_vert_%s\n",cvcode);
    return(VGD_ERROR);
  }
  
  (*self)->valid = 1;
  if(fstd_init(*self) == VGD_ERROR) {
    printf("(Cvgd) ERROR in Cvgd_new_build_vert, problem with fstd_init\n");
  }

  return(VGD_OK);

}

static int c_encode_vert_1001(vgrid_descriptor **self,int nk){
  
  int skip = 2, table_size;

  if( (*self)->table )
    free( (*self)->table );
  (*self)->table_ni = 3;
  (*self)->table_nj = nk+skip;
  (*self)->table_nk = 1;
  table_size = (*self)->table_ni * (*self)->table_nj * (*self)->table_nk;
  (*self)->table = malloc ( table_size * sizeof(double) );
  if(! (*self)->table ) {
    printf("(Cvgd) ERROR in c_encode_vert_1001, cannot allocate table of bouble of size %d\n",table_size );
    return(VGD_ERROR);
  }
  strcpy((*self)->ref_name,"P0");

  //Fill header
  (*self)->table[0] = (*self)->kind;
  (*self)->table[1] = (*self)->version;
  (*self)->table[2] = skip;
  flip_transfer_c2d((*self)->ref_name, &((*self)->table[3]));
  (*self)->table[4] = 0.;
  (*self)->table[5] = 0.;
  
  int k, ind = 6;
  for ( k = 0; k < nk; k++){
    (*self)->table[ind  ] = (*self)->ip1_m[k];
    (*self)->table[ind+1] = (*self)->a_m_8[k];
    (*self)->table[ind+2] = (*self)->b_m_8[k];
    ind = ind + 3;
  }

  (*self)->valid = 1;
  return(VGD_OK);
}

static int c_encode_vert_1002(vgrid_descriptor **self,int nk){
  
  int skip = 2, table_size;
  
  if( (*self)->table )
    free( (*self)->table );
  (*self)->table_ni = 3;
  (*self)->table_nj = nk+skip;
  (*self)->table_nk = 1;
  table_size = (*self)->table_ni * (*self)->table_nj * (*self)->table_nk;
  (*self)->table = malloc ( table_size * sizeof(double) );
  if(! (*self)->table ) {
    printf("(Cvgd) ERROR in c_encode_vert_1002, cannot allocate table of bouble of size %d\n",table_size );
    return(VGD_ERROR);
  }
  strcpy((*self)->ref_name,"P0");

  //Fill header
  (*self)->table[0] = (*self)->kind;
  (*self)->table[1] = (*self)->version;
  (*self)->table[2] = skip;
  (*self)->table[3] = (*self)->ptop_8;
  flip_transfer_c2d((*self)->ref_name, &((*self)->table[4]));
  (*self)->table[5] = 0.;
  
  int k, ind = 6;
  for ( k = 0; k < nk; k++){
    (*self)->table[ind  ] = (*self)->ip1_m[k];
    (*self)->table[ind+1] = (*self)->a_m_8[k];
    (*self)->table[ind+2] = (*self)->b_m_8[k];
    ind = ind + 3;
  }

  (*self)->valid = 1;
  return(VGD_OK);
}

static int c_encode_vert_2001(vgrid_descriptor **self,int nk){
  
  int skip = 1, table_size;
  
  if( (*self)->table )
    free( (*self)->table );
  (*self)->table_ni = 3;
  (*self)->table_nj = nk+skip;
  (*self)->table_nk = 1;
  table_size = (*self)->table_ni * (*self)->table_nj * (*self)->table_nk;
  (*self)->table = malloc ( table_size * sizeof(double) );
  if(! (*self)->table ) {
    printf("(Cvgd) ERROR in c_encode_vert_2001, cannot allocate table of bouble of size %d\n",table_size );
    return(VGD_ERROR);
  }
  strcpy((*self)->ref_name,"");

  //Fill header
  (*self)->table[0] = (*self)->kind;
  (*self)->table[1] = (*self)->version;
  (*self)->table[2] = skip;
  
  int k, ind = 3;
  for ( k = 0; k < nk; k++){
    (*self)->table[ind  ] = (*self)->ip1_m[k];
    (*self)->table[ind+1] = (*self)->a_m_8[k];
    (*self)->table[ind+2] = (*self)->b_m_8[k];
    ind = ind + 3;
  }

  (*self)->valid = 1;

  return(VGD_OK);
}

static int c_encode_vert_5001(vgrid_descriptor **self,int nk){
  int skip = 3, table_size;

  if( (*self)->table )
    free( (*self)->table );
  (*self)->table_ni = 3;
  (*self)->table_nj = nk+skip;
  (*self)->table_nk = 1;
  table_size = (*self)->table_ni * (*self)->table_nj * (*self)->table_nk;
  (*self)->table = malloc ( table_size * sizeof(double) );
  if(! (*self)->table ) {
    printf("(Cvgd) ERROR in c_encode_vert_5001, cannot allocate table of bouble of size %d\n",table_size );
    return(VGD_ERROR);
  }
  strcpy((*self)->ref_name,"P0");

  //Fill header
  (*self)->table[0] = (*self)->kind;
  (*self)->table[1] = (*self)->version;
  (*self)->table[2] = skip;

  (*self)->table[3] = (*self)->ptop_8;
  (*self)->table[4] = (*self)->pref_8;
  (*self)->table[5] = (*self)->rcoef1;
  
  flip_transfer_c2d((*self)->ref_name, &((*self)->table[6]));
  (*self)->table[7] = 0.;
  (*self)->table[8] = 0.;

  int k, ind = 9;
  for ( k = 0; k < nk; k++){
    (*self)->table[ind  ] = (*self)->ip1_m[k];
    (*self)->table[ind+1] = (*self)->a_m_8[k];
    (*self)->table[ind+2] = (*self)->b_m_8[k];
    ind = ind + 3;
  }

  (*self)->valid = 1;

  return(VGD_OK);
}

static int c_encode_vert_5002_5003_5004_5005(vgrid_descriptor **self, char update){
  int skip = 3, table_size;
  if(! *self ) {
    printf("(Cvgd) ERROR in c_encode_vert_5002_5003_5004_5005, vgrid descriptor not constructed\n");
    return(VGD_ERROR);
  }
  if(! update) {    
    if( (*self)->table )
      free( (*self)->table );
    (*self)->table_ni = 3;
    (*self)->table_nj = (*self)->nl_m + (*self)->nl_t + skip;
    (*self)->table_nk = 1;
    table_size = (*self)->table_ni * (*self)->table_nj * (*self)->table_nk;
    (*self)->table = malloc ( table_size * sizeof(double) );
    if(! (*self)->table ) {
      printf("(Cvgd) ERROR in c_encode_vert_5002_5003_5004_5005, cannot allocate table of bouble of size %d\n", table_size);
      return(VGD_ERROR);
    }
    strcpy((*self)->ref_name,"P0");
  }
  //Fill header
  (*self)->table[0] = (*self)->kind;
  (*self)->table[1] = (*self)->version;
  (*self)->table[2] = skip;
  (*self)->table[3] = (*self)->ptop_8;
  (*self)->table[4] = (*self)->pref_8;
  (*self)->table[5] = (*self)->rcoef1;  
  (*self)->table[6] = (*self)->rcoef2;
  flip_transfer_c2d((*self)->ref_name, &((*self)->table[7]));
  (*self)->table[8] = 0.;

  int k, ind = 9;
  for ( k = 0; k < (*self)->nl_m; k++){
    (*self)->table[ind  ] = (*self)->ip1_m[k];
    (*self)->table[ind+1] = (*self)->a_m_8[k];
    (*self)->table[ind+2] = (*self)->b_m_8[k];
    ind = ind + 3;
  }
  for ( k = 0; k < (*self)->nl_t; k++){
    (*self)->table[ind  ] = (*self)->ip1_t[k];
    (*self)->table[ind+1] = (*self)->a_t_8[k];
    (*self)->table[ind+2] = (*self)->b_t_8[k];
    ind = ind + 3;
  }

  (*self)->valid = 1;

  return(VGD_OK);
}

static int c_decode_vert_1001(vgrid_descriptor **self) {
  int skip, nk, k, ind;
  (*self)->kind    = (int) (*self)->table[0];
  (*self)->version = (int) (*self)->table[1];
  skip             = (int) (*self)->table[2];
  flip_transfer_d2c((*self)->ref_name,(*self)->table[3]);
  // The next two values in table are not used, so we continue with ind = 6
  ind = 6;
  
  nk = (*self)->table_nj - skip;
  // Free A, B and Ip1 vectors for momentum and thermo.
  c_vgd_free_abi(self);
  // Allocate and assign level data, there are nk of them
  (*self)->nl_m = nk;
  (*self)->nl_t = nk;
  (*self)->ip1_m = malloc( nk * sizeof(int) );
  (*self)->a_m_8 = malloc( nk * sizeof(double) );
  (*self)->b_m_8 = malloc( nk * sizeof(double) );
  if( !(*self)->ip1_m || !(*self)->a_m_8 || !(*self)->b_m_8 ){
    printf("(Cvgd) ERROR in c_decode_vert_1001, cannot allocate,  ip1_m, a_m_8 and b_m_8 of size %d\n", nk);
    return(VGD_ERROR);
  }
  for ( k = 0; k < nk; k++){      
    (*self)->ip1_m[k] = (int) (*self)->table[ind  ];
    (*self)->a_m_8[k] =       (*self)->table[ind+1];
    (*self)->b_m_8[k] =       (*self)->table[ind+2];
    ind = ind + 3;
  }  
  (*self)->ip1_t = (*self)->ip1_m;
  (*self)->a_t_8 = (*self)->a_m_8;
  (*self)->b_t_8 = (*self)->b_m_8;
  return(VGD_OK);
}

static int c_decode_vert_1002(vgrid_descriptor **self) {
  int skip, nk, k, ind;
  (*self)->kind    = (int) (*self)->table[0];
  (*self)->version = (int) (*self)->table[1];
  skip             = (int) (*self)->table[2];
  (*self)->ptop_8  = (*self)->table[3];
    flip_transfer_d2c((*self)->ref_name,(*self)->table[4]);
  // The next value in table is not used, so we continue with ind = 6
  ind = 6;
  nk = (*self)->table_nj - skip;
  
  // Free A, B and Ip1 vectors for momentum and thermo.
  c_vgd_free_abi(self);
  // Allocate and assign level data, there are nk of them
  (*self)->nl_m = nk;
  (*self)->nl_t = nk;
  (*self)->ip1_m = malloc( nk * sizeof(int) );
  (*self)->a_m_8 = malloc( nk * sizeof(double) );
  (*self)->b_m_8 = malloc( nk * sizeof(double) );
  if( !(*self)->ip1_m || !(*self)->a_m_8 || !(*self)->b_m_8 ){
    printf("(Cvgd) ERROR in c_decode_vert_1002, cannot allocate,  ip1_m, a_m_8 and b_m_8 of size %d\n", nk);
    return(VGD_ERROR);
  }
   for ( k = 0; k < nk; k++){      
    (*self)->ip1_m[k] = (int) (*self)->table[ind  ];
    (*self)->a_m_8[k] =       (*self)->table[ind+1];
    (*self)->b_m_8[k] =       (*self)->table[ind+2];
    ind = ind + 3;
  }
  (*self)->ip1_t = (*self)->ip1_m;
  (*self)->a_t_8 = (*self)->a_m_8;
  (*self)->b_t_8 = (*self)->b_m_8;
  return(VGD_OK);
}

static int c_decode_vert_2001(vgrid_descriptor **self) {
  int skip, nk, k, ind;
  (*self)->kind    = (int) (*self)->table[0];
  (*self)->version = (int) (*self)->table[1];
  skip             = (int) (*self)->table[2];
  ind = 3;

  nk = (*self)->table_nj - skip;

  // Free A, B and Ip1 vectors for momentum and thermo.
  c_vgd_free_abi(self);
  // Allocate and assign level data, there are nk of them
  (*self)->nl_m = nk;
  (*self)->nl_t = nk;
  (*self)->ip1_m = malloc( nk * sizeof(int) );
  (*self)->a_m_8 = malloc( nk * sizeof(double) );
  (*self)->b_m_8 = malloc( nk * sizeof(double) );
  if( !(*self)->ip1_m || !(*self)->a_m_8 || !(*self)->b_m_8 ){
    printf("(Cvgd) ERROR in c_decode_vert_1002, cannot allocate,  ip1_m, a_m_8 and b_m_8 of size %d\n", nk);
    return(VGD_ERROR);
  }
  for ( k = 0; k < nk; k++){      
    (*self)->ip1_m[k] = (int) (*self)->table[ind  ];
    (*self)->a_m_8[k] =       (*self)->table[ind+1];
    (*self)->b_m_8[k] =       (*self)->table[ind+2];
    ind = ind + 3;
  }
  (*self)->ip1_t = (*self)->ip1_m;
  (*self)->a_t_8 = (*self)->a_m_8;
  (*self)->b_t_8 = (*self)->b_m_8;
  return(VGD_OK);
}

static int c_decode_vert_1003_5001(vgrid_descriptor **self) {
  int skip, k, ind, nk;
  
  (*self)->kind    = (int) (*self)->table[0];
  (*self)->version = (int) (*self)->table[1];
  skip             = (int) (*self)->table[2];
  (*self)->ptop_8  = (*self)->table[3];
  (*self)->pref_8  = (*self)->table[4];
  (*self)->rcoef1  = (float) (*self)->table[5];
  flip_transfer_d2c((*self)->ref_name,(*self)->table[6]);

  // The next two values in table are not used, so we continue with ind = 9
  ind = 9;
  nk = (*self)->table_nj - skip;

  // Free A, B and Ip1 vectors for momentum and thermo.
  c_vgd_free_abi(self);

  // Allocate and assign momentum level data, there are nk of them
  (*self)->nl_m = nk;
  (*self)->nl_t = nk;
  (*self)->ip1_m = malloc( nk * sizeof(int) );
  (*self)->a_m_8 = malloc( nk * sizeof(double) );
  (*self)->b_m_8 = malloc( nk * sizeof(double) );
  if( !(*self)->ip1_m || !(*self)->a_m_8 || !(*self)->b_m_8 ){
    printf("(Cvgd) ERROR in c_decode_vert_1003_5001, cannot allocate,  ip1_m, a_m_8 and b_m_8 of size %d\n", nk);
    return(VGD_ERROR);
  }
  for ( k = 0; k < nk; k++){    
    (*self)->ip1_m[k] = (int) (*self)->table[ind  ];
    (*self)->a_m_8[k] =       (*self)->table[ind+1];
    (*self)->b_m_8[k] =       (*self)->table[ind+2];
    ind = ind + 3;
  }
  (*self)->ip1_t = (*self)->ip1_m;
  (*self)->a_t_8 = (*self)->a_m_8;
  (*self)->b_t_8 = (*self)->b_m_8;
  
  return(VGD_OK);
}

static int c_decode_vert_5002_5003_5004_5005(vgrid_descriptor **self) {
  int skip, k, ind, k_plus_top, k_plus_diag, nk, nb, kind;
  
  k_plus_top = 1;
  (*self)->kind    = (int) (*self)->table[0];
  (*self)->version = (int) (*self)->table[1];
  skip             = (int) (*self)->table[2];
  (*self)->ptop_8  = (*self)->table[3];
  (*self)->pref_8  = (*self)->table[4];
  (*self)->rcoef1  = (float) (*self)->table[5];
  (*self)->rcoef2  = (float) (*self)->table[6];
  flip_transfer_d2c((*self)->ref_name,(*self)->table[7]);

  if( Cvgd_set_vcode_i(*self, (*self)->kind, (*self)->version) == VGD_ERROR ) {
    printf("(Cvgd) ERROR in c_decode_vert_5002_5003_5004_5005, cannot set vcode\n");
    return(VGD_ERROR);
  }
  switch((*self)->vcode) {
  case 5002:
  case 5003:
    k_plus_top=1;
    break;
  case 5004:
  case 5005:
    k_plus_top=0;
    break;
  default:
    printf("(Cvgd) ERROR in c_decode_vert_5002_5003_5004_5005, Vcode %d not supported\n", (*self)->vcode);
    return(VGD_ERROR);
  }

  k_plus_diag = 0;
  if(is_valid(*self,dhm_valid)) {
    k_plus_diag=1;
  }

  // The next value in table is not used, so we continue with ind = 9
  ind = 9;
  // nk is the number of momentum level without hyb=1.0 and the diag level in m
  nk = ( (*self)->table_nj - k_plus_top - skip ) / 2 -1 -k_plus_diag;

  // Free A, B and Ip1 vectors for momentum and thermo.
  c_vgd_free_abi(self);

  // Allocate and assign momentum level data, there are nb of them nk + hyb=1 and possibly the diag in m
  nb = nk + 1 + k_plus_diag;
  (*self)->nl_m = nb;
  (*self)->ip1_m = malloc( nb * sizeof(int) );
  (*self)->a_m_8 = malloc( nb * sizeof(double) );
  (*self)->b_m_8 = malloc( nb * sizeof(double) );
  if( !(*self)->ip1_m || !(*self)->a_m_8 || !(*self)->b_m_8 ){
    printf("(Cvgd) ERROR in c_decode_vert_5002_5003_5004_5005, cannot allocate,  ip1_m, a_m_8 and b_m_8 of size %d\n", nb);
    return(VGD_ERROR);
  }
  for ( k = 0; k < nb; k++){
    (*self)->ip1_m[k] = (int) (*self)->table[ind  ];
    (*self)->a_m_8[k] =       (*self)->table[ind+1];
    (*self)->b_m_8[k] =       (*self)->table[ind+2];
    ind = ind + 3;
  }
  if(is_valid(*self,dhm_valid)) (*self)->dhm = c_convip_IP2Level( (*self)->ip1_m[nb-1], &kind );

  // Allocate and assign thermodynamic level data
  nb = nb + k_plus_top;
  (*self)->nl_t = nb;
  (*self)->ip1_t = malloc( nb * sizeof(int) );
  (*self)->a_t_8 = malloc( nb * sizeof(double) );
  (*self)->b_t_8 = malloc( nb * sizeof(double) );
  if( !(*self)->ip1_t || !(*self)->a_t_8 || !(*self)->b_t_8 ){
    printf("(Cvgd) ERROR in c_decode_vert_5002_5003_5004_5005, cannot allocate,  ip1_t, a_t_8 and b_t_8 of size %d\n", nb);
    return(VGD_ERROR);
  }
  for ( k = 0; k < nb; k++){
    (*self)->ip1_t[k] = (int) (*self)->table[ind  ];
    (*self)->a_t_8[k] =       (*self)->table[ind+1];
    (*self)->b_t_8[k] =       (*self)->table[ind+2];
    ind = ind + 3;
  }
  if(is_valid(*self,dht_valid)) (*self)->dht= c_convip_IP2Level( (*self)->ip1_t[nb-1], &kind );
  (*self)->valid = 1;

  return(VGD_OK);  

}

static int C_genab_1001(float *hyb, int nk, double **a_m_8, double **b_m_8, int **ip1_m)
{

  // Andre Plante May 2015. 
  char ok = 1;
  int k,ip1, kind2;
  float f_one=1.f;

  if( my_alloc_double(a_m_8, nk, "(Cvgd) ERROR in C_genab_1001, malloc error with a_m_8") == VGD_ERROR )
    return(VGD_ERROR);
  if( my_alloc_double(b_m_8, nk, "(Cvgd) ERROR in C_genab_1001, malloc error with b_m_8") == VGD_ERROR )
    return(VGD_ERROR);
  if( my_alloc_int   (ip1_m, nk, "(Cvgd) ERROR in C_genab_1001, malloc error with ip1_m") == VGD_ERROR )
    return(VGD_ERROR);
  
  if( memcmp( &(hyb[nk-1]), &f_one, sizeof(float)/sizeof(char)) ){
    printf("WRONG SPECIFICATION OF SIGMA VERTICAL LEVELS: SIGMA(NK) MUST BE 1.0\n");
    ok=0;
  }
  //Check monotonicity
  for ( k = 1; k < nk; k++){
    if(hyb[k] <= hyb[k-1]){
      printf("WRONG SPECIFICATION OF SIGMA VERTICAL LEVELS: LEVELS MUST BE MONOTONICALLY INCREASING\n");
      ok=0;
      break;
    }
  }
  if(! ok){
    printf("   Current choice:\n");
    for ( k = 0; k < nk; k++){
      printf("   %f\n", hyb[k]);
    }
    return(VGD_ERROR);
  }

  for ( k = 0; k < nk; k++){
    (*a_m_8)[k]=0.;
    // Go back and forth to ip1 in order to make sure hyb value is encodable.
    ip1 = c_convip_Level2IP_old_style(hyb[k],1);
    (*b_m_8)[k] = (double) c_convip_IP2Level(ip1,&kind2);
    (*ip1_m)[k] = ip1;
  }

  return(VGD_OK);
  
}

int Cvgd_new_from_table(vgrid_descriptor **self, double *table, int ni, int nj, int nk) {
  int table_size, i;
  double *ltable;

  // Coordinate constructor - build vertical descriptor from table input
  // Set internal vcode (if all above was successful)

  if(! *self){
    *self = c_vgd_construct();
    if(! *self){
      printf("(Cvgd) ERROR in Cvgd_new_from_table, null pointer returned by c_vgd_construct\n");
      return (VGD_ERROR);
    }
  }
  (*self)->valid = 0;
  // Since table passed in argument may be the (*self)->table, we take a copy before the call to free
  table_size = ni * nj * nk;
  ltable = malloc ( table_size * sizeof(double) );
  if(! ltable ) {
    printf("(Cvgd) ERROR in Cvgd_new_from_table, cannot allocate ltable of bouble of size %d\n", table_size);
    return(VGD_ERROR);
  }
  my_copy_double(table, &ltable, table_size);  
  free((*self)->table);
  (*self)->table_ni = ni;
  (*self)->table_nj = nj;
  (*self)->table_nk = nk;
  (*self)->table = malloc ( ni * nj * nk * sizeof(double) );
  if(! (*self)->table ) {
    printf("(Cvgd) ERROR in Cvgd_new_from_table, cannot allocate table of bouble of size %d\n",table_size );
    return(VGD_ERROR);
  }
  for(i = 0; i < table_size; i++) {
    (*self)->table[i] = ltable[i];
  }
  free(ltable);
  (*self)->kind    = (int) (*self)->table[0];
  (*self)->version = (int) (*self)->table[1];
  // Fill remainder of structure
  if( Cvgd_set_vcode(*self) == VGD_ERROR ) {
    printf("(Cvgd) ERROR in Cvgd_new_from_table, cannot set vcode\n");
    return(VGD_ERROR);
  }

  switch((*self)->vcode) {
  case 1001:
    if( c_decode_vert_1001(self) == VGD_ERROR ) {
      printf("(Cvgd) in Cvgd_new_from_table, problem decoding table with vcode 1001\n");
      return(VGD_ERROR);
    }
    break;
  case 1002:
    if( c_decode_vert_1002(self) == VGD_ERROR ) {
      printf("(Cvgd) in Cvgd_new_from_table, problem decoding table with vcode 1002\n");
      return(VGD_ERROR);
    }
    break;
  case 2001:
    if( c_decode_vert_2001(self) == VGD_ERROR ) {
      printf("(Cvgd) in Cvgd_new_from_table, problem decoding table with vcode 2001\n");
      return(VGD_ERROR);
    }
    break;
  case 1003:
  case 5001:
    if( c_decode_vert_1003_5001(self) == VGD_ERROR ) {
      printf("(Cvgd) in Cvgd_new_from_table, problem decoding table with vcode 1003 or 5001\n");
      return(VGD_ERROR);
    }
    break;
  case 5002:
  case 5003:
  case 5004:
  case 5005:
    if( c_decode_vert_5002_5003_5004_5005(self) == VGD_ERROR ) {
      printf("(Cvgd) in Cvgd_new_from_table, problem decoding table with vcode 5002,5003,5004 or 5005\n");
      return(VGD_ERROR);
    }
    break;
  default:
    printf("(Cvgd) in Cvgd_new_from_table, invalid Vcode %d\n", (*self)->vcode);
    return(VGD_ERROR);
  }
  (*self)->valid = 1;
  if(fstd_init(*self) == VGD_ERROR) {
    printf("(Cvgd) ERROR in Cvgd_new_from_table, problem creating record information\n");
  }

  return(VGD_OK);
}

static int C_genab_1002(float *etauser, int nk, double *ptop_8, double **a_m_8, double **b_m_8, int **ip1_m)
{
  // Andre Plante May 2015.   
  char ok=1;
  int k;

  if( my_alloc_double(a_m_8, nk, "(Cvgd) ERROR in C_genab_1002, malloc error with a_m_8") == VGD_ERROR )
    return(VGD_ERROR);
  if( my_alloc_double(b_m_8, nk, "(Cvgd) ERROR in C_genab_1002, malloc error with b_m_8") == VGD_ERROR )
    return(VGD_ERROR);
  if( my_alloc_int   (ip1_m, nk, "(Cvgd) ERROR in C_genab_1002, malloc error with ip1_m") == VGD_ERROR )
    return(VGD_ERROR);

  // For eta, relax the test on etauser[nk-1] != 1. to allow legacy construction of
  // partial atmospheric levels. Some users just wanted the top half of the atmosphere and
  // this test was making their call to vgrid to bomb. Since there is no more model
  // using eta this is safe.

  if(etauser[nk-1] > 1.){
    printf("WRONG SPECIFICATION OF ETA VERTICAL LEVELS: ETA(NK-1) MUST BE LESS OR EQUAL TO 1.0\n");
    ok=0;
  }
  //Check monotonicity
  for ( k = 1; k < nk; k++){
    if(etauser[k] <= etauser[k-1]){
      printf(" WRONG SPECIFICATION OF ETA VERTICAL LEVELS: LEVELS MUST BE MONOTONICALLY INCREASING\n");
      ok=0;
      break;
    }
  }
  if(! ok){
    printf("   Current choice:\n");
    for ( k = 0; k < nk; k++){
      printf("   %f\n", etauser[k]);
    }
    return(VGD_ERROR);
  }

  if( *ptop_8 <= 0.) {
    printf("(Cvgd) ERROR in C_genab_1002: ptop = %f must be greater than zero\n", *ptop_8);
    return(VGD_ERROR);
  }

  int ip1, kind2;
  float eta;
  for ( k = 0; k < nk; k++){
    ip1 = c_convip_Level2IP_old_style(etauser[k],1);
    eta = c_convip_IP2Level(ip1,&kind2);
    (*ip1_m)[k] = ip1;
    (*a_m_8)[k] = (1. - eta) * (*ptop_8);
    (*b_m_8)[k] = eta;
  }

  return(VGD_OK);
}

static int C_genab_2001(float *pres, int nk, double **a_m_8, double **b_m_8, int **ip1_m)
{

  // Andre Plante May 2015. 
  char ok = 1;
  int k;
  
  if( my_alloc_double(a_m_8, nk, "(Cvgd) ERROR in C_genab_2001, malloc error with a_m_8") == VGD_ERROR )
    return(VGD_ERROR);
  if( my_alloc_double(b_m_8, nk, "(Cvgd) ERROR in C_genab_2001, malloc error with b_m_8") == VGD_ERROR )
    return(VGD_ERROR);
  if( my_alloc_int   (ip1_m, nk, "(Cvgd) ERROR in C_genab_2001, malloc error with ip1_m") == VGD_ERROR )
    return(VGD_ERROR);
  
  //Check monotonicity
  for ( k = 1; k < nk; k++){
    if(pres[k] <= pres[k-1]){
      printf("WRONG SPECIFICATION OF PRESSURE VERTICAL LEVELS: LEVELS MUST BE MONOTONICALLY INCREASING\n");
      ok=0;
      break;
    }
  }
  if(! ok){
    printf("   Current choice:\n");
    for ( k = 0; k < nk; k++){
      printf("   %f\n", pres[k]);
    }
    return(VGD_ERROR);
  }

  for ( k = 0; k < nk; k++){
    (*a_m_8)[k] = pres[k] * 100.;
    (*b_m_8)[k] = 0.;
    // Go back and forth to ip1 in order to make sure pres value is encodable.
    (*ip1_m)[k] = c_convip_Level2IP(pres[k],2);
  }

  return(VGD_OK);
  
}

static int C_genab_5001(float *hybuser, int nk, float rcoef, double ptop_8, double pref_8, double **a_m_8, double **b_m_8, int **ip1_m)
{
  // Andre Plante May 2015. 
  char ok = 1;
  int k;
  int complet, ip1, kind2;
  float epsilon=1.0e-6, f_one=1.f;
  double hybtop = ptop_8 / pref_8;
  double hyb, pr1;
   
  if( my_alloc_double(a_m_8, nk, "(Cvgd) ERROR in C_genab_5001, malloc error with a_m_8") == VGD_ERROR )
    return(VGD_ERROR);
  if( my_alloc_double(b_m_8, nk, "(Cvgd) ERROR in C_genab_5001, malloc error with b_m_8") == VGD_ERROR )
    return(VGD_ERROR);
  if( my_alloc_int   (ip1_m, nk, "(Cvgd) ERROR in C_genab_5001, malloc error with ip1_m") == VGD_ERROR )
    return(VGD_ERROR);

  if( memcmp( &(hybuser[nk-1]), &f_one, sizeof(float)/sizeof(char)) ){
    printf("WRONG SPECIFICATION OF HYB VERTICAL LEVELS: HYB(NK) MUST BE 1.0\n");
    ok=0;
  }
  //Check monotonicity
  for ( k = 1; k < nk; k++){
    if(hybuser[k] <= hybuser[k-1]){
      printf(" WRONG SPECIFICATION OF HYB VERTICAL LEVELS: LEVELS MUST BE MONOTONICALLY INCREASING\n");
      ok=0;
      break;
    }
  }
  if(! ok){
    printf("   Current choice:\n");
    for ( k = 0; k < nk; k++){
      printf("   %f\n", hybuser[k]);
    }
    return(VGD_ERROR);
  }

  if( ptop_8 <= 0.) {
    printf("(Cvgd) ERROR in C_genab_5001: ptop = %f must be greater than zero\n", ptop_8);
    return(VGD_ERROR);
  }

  if( ( ptop_8 - hybuser[0] * pref_8 ) / ptop_8 > epsilon ) {
    printf("(Cvgd) ERROR in C_genab_5001: ptop = %f is lower than first hyb level = %f\n", ptop_8, hybuser[0]*pref_8);
    return(VGD_ERROR);
  }

  ip1 = c_convip_Level2IP( (float) hybtop, 5);
  hybtop = (double) c_convip_IP2Level(ip1,&kind2);
  pr1 = 1./(1.-hybtop);
  
  // Find out if first level is at top
  
  if( fabs( ptop_8 - hybuser[0] * pref_8 ) / ptop_8 < epsilon) {
    complet = 1;
  } else{
    printf("(Cvgd) NOTE: First hyb level is not at model top\n");
    complet = 0;
  }

  for ( k = 0; k < nk; k++){
    ip1 = c_convip_Level2IP(hybuser[k],5);
    hyb = c_convip_IP2Level(ip1,&kind2);
    (*ip1_m)[k] = ip1;
    (*b_m_8)[k] = pow( (hyb - hybtop) * pr1, rcoef);
    (*a_m_8)[k] = pref_8 * ( hyb - (*b_m_8)[k] );
  }
  if(complet) {
    (*b_m_8)[0] = 0.;
    (*a_m_8)[0] = ptop_8;
  }
    
  return(VGD_OK);
}

static int C_genab_5002_5003(float *hybuser, int nk, int *nl_m, int *nl_t, float rcoef1, float rcoef2, double ptop_8, double pref_8, double **PP_a_m_8, double **PP_b_m_8, int **PP_ip1_m, double **PP_a_t_8, double **PP_b_t_8, int **PP_ip1_t, int tlift)
{
  // Andre Plante May 2015.
  
  // Processing option
  if( ! ( tlift == 0 || tlift == 1 ) ){
    printf("(Cvgd) ERROR in C_genab_5002_5003, wrong value given to tlift, expecting 0 (for false) or 1 (for true), got %d\n",tlift);
    fflush(stdout);
    return(VGD_ERROR);
  }

  // Define local pointers pointing to "pointer to pointer" to simplify equation below
  double *a_m_8, *b_m_8, *a_t_8, *b_t_8;
  int *ip1_m, *ip1_t;
    
  char ok = 1;
  int k;
  float hybtop, rcoef;
  double zsrf_8, ztop_8, zeta_8, lamba_8, pr1;  
  
  *nl_m = nk + 1;
  *nl_t = nk + 2;

  *PP_a_m_8 = malloc( (*nl_m)*sizeof(double) );
  if(! *PP_a_m_8){
    printf("(Cvgd) ERROR in C_genab_5002_5003, malloc error with *PP_a_m_8\n");
    return(VGD_ERROR);
  }
  *PP_b_m_8 = malloc( (*nl_m)*sizeof(double) );
  if(! *PP_b_m_8){
    printf("(Cvgd) ERROR in C_genab_5002_5003, malloc error with *PP_b_m_8\n");
    return(VGD_ERROR);
  }
  *PP_ip1_m = malloc( (*nl_m)*sizeof(int) );
  if(! *PP_ip1_m){
    printf("(Cvgd) ERROR in C_genab_5002_5003, malloc error with *PP_ip1_m\n");
    return(VGD_ERROR);
  }
  *PP_a_t_8 = malloc( (*nl_t)*sizeof(double) );
  if(! *PP_a_t_8){
    printf("(Cvgd) ERROR in C_genab_5002_5003, malloc error with *PP_a_t_8\n");
    return(VGD_ERROR);
  }
  *PP_b_t_8 = malloc( (*nl_t)*sizeof(double) );
  if(! *PP_b_t_8){
    printf("(Cvgd) ERROR in C_genab_5002_5003, malloc error with *PP_b_t_8\n");
    return(VGD_ERROR);
  }
  *PP_ip1_t = malloc( (*nl_t)*sizeof(int) );
  if(! *PP_ip1_t){
    printf("(Cvgd) ERROR in C_genab_5002_5003, malloc error with *PP_ip1_t\n");
    return(VGD_ERROR);
  }

  a_m_8 = *PP_a_m_8;
  b_m_8 = *PP_b_m_8;
  ip1_m = *PP_ip1_m;
  a_t_8 = *PP_a_t_8;
  b_t_8 = *PP_b_t_8;
  ip1_t = *PP_ip1_t;

  zsrf_8  = log(pref_8);
  if ( ptop_8 <= 0. ) {
    printf("(Cvgd) ERROR in C_genab_5002_5003: ptop_8 must be > 0, got %f\n", ptop_8);
    fflush(stdout);
    return(VGD_ERROR);
  }
  ztop_8  = log(ptop_8);

  // Checking vertical layering

  //    Check range
  hybtop = (float) (ptop_8 / pref_8);
  if( hybuser[nk-1] >= 1. ) {
    printf("(Cvgd) ERROR in C_genab_5002_5003: hyb must be < 1.0, got %f\n", hybuser[nk-1]);
    fflush(stdout);
    return(VGD_ERROR);
  }
  if( hybuser[0] <= hybtop ) {
    printf("(Cvgd) ERROR in C_genab_5002_5003: hyb must be > %f, got %f\n", hybtop, hybuser[0]);
    fflush(stdout);
    return(VGD_ERROR);
  }

  //Check monotonicity
  for ( k = 1; k < nk; k++){
    if(hybuser[k] <= hybuser[k-1]){
      printf(" WRONG SPECIFICATION OF HYB VERTICAL LEVELS: LEVELS MUST BE MONOTONICALLY INCREASING\n");
      ok=0;
      break;
    }
  }
  if(! ok){
    printf("   Current choice:\n");
    for ( k = 0; k < nk; k++){
      printf("   %f\n", hybuser[k]);
    }
    fflush(stdout);
    return(VGD_ERROR);
  }

  // Momentum levels
  pr1 = 1. / (zsrf_8 - ztop_8);
  for( k = 0; k < nk; k++ ) {
    zeta_8  = zsrf_8 + log(hybuser[k]);
    lamba_8  = ( zeta_8 - ztop_8 ) * pr1;
    rcoef  = (float) (rcoef2 - ( rcoef2 - rcoef1 ) * lamba_8);
    b_m_8[k] = pow(lamba_8, rcoef);
    a_m_8[k] = zeta_8;
  }
  a_m_8[nk] = zsrf_8;
  b_m_8[nk] = 1.;

  // Thermodynamic levels    
  for( k = 1; k < nk; k++ ) {
    b_t_8[k] = 0.5 * ( b_m_8[k] + b_m_8[k-1] );
    a_t_8[k] = 0.5 * ( a_m_8[k] + a_m_8[k-1] );
  }
  // Special thermo levels
  b_t_8[0]    = 0.5 * ( b_m_8[0]    + 0.    );
  b_t_8[nk]   = 0.5 * ( b_m_8[nk-1] + 1.    );
  b_t_8[nk+1] = 1.;
  a_t_8[0]    = 0.5 * ( a_m_8[0]    + ztop_8);
  a_t_8[nk]   = 0.5 * ( a_m_8[nk-1] + zsrf_8);
  a_t_8[nk+1] = zsrf_8;

  if( tlift ){
    a_t_8[nk]   = a_m_8[nk-1];
    b_t_8[nk]   = b_m_8[nk-1];
  }

  // Compute ip1 values
  for(k = 0; k < nk; k++ ) {
    ip1_m[k] = c_convip_Level2IP(hybuser[k],5);
  }
  ip1_m[nk] = c_convip_Level2IP(1.,5);
  
  ip1_t[0]    = c_convip_Level2IP( sqrtf( hybtop     * hybuser[0]   ), 5 );
  for(k = 1; k < nk; k++ ) {
    ip1_t[k]  = c_convip_Level2IP( sqrtf( hybuser[k] * hybuser[k-1] ), 5 );
  }
  if( tlift ){
    ip1_t[nk]   = c_convip_Level2IP( hybuser[nk-1] , 5 );
  } else {
    ip1_t[nk]   = c_convip_Level2IP( sqrtf( hybuser[nk-1]*1.0f ), 5 );
  }
  ip1_t[nk+1] = c_convip_Level2IP(1.,5);
  
  return(VGD_OK);

}

static int C_genab_5004(float *hybuser, int nk, int *nl_m, int *nl_t, float rcoef1, float rcoef2, double ptop_8, double pref_8, double **PP_a_m_8, double **PP_b_m_8, int **PP_ip1_m, double **PP_a_t_8, double **PP_b_t_8, int **PP_ip1_t)
{
  // Andre Plante May 2015.
  
  // Processing option

  // Define local pointers pointing to "pointer to pointer" to simplify equation below
  double *a_m_8, *b_m_8, *a_t_8, *b_t_8;
  int *ip1_m, *ip1_t;
    
  char ok = 1;
  int k;
  float hybtop, rcoef;
  double zsrf_8, ztop_8, zeta_8, lamba_8, pr1, zetau_8, zeta2_8, l_ptop_8;  
  
  *nl_m = nk + 1;
  *nl_t = nk + 1;

  *PP_a_m_8 = malloc( (*nl_m)*sizeof(double) );
  if(! *PP_a_m_8){
    printf("(Cvgd) ERROR in C_genab_5004, malloc error with *PP_a_m_8\n");
    return(VGD_ERROR);
  }
  *PP_b_m_8 = malloc( (*nl_m)*sizeof(double) );
  if(! *PP_b_m_8){
    printf("(Cvgd) ERROR in C_genab_5004, malloc error with *PP_b_m_8\n");
    return(VGD_ERROR);
  }
  *PP_ip1_m = malloc( (*nl_m)*sizeof(int) );
  if(! *PP_ip1_m){
    printf("(Cvgd) ERROR in C_genab_5004, malloc error with *PP_ip1_m\n");
    return(VGD_ERROR);
  }
  *PP_a_t_8 = malloc( (*nl_t)*sizeof(double) );
  if(! *PP_a_t_8){
    printf("(Cvgd) ERROR in C_genab_5004, malloc error with *PP_a_t_8\n");
    return(VGD_ERROR);
  }
  *PP_b_t_8 = malloc( (*nl_t)*sizeof(double) );
  if(! *PP_b_t_8){
    printf("(Cvgd) ERROR in C_genab_5004, malloc error with *PP_b_t_8\n");
    return(VGD_ERROR);
  }
  *PP_ip1_t = malloc( (*nl_t)*sizeof(int) );
  if(! *PP_ip1_t){
    printf("(Cvgd) ERROR in C_genab_5004, malloc error with *PP_ip1_t\n");
    return(VGD_ERROR);
  }

  a_m_8 = *PP_a_m_8;
  b_m_8 = *PP_b_m_8;
  ip1_m = *PP_ip1_m;
  a_t_8 = *PP_a_t_8;
  b_t_8 = *PP_b_t_8;
  ip1_t = *PP_ip1_t;

  zsrf_8  = log(pref_8);

  if ( lrint(ptop_8) == -2 || lrint(ptop_8) == -1 ) {
    // Auto compute ptop and make B(1) = 0
    zetau_8 = zsrf_8 + log(hybuser[0]);
    zeta2_8 = zsrf_8 + log(hybuser[1]);
    ztop_8  = 0.5 * ( 3. * zetau_8 - zeta2_8);
    l_ptop_8 = exp(ztop_8);
    if( lrint(ptop_8) == -1 ) {
      // Compute B(1) from ztop, B(1) != 0
      zetau_8 = ztop_8;
    }
  } else if (ptop_8 <= 0.) {
    printf("(Cvgd) ERROR in C_genab_5004: ptop_8 must be > 0, got %f\n",ptop_8);
    return(VGD_ERROR);
  } else {
    // Take B(1) from user's ztop
    l_ptop_8 = ptop_8;
    ztop_8  = log(ptop_8);
    zetau_8 = ztop_8;
  }

  // Checking vertical layering

  //    Check range
  hybtop = (float) (l_ptop_8 / pref_8);
  if( hybuser[nk-1] >= 1. ) {
    printf("(Cvgd) ERROR in C_genab_5004: hyb must be < 1.0, got %f\n", hybuser[nk-1]);
    return(VGD_ERROR);
  }
  if( hybuser[0] <= hybtop ) {
    printf("(Cvgd) ERROR in C_genab_5004: hyb must be > %f, got %f\n", hybtop, hybuser[0]);
    return(VGD_ERROR);
  }

  //Check monotonicity
  for ( k = 1; k < nk; k++){
    if(hybuser[k] <= hybuser[k-1]){
      printf(" WRONG SPECIFICATION OF HYB VERTICAL LEVELS: LEVELS MUST BE MONOTONICALLY INCREASING\n");
      ok=0;
      break;
    }
  }
  if(! ok){
    printf("   Current choice:\n");
    for ( k = 0; k < nk; k++){
      printf("   %f\n", hybuser[k]);
    }
    return(VGD_ERROR);
  }

  // Momentum levels
  pr1 = 1. / (zsrf_8 - zetau_8);
  for( k = 0; k < nk; k++ ) {
    zeta_8  = zsrf_8 + log(hybuser[k]);
    lamba_8  = ( zeta_8 - zetau_8 ) * pr1;
    rcoef  = (float) (rcoef2 - ( rcoef2 - rcoef1 ) * lamba_8);
    b_m_8[k] = pow(lamba_8, rcoef);
    a_m_8[k] = zeta_8;
  }
  a_m_8[nk] = zsrf_8;
  b_m_8[nk] = 1.;

  // Thermodynamic levels    
  for( k = 0; k < nk; k++ ) {
    b_t_8[k] = 0.5 * ( b_m_8[k+1] + b_m_8[k] );
    a_t_8[k] = 0.5 * ( a_m_8[k+1] + a_m_8[k] );
  }
  // Special thermo levels
  b_t_8[nk] = 1.;
  a_t_8[nk] = zsrf_8;

  // Compute ip1 values
  for(k = 0; k < nk; k++ ) {
    ip1_m[k] = c_convip_Level2IP(hybuser[k],5);    
  }
  ip1_m[nk] = c_convip_Level2IP(1.,5);

  for(k = 0; k < nk-1; k++ ) {
    ip1_t[k]  = c_convip_Level2IP( sqrtf( hybuser[k+1] * hybuser[k] ), 5 );
  }
  ip1_t[nk-1] = c_convip_Level2IP( sqrtf( 1.f * hybuser[nk-1] ), 5 );
  ip1_t[nk]   = c_convip_Level2IP(1.,5);
  
  return(VGD_OK);

}

static int c_vgrid_genab_5005(float *hybuser, int nk, int *nl_m, int *nl_t, float rcoef1, float rcoef2, double **ptop_out_8, double pref_8, double **PP_a_m_8, double **PP_b_m_8, int **PP_ip1_m, double **PP_a_t_8, double **PP_b_t_8, int **PP_ip1_t, float dhm, float dht)
{
  // Andre Plante May 2015.

  // Define local pointers pointing to "pointer to pointer" to simplify equation below
  double *a_m_8, *b_m_8, *a_t_8, *b_t_8;
  int *ip1_m, *ip1_t;
    
  char ok = 1;
  int k;
  float hybtop, rcoef;
  double zsrf_8, ztop_8, zeta_8, lamba_8, pr1, zetau_8, zeta2_8;
  
  *nl_m = nk + 2;
  *nl_t = nk + 2;

  *PP_a_m_8 = malloc( (*nl_m)*sizeof(double) );
  if(! *PP_a_m_8){
    printf("(Cvgd) ERROR in c_vgrid_genab_5005, malloc error with *PP_a_m_8\n");
    return(VGD_ERROR);
  }
  *PP_b_m_8 = malloc( (*nl_m)*sizeof(double) );
  if(! *PP_b_m_8){
    printf("(Cvgd) ERROR in c_vgrid_genab_5005, malloc error with *PP_b_m_8\n");
    return(VGD_ERROR);
  }
  *PP_ip1_m = malloc( (*nl_m)*sizeof(int) );
  if(! *PP_ip1_m){
    printf("(Cvgd) ERROR in c_vgrid_genab_5005, malloc error with *PP_ip1_m\n");
    return(VGD_ERROR);
  }
  *PP_a_t_8 = malloc( (*nl_t)*sizeof(double) );
  if(! *PP_a_t_8){
    printf("(Cvgd) ERROR in c_vgrid_genab_5005, malloc error with *PP_a_t_8\n");
    return(VGD_ERROR);
  }
  *PP_b_t_8 = malloc( (*nl_t)*sizeof(double) );
  if(! *PP_b_t_8){
    printf("(Cvgd) ERROR in c_vgrid_genab_5005, malloc error with *PP_b_t_8\n");
    return(VGD_ERROR);
  }
  *PP_ip1_t = malloc( (*nl_t)*sizeof(int) );
  if(! *PP_ip1_t){
    printf("(Cvgd) ERROR in c_vgrid_genab_5005, malloc error with *PP_ip1_t\n");
    return(VGD_ERROR);
  }

  a_m_8 = *PP_a_m_8;
  b_m_8 = *PP_b_m_8;
  ip1_m = *PP_ip1_m;
  a_t_8 = *PP_a_t_8;
  b_t_8 = *PP_b_t_8;
  ip1_t = *PP_ip1_t;

  zsrf_8  = log(pref_8);
  
  // Auto compute ptop and make B(0) = 0
  zetau_8 = zsrf_8 + log(hybuser[0]);
  zeta2_8 = zsrf_8 + log(hybuser[1]);
  ztop_8  = 0.5 * ( 3. * zetau_8 - zeta2_8);
  (**ptop_out_8) = exp(ztop_8);

  // Checking vertical layering

  //    Check range
  hybtop = (float) ( (**ptop_out_8) / pref_8 );
  if( hybuser[nk-1] >= 1. ) {
    printf("(Cvgd) ERROR in c_vgrid_genab_5005: hyb must be < 1.0, got %f\n", hybuser[nk-1]);
    return(VGD_ERROR);
  }
  if( hybuser[0] <= hybtop ) {
    printf("(Cvgd) ERROR in c_vgrid_genab_5005: hyb must be > %f, got %f\n", hybtop, hybuser[0]);
    return(VGD_ERROR);
  }

  //Check monotonicity
  for ( k = 1; k < nk; k++){
    if(hybuser[k] <= hybuser[k-1]){
      printf(" WRONG SPECIFICATION OF HYB VERTICAL LEVELS: LEVELS MUST BE MONOTONICALLY INCREASING\n");
      ok=0;
      break;
    }
  }
  if(! ok){
    printf("   Current choice:\n");
    for ( k = 0; k < nk; k++){
      printf("   %f\n", hybuser[k]);
    }
    return(VGD_ERROR);
  }

  // Momentum levels
  pr1 = 1. / (zsrf_8 - zetau_8);
  for( k = 0; k < nk; k++ ) {
    zeta_8  = zsrf_8 + log(hybuser[k]);
    lamba_8  = ( zeta_8 - zetau_8 ) * pr1;
    rcoef  = (float) (rcoef2 - ( rcoef2 - rcoef1 ) * lamba_8);
    b_m_8[k] = pow(lamba_8, rcoef);
    a_m_8[k] = zeta_8;
  }
  a_m_8[nk] = zsrf_8;
  b_m_8[nk] = 1.;
  // Integrating the hydrostatic eq with T=0C
  // ln[p(z=dhm)] = ln(ps) - g/(Rd*T)*dhm
  // s = ln(ps) - ln(pref)
  // ln[p(z=dhm)] = ln(pref) - g/(Rd*T)*dhm + s
  // => B=1, A = ln(pref) - g/(Rd*T)*dhm
  // We take T at 0C
  a_m_8[nk+1] = c_comp_diag_a_height(pref_8,dhm);
  b_m_8[nk+1] = 1.;

  // Thermodynamic levels    
  for( k = 0; k < nk; k++ ) {
    b_t_8[k] = 0.5 * ( b_m_8[k+1] + b_m_8[k] );
    a_t_8[k] = 0.5 * ( a_m_8[k+1] + a_m_8[k] );
  }
  // Special thermo levels
  b_t_8[nk]   = 1.;
  a_t_8[nk]   = zsrf_8;
  a_t_8[nk+1] = c_comp_diag_a_height(pref_8,dht);
  b_t_8[nk+1] = 1.;

  // Compute ip1 values
  for(k = 0; k < nk; k++ ) {
    ip1_m[k] = c_convip_Level2IP(hybuser[k],5);    
  }
  ip1_m[nk] = c_convip_Level2IP(1.,5);
  // Encoding kind= 4       : M  [metres] (height with respect to ground level)
  ip1_m[nk+1] = c_convip_Level2IP(dhm,4);

  for(k = 0; k < nk-1; k++ ) {
    ip1_t[k]  = c_convip_Level2IP( sqrtf( hybuser[k+1] * hybuser[k] ), 5 );
  }
  ip1_t[nk-1] = c_convip_Level2IP( sqrtf( 1.f * hybuser[nk-1] ), 5 );
  ip1_t[nk]   = c_convip_Level2IP(1.,5);
  // Encoding kind= 4       : M  [metres] (height with respect to ground level)
  ip1_t[nk+1] = c_convip_Level2IP(dht,4);
  
  return(VGD_OK);

}

int Cvgd_getopt_int(char *key, int *value, int quiet)
{
  if(! value){
    printf("(Cvgd) ERROR in Cvgd_getopt_int, value is a NULL pointer\n");
    return(VGD_ERROR);
  }
  if (strcmp(key, "ALLOW_SIGMA") == 0){
      *value = ALLOW_SIGMA;
  } else {
    if(! quiet) {
      printf("(Cvgd) ERROR in Cvgd_getopt_int, invalid key %s\n",key);
      fflush(stdout);
    }
    return(VGD_ERROR);
  }  
  
  return(VGD_OK);
}

int Cvgd_get_int(vgrid_descriptor *self, char *key, int *value, int quiet)
{  
  if(! Cvgd_is_valid(self,"SELF")){
    printf("(Cvgd) ERROR in Cvgd_get_int, invalid vgrid.\n");
    return(VGD_ERROR);
  }
  if(! value){
    printf("(Cvgd) ERROR in Cvgd_get_int, value is a NULL pointer\n");
    return(VGD_ERROR);
  }
  if (strcmp(key, "NL_M") == 0){
    *value = self->nl_m;
  } else if (strcmp(key, "NL_T") == 0){
    *value = self->nl_t;
  } else if (strcmp(key, "KIND") == 0){
    *value = self->kind;
  } else if (strcmp(key, "VERS") == 0){
    *value = self->version;
  } else if( strcmp(key, "DATE") == 0){
    *value = self->rec.dateo;
  } else if (strcmp(key, "IG_1") == 0){
    *value = self->rec.ig1;
  } else if (strcmp(key, "IG_2") == 0){
    *value = self->rec.ig2;
  } else if (strcmp(key, "IG_3") == 0){
    *value = self->rec.ig3;
  } else if (strcmp(key, "IG_4") == 0){
    *value = self->rec.ig4;
  } else if (strcmp(key, "IP_1") == 0){
    *value = self->rec.ip1;
  } else if (strcmp(key, "IP_2") == 0){
    *value = self->rec.ip2;
  } else if (strcmp(key, "DIPM") == 0){
    *value = self->ip1_m[self->nl_m-1];
  } else if (strcmp(key, "DIPT") == 0){
    *value = self->ip1_t[self->nl_t-1];
  } else if (strcmp(key, "MIPG") == 0){
    *value = self->match_ipig;
  } else if (strcmp(key, "LOGP") == 0){
    *value = is_valid(self,is_in_logp);
  } else {
    if(! quiet) {
      printf("(Cvgd) ERROR in Cvgd_get_int, invalid key %s\n",key);
      fflush(stdout);
    }
    return(VGD_ERROR);
  }
  
  return(VGD_OK);

}

int Cvgd_get_int_1d(vgrid_descriptor *self, char *key, int **value, int *nk, int quiet)
{
  int OK = 1;
  if(nk) *nk = -1;
  if(! Cvgd_is_valid(self,"SELF")){
    printf("(Cvgd) ERROR in Cvgd_get_int_1d, invalid vgrid.\n");
    return(VGD_ERROR);
  }
  if(strcmp(key, "VIP1") == 0 ){
    if( is_valid(self,ip1_m_valid) ){
      printf("(Cvgd) ERROR in Cvgd_get_int_1d, depricated key '%s' use VIPM instead.\n", key);
      fflush(stdout);
      return(VGD_ERROR);
    } else {
      OK = 0;
    }
  }
  if( strcmp(key, "VIPM") == 0 ){
    if( is_valid(self,ip1_m_valid) ){
      if(! *value){
	(*value) = malloc(self->nl_m * sizeof(int));
	if(! *value){
	  printf("(Cvgd) ERROR in Cvgd_get_int_1d, problem allocating %d int\n",self->nl_m);
	  return(VGD_ERROR);
	}
      }
      my_copy_int(self->ip1_m, value, self->nl_m);
      if(nk) *nk = self->nl_m;
    } else {
      OK = 0;
    }
  } else  if( strcmp(key, "VIPT") == 0 ){
    if( is_valid(self,ip1_t_valid_get) ){
      if(! *value){
	(*value) = malloc(self->nl_t * sizeof(int));
	if(! *value){
	  printf("(Cvgd) ERROR in Cvgd_get_int_1d, problem allocating %d int\n",self->nl_t);
	  return(VGD_ERROR);
	}
      }
      my_copy_int(self->ip1_t, value, self->nl_t);
      if(nk) *nk = self->nl_t;
    } else {
      OK = 0;
    }
  }else{
    OK = 0;
  }
  if(! OK) {
    if(! quiet) {
      printf("(Cvgd) ERROR in Cvgd_get_int_1d, invalid key '%s' for Vcode %d\n",key, self->vcode);
      fflush(stdout);
    }
    return(VGD_ERROR);    
  }
  return(VGD_OK);

}

int Cvgd_get_float(vgrid_descriptor *self, char *key, float *value, int quiet) {

  if(! Cvgd_is_valid(self,"SELF")){
    printf("(Cvgd) ERROR in Cvgd_get_float, invalid vgrid.\n");
    return(VGD_ERROR);
  }
  if(! value){
    printf("(Cvgd) ERROR in Cvgd_get_float, value is a NULL pointer\n");
    return(VGD_ERROR);
  }  

  if( strcmp(key, "RC_1" ) == 0 ){    
    if( is_valid(self,rcoef1_valid) ){
      *value = self->rcoef1;
    } else {
      *value = (float) c_get_error(key,quiet);
    }
  } else  if( strcmp(key, "RC_2" ) == 0 ){
    if( is_valid(self,rcoef2_valid) ){
      *value = self->rcoef2;
    } else {
      *value = (float) c_get_error(key,quiet);
    }
  } else  if( strcmp(key, "DHM " ) == 0 ){
    if( is_valid(self,dhm_valid) ){
      *value = self->dhm;
    } else {
      *value = (float) c_get_error(key,quiet);
    }
  } else  if( strcmp(key, "DHT " ) == 0 ){
    if( is_valid(self,dht_valid) ){
      *value = self->dht;
    } else {
      *value = (float) c_get_error(key,quiet);
    }
  } else {
    if(! quiet) {
      printf("(Cvgd) ERROR in Cvgd_get_float, invalid key '%s'\n",key);
      fflush(stdout);
    }
    return(VGD_ERROR);
  }
  return(VGD_OK);

}

int Cvgd_get_float_1d(vgrid_descriptor *self, char *key, float **value, int *nk, int quiet)
{
  char key2[5];
  int *vip1=NULL, kind, k, OK = 1;
  if(nk) *nk = -1;
  if(! Cvgd_is_valid(self,"SELF")){
    printf("(Cvgd) ERROR in Cvgd_get_float_1d, invalid vgrid.\n");
    return(VGD_ERROR);
  }
  if( strcmp(key, "VCDM") == 0 ){
    if (is_valid(self,ip1_m_valid)) {
      if(! *value){
	(*value) = malloc(self->nl_m * sizeof(float));
	if(! *value){
	  printf("(Cvgd) ERROR in Cvgd_get_float_1d, problem allocating %d double\n",self->nl_m);
	  return(VGD_ERROR);
	}
      }    
      strcpy(key2,"VIPM");
      Cvgd_get_int_1d(self, key2, &vip1, NULL, quiet);
      for(k = 0; k < self->nl_m; k++){
	(*value)[k] = c_convip_IP2Level(vip1[k], &kind);
      }
      free(vip1);
      if(nk) *nk = self->nl_m;
    } else {
      OK = 0;
    }
  } else if( strcmp(key, "VCDT") == 0 ){
    if (is_valid(self,ip1_t_valid_get)) {
      if(! *value){
	(*value) = malloc(self->nl_t * sizeof(float));
	if(! *value){
	  printf("(Cvgd) ERROR in Cvgd_get_float_1d, problem allocating %d double\n",self->nl_t);
	  return(VGD_ERROR);
	}
      }  
      strcpy(key2,"VIPT");
      Cvgd_get_int_1d(self, key2, &vip1, NULL, quiet);
      for(k = 0; k < self->nl_t; k++){
	(*value)[k] = c_convip_IP2Level(vip1[k], &kind);
      }    
      free(vip1);
      if(nk) *nk = self->nl_t;
    } else {
      OK = 0;
    }
  } else {
    OK = 0;
  }
  if(! OK){
    if(! quiet) {
      printf("(Cvgd) ERROR in Cvgd_get_float_1d, invalid key '%s' for vcode %d.\n",key, self->vcode);
      fflush(stdout);
    }
    return(VGD_ERROR);    
  }
  return(VGD_OK);
}

static int c_get_put_double(vgrid_descriptor **self, char *key, double *value_get, double value_put, int quiet, char *action) {
  int get, OK = 1;
  if(! Cvgd_is_valid(*self,"SELF")){
    printf("(Cvgd) ERROR in c_get_double, invalid vgrid.\n");
    return(VGD_ERROR);
  }
  
  if(strcmp(action,"GET") == 0 ) {
    get = 1;
  } else if (strcmp(action,"PUT") == 0 ) {
    get = 0;
  } else {
    printf("(Cvgd) INTERNAL ERROR using c_get_put_double, please report to developers\n");
    return(VGD_ERROR);
  }
  if( strcmp(key, "PTOP") == 0 ) {
    if(! is_valid(*self, ptop_8_valid)) OK = 0;
    if(get) {
      *value_get = (*self)->ptop_8;
    } else {
      (*self)->ptop_8 = value_put;
    }
  } else if ( strcmp(key, "PREF") == 0 ) {
    if(! is_valid(*self, pref_8_valid)) OK = 0;
    if(get) {
      *value_get = (*self)->pref_8;
    } else {
      (*self)->pref_8 = value_put;
    }
  } else if ( strcmp(key, "RC_1") == 0 ) {
    if(! is_valid(*self, rcoef1_valid)) OK = 0;
    if(get) {
      *value_get = (*self)->rcoef1;
    } else {
      (*self)->rcoef1 = (float) value_put;
    }
  } else if ( strcmp(key, "RC_2") == 0 ) {
    if(! is_valid(*self, rcoef2_valid)) OK = 0;
    if(get) {
      *value_get = (*self)->rcoef2;
    } else {
      (*self)->rcoef2 = (float) value_put;
    }
  } else {
    if(! quiet) {
      printf("(Cvgd) ERROR in Cvgd_put_double, invalid key '%s'\n", key);
      fflush(stdout);
    }
    return(VGD_ERROR);
  }
  
  if(! OK) {
    if(! quiet) {
      printf("(Cvgd) ERROR in Cvgd_put_double, %s cannot be put for Vcode %d\n", key, (*self)->vcode);
      fflush(stdout);
    }
    return(VGD_ERROR);
  }    
  
  return(VGD_OK);

}

int Cvgd_put_double(vgrid_descriptor **self, char *key, double value_put) {
  double value_get = 0.0; // Will not be used
  int quiet = 0; //not quiet
  return(c_get_put_double(self, key, &value_get, value_put, quiet, "PUT"));
}

int Cvgd_get_double(vgrid_descriptor *self, char *key, double *value_get, int quiet)
{
  double value_put = 0.0; //Will not be used
  return(c_get_put_double(&self, key, value_get, value_put, quiet, "GET"));
}

int Cvgd_get_double_1d(vgrid_descriptor *self, char *key, double **value, int *nk, int quiet)
{
  int OK = 1;
  if(nk) *nk = -1;
  if(! Cvgd_is_valid(self,"SELF")){
    printf("(Cvgd) ERROR in Cvgd_get_double_1d, invalid vgrid.\n");
    return(VGD_ERROR);
  }
  if( strcmp(key, "CA_M") == 0 || strcmp(key, "COFA") == 0 ){
    if( is_valid(self,a_m_8_valid) ) {
      if(! *value){
	(*value) = malloc(self->nl_m * sizeof(double));
	if(! *value){
	  printf("(Cvgd) ERROR in Cvgd_get_double_1d, problem allocating %d double\n",self->nl_m);
	  return(VGD_ERROR);
	}
      }
      my_copy_double(self->a_m_8, value, self->nl_m);
      if(nk) *nk = self->nl_m;
    } else {
      OK = 0;
    }
  } else if( strcmp(key, "CB_M") == 0 || strcmp(key, "COFB") == 0 ) {
    if( is_valid(self,b_m_8_valid) ) {
      if(! *value){
	(*value) = malloc(self->nl_m * sizeof(double));
	if(! *value){
	  printf("(Cvgd) ERROR in Cvgd_get_double_1d, problem allocating %d double\n",self->nl_m);
	  return(VGD_ERROR);
	}
      }
      my_copy_double(self->b_m_8, value, self->nl_m);
      if(nk) *nk = self->nl_m;
    } else {
      OK = 0;
    }
  } else if( strcmp(key, "CA_T") == 0 ){
    if (is_valid(self,a_t_8_valid_get) ) {
      if(! *value){
	(*value) = malloc(self->nl_t * sizeof(double));
	if(! *value){
	  printf("(Cvgd) ERROR in Cvgd_get_double_1d, problem allocating %d double\n",self->nl_t);
	  return(VGD_ERROR);
	}
      }
      my_copy_double(self->a_t_8, value, self->nl_t);
      if(nk) *nk = self->nl_t;
    } else {
      OK = 0;
    }
  } else if( strcmp(key, "CB_T") == 0 ){
    if( is_valid(self,b_t_8_valid_get) ) {
      if(! *value){
	(*value) = malloc(self->nl_t * sizeof(double));
	if(! *value){
	  printf("(Cvgd) ERROR in Cvgd_get_double_1d, problem allocating %d double\n",self->nl_t);
	  return(VGD_ERROR);
	}
      }
      my_copy_double(self->b_t_8, value, self->nl_t);
      if(nk) *nk = self->nl_t;
    } else {
      OK =0;
    }
  } else {
    OK = 0;
  }    
  if( ! OK) {
    if(! quiet) {
      printf("(Cvgd) ERROR in Cvgd_get_double_1d, invalid key '%s' for vcode %d\n", key, self->vcode);
      fflush(stdout);
    }
    return(VGD_ERROR);
  }

  return(VGD_OK);

}

int Cvgd_get_double_3d(vgrid_descriptor *self, char *key, double **value, int *ni, int *nj, int *nk, int quiet)
{
  if(ni) *ni = -1;
  if(nj) *nj = -1;
  if(nk) *nk = -1;    
  if(! Cvgd_is_valid(self,"SELF")){
    printf("(Cvgd) ERROR in Cvgd_get_double_3d, invalid vgrid.\n");
    return(VGD_ERROR);
  }
  int table_size = self->table_ni * self->table_nj * self->table_nk;
  if( strcmp(key, "VTBL") == 0 ){
    if(! *value){
      (*value) = malloc( table_size * sizeof(double));
      if(! *value){
	printf("(Cvgd) ERROR in Cvgd_get_double_3d, problem allocating %d double.\n",table_size);
	return(VGD_ERROR);
      }
    }
    my_copy_double(self->table, value, table_size);
    if(ni) *ni = self->table_ni;
    if(nj) *nj = self->table_nj;
    if(nk) *nk = self->table_nk;
  } else {
    if(! quiet) {
      printf("(Cvgd) ERROR in Cvgd_get_double_3d, invalid key '%s'\n",key);
      fflush(stdout);
    }
    return(VGD_ERROR);
  }

  return(VGD_OK);
}

int Cvgd_get_char(vgrid_descriptor *self, char *key, char out[], int quiet) {
  if(! Cvgd_is_valid(self,"SELF")){
    printf("(Cvgd) ERROR in Cvgd_get_char, invalid vgrid structure.\n");
    return(VGD_ERROR);
  }
  if( strcmp(key, "ETIK") == 0 ){
    strcpy(out,self->rec.etiket);
  } else if( strcmp(key, "NAME") == 0 ){
    strcpy(out,self->rec.nomvar);
  } else if( strcmp(key, "RFLD") == 0 ){
    strcpy(out,self->ref_name);
  } else {
    if(! quiet){
      printf("(Cvgd) ERROR in Cvgd_get_char, invalid key -> '%s'\n",key);
    }
    return(VGD_ERROR);
  }
  return(VGD_OK);
}

int Cvgd_put_char(vgrid_descriptor **self, char *key, char *value) {
  if(! Cvgd_is_valid(*self,"SELF")){
    printf("(Cvgd) ERROR in Cvgd_put_char, invalid vgrid.\n");
    return(VGD_ERROR);
  }
  if( strcmp(key, "ETIK") == 0 ){
    strcpy((*self)->rec.etiket,value);
  } else if( strcmp(key, "NAME") == 0 ){
    strcpy((*self)->rec.nomvar,value);
  } else if( strcmp(key, "RFLD") == 0 ){
    strcpy((*self)->ref_name,value);
  } else {
    printf("(Cvgd) ERROR in Cvgd_out_char, invalid key -> '%s'\n",key);
    return(VGD_ERROR);
  }
  return(VGD_OK);
}

int Cvgd_putopt_int(char *key, int value) {
  if( strcmp(key, "ALLOW_SIGMA") == 0 ) {
    ALLOW_SIGMA = value;
  } else {
    printf("(Cvgd) ERROR in Cvgd_putopt_int, invalid key %s\n", key);
    return(VGD_ERROR);
  }
  return(VGD_OK);
}
    
int Cvgd_put_int(vgrid_descriptor **self, char *key, int value) {
  
  if(! self) {
    printf("(Cvgd) ERROR in Cvgd_put_int, vgrid is a null pointer.\n");
    return(VGD_ERROR);
  }
  
  if(! Cvgd_is_valid((*self),"SELF")){
    printf("(Cvgd) ERROR in Cvgd_put_int, invalid vgrid.\n");
    return(VGD_ERROR);
  }
  if( strcmp(key, "DATE") == 0 ) {
    (*self)->rec.dateo = value;
  } else if( strcmp(key, "IG_1") == 0 ) {
       (*self)->rec.ig1 = value;
  } else if( strcmp(key, "IG_2") == 0 ) {
       (*self)->rec.ig2 = value;
  } else if( strcmp(key, "IG_3") == 0 ) {
       (*self)->rec.ig3 = value;
  } else if( strcmp(key, "IG_4") == 0 ) {
       (*self)->rec.ig4 = value;
  } else if( strcmp(key, "IP_1") == 0 ) {
       (*self)->rec.ip1 = value;
  } else if( strcmp(key, "IP_2") == 0 ) {
       (*self)->rec.ip2 = value;
  } else if( strcmp(key, "IP_3") == 0 ) {
    (*self)->rec.ip3 = value;
  } else if( strcmp(key, "DIPM") == 0 ) {
    if ( is_valid((*self), dhm_valid)) {
      (*self)->ip1_m[(*self)->nl_m -1 ] = value;
      (*self)->a_m_8[(*self)->nl_m -1 ] = c_comp_diag_a_ip1((*self)->pref_8, value);
      if( c_table_update(self) == VGD_ERROR) {
	printf("(Cvgd) ERROR in Cvgd_put_int, problem with c_table_update for key %s\n",key);
	return(VGD_ERROR);
      }
    } else {
      printf("(Cvgd) ERROR in Cvgd_put_int, DIPM cannot be put for Vcode %d\n", (*self)->vcode);
      return(VGD_ERROR);
    }
  } else if( strcmp(key, "DIPT") == 0 ) {
    if ( is_valid((*self), dht_valid)) {
      (*self)->ip1_t[(*self)->nl_t - 1] = value;
      (*self)->a_t_8[(*self)->nl_t - 1] = c_comp_diag_a_ip1((*self)->pref_8, value);
      if( c_table_update(self) == VGD_ERROR) {
	printf("(Cvgd) ERROR in Cvgd_put_int, problem with c_table_update for key %s\n", key);
	return(VGD_ERROR);
      }
    } else {
      printf("(Cvgd) ERROR in Cvgd_put_int, DIPT cannot be put for Vcode %d\n", (*self)->vcode);
      return(VGD_ERROR);
    }
  } else {
    printf("(Cvgd) ERROR in Cvgd_put_int, invalid key %s\n", key);
    return(VGD_ERROR);
  }
  return(VGD_OK);
}

int Cvgd_new_gen(vgrid_descriptor **self, int kind, int version, float *hyb, int size_hyb, float *rcoef1, float *rcoef2,
	      double *ptop_8, double *pref_8, double *ptop_out_8,
	      int ip1, int ip2, float *dhm, float *dht)
{
  printf("IN Cvgd_new_gen hyb = %p\n",hyb);
  printf("IN Cvgd_new_gen hyb[0] = %f, hyb[1] = %f\n",hyb[0],hyb[1]);
  float *hybm = NULL;
  double *a_m_8 = NULL, *b_m_8 = NULL, *a_t_8 = NULL, *b_t_8 = NULL;
  int *ip1_m = NULL, *ip1_t = NULL, tlift, errorInput;

  if(*self){
    Cvgd_free(self);
  }

  *self = c_vgd_construct();
  if(! *self){
    printf("(Cvgd) ERROR in Cvgd_new_gen, null pointer returned by c_vgd_construct\n");
    return (VGD_ERROR);
  }

  if(Cvgd_set_vcode_i(*self, kind, version) == VGD_ERROR)  {
    printf("(Cvgd) ERROR in Cvgd_new_gen, ERROR with Cvgd_set_vcode_i");
    return (VGD_ERROR);
  }

  //TODO get better error handling like in new_build
  errorInput = 0;
  errorInput = errorInput + is_required_double((*self), ptop_8,     ptop_8_valid,     "ptop_8"    );
  errorInput = errorInput + is_required_double((*self), ptop_out_8, ptop_out_8_valid, "ptop_out_8");
  errorInput = errorInput + is_required_double((*self), pref_8,     pref_8_valid,     "pref_8"    );
  errorInput = errorInput + is_required_float ((*self), rcoef1,     rcoef1_valid,     "rcoef1"    );
  errorInput = errorInput + is_required_float ((*self), rcoef2,     rcoef2_valid,     "rcoef2"    );
  errorInput = errorInput + is_required_float ((*self), dhm,        dhm_valid,        "dhm"       );
  errorInput = errorInput + is_required_float ((*self), dht,        dht_valid,        "dht"       );  

  if (errorInput != 7 ) {
    return(VGD_ERROR);
  }

  int nk = -1, nl_m = -1, nl_t = -1;

  switch((*self)->vcode) {
  case 1001:	
    nk   = size_hyb;
    nl_m = size_hyb;
    nl_t = size_hyb;
    if(C_genab_1001(hyb, size_hyb, &a_m_8, &b_m_8, &ip1_m) == VGD_ERROR ) {
      free(hybm);
      free(a_m_8);
      free(b_m_8);
      free(ip1_m);
      return(VGD_ERROR);
    }
    break;
  case 1002:
    nk   = size_hyb;
    nl_m = size_hyb;
    nl_t = size_hyb;
    if(C_genab_1002(hyb, size_hyb, ptop_8, &a_m_8, &b_m_8, &ip1_m) == VGD_ERROR ) {
      free(a_m_8);
      free(b_m_8);
      free(ip1_m);
      return(VGD_ERROR);
    }
    break;
  case 1003:
    fprintf(stderr,"(Cvgd) ERROR in Cvgd_new_gen, kind=%d, version=%d\n cannot be generated, please use kind 1 of version 2\n",kind,version);
    return(VGD_ERROR);
    break;
  case 2001:
    nk   = size_hyb;
    nl_m = size_hyb;
    nl_t = -1;
    if(C_genab_2001(hyb, size_hyb, &a_m_8, &b_m_8, &ip1_m) == VGD_ERROR ) {
      free(a_m_8);
      free(b_m_8);
      free(ip1_m);
      return(VGD_ERROR);
    }
    break;
  case 5001:
    nk   = size_hyb;
    nl_m = size_hyb;
    nl_t = size_hyb;
    if(C_genab_5001(hyb, size_hyb, *rcoef1, *ptop_8, *pref_8, &a_m_8, &b_m_8, &ip1_m) == VGD_ERROR ) {
      free(a_m_8);
      free(b_m_8);
      free(ip1_m);
      return(VGD_ERROR);
    }
    break;
  case 5002:
    nk   = size_hyb;
    tlift = 0;
    if(C_genab_5002_5003(hyb, size_hyb, &nl_m, &nl_t, *rcoef1, *rcoef2, *ptop_8, *pref_8, &a_m_8, &b_m_8, &ip1_m, &a_t_8, &b_t_8, &ip1_t, tlift) == VGD_ERROR ) {
      free(a_m_8);
      free(b_m_8);
      free(ip1_m);
      free(a_t_8);
      free(b_t_8);
      free(ip1_t);
      return(VGD_ERROR);
    }    
    break;
  case 5003:
    nk   = size_hyb;
    tlift = 1;
    if(C_genab_5002_5003(hyb, size_hyb, &nl_m, &nl_t, *rcoef1, *rcoef2, *ptop_8, *pref_8, &a_m_8, &b_m_8, &ip1_m, &a_t_8, &b_t_8, &ip1_t, tlift) == VGD_ERROR ) {
      free(a_m_8);
      free(b_m_8);
      free(ip1_m);
      free(a_t_8);
      free(b_t_8);
      free(ip1_t);
      return(VGD_ERROR);
    }    
    break;
  case 5004:
    nk   = size_hyb;
    if(C_genab_5004(hyb, size_hyb, &nl_m, &nl_t, *rcoef1, *rcoef2, *ptop_8, *pref_8, &a_m_8, &b_m_8, &ip1_m, &a_t_8, &b_t_8, &ip1_t) == VGD_ERROR ) {
      free(a_m_8);
      free(b_m_8);
      free(ip1_m);
      free(a_t_8);
      free(b_t_8);
      free(ip1_t);
      return(VGD_ERROR);
    }    
     break;
  case 5005:
    nk   = size_hyb;
    if(c_vgrid_genab_5005(hyb, size_hyb, &nl_m, &nl_t, *rcoef1, *rcoef2, &ptop_out_8, *pref_8, &a_m_8, &b_m_8, &ip1_m, &a_t_8, &b_t_8, &ip1_t, *dhm, *dht) == VGD_ERROR ) {
      free(a_m_8);
      free(b_m_8);
      free(ip1_m);
      free(a_t_8);
      free(b_t_8);
      free(ip1_t);
      return(VGD_ERROR);

    }
    break;
  default:
    printf("(Cvgd) ERROR in Cvgd_new_gen, invalid kind or version, kind = %d, version = %d\n",kind,version);
    return(VGD_ERROR);
  }

  if( VGD_ERROR == Cvgd_new_build_vert(self,kind,version,nk,ip1,ip2,ptop_8,pref_8,rcoef1,rcoef2,a_m_8,b_m_8,a_t_8,b_t_8,ip1_m,ip1_t,nl_m,nl_t) ) {
    fprintf(stderr,"(Cvgd) ERROR in Cvgd_new_gen, problem with new_build_vert for kind = %d, version = %d\n",kind,version);
    return(VGD_ERROR);
  }
  free(hybm);
  free(a_m_8);
  free(b_m_8);
  free(a_t_8);
  free(b_t_8);
  free(ip1_m);  
  free(ip1_t);  

  return (VGD_OK);
}

static int C_get_consistent_pt_e1(int iun, float *val, char *nomvar ){
  int error, ni, nj, nk, nmax=1000, infon, k;
  int liste[nmax];
  float *work;
  VGD_TFSTD_ext var;

  error = c_fstinl(iun, &ni, &nj, &nk, -1, " ", -1, -1, -1, " ", nomvar, liste, &infon, nmax);
  if (error < 0) {
    printf("(Cvgd) ERROR in C_get_consistent_pt_e1, with fstinl\n");
    return(VGD_ERROR);
  }
  
  if( infon > 1 ){
    printf("(Cvgd)  More than one %s checking consistency ...\n",nomvar);
  }

  if( my_alloc_float(&work, ni*nj, "(Cvgd) ERROR in C_get_consistent_pt_e1, unable to allocate work") == VGD_ERROR )
    return(VGD_ERROR);

  for( k = 0; k < infon; k++ ){
    if( my_fstprm(liste[k], &var) == VGD_ERROR ){
      goto bomb;
    }
    if ( var.ni != ni && var.nj != nj && var.nk != nk ){
	printf("(Cvgd) ERROR: in C_get_consistent_pt_e1, dim misatch for %s, expected (%d,%d,%d), got (%d,%d,%d)\n", nomvar, ni, nj, nk, var.ni, var.nj, var.nk);
      goto bomb;
    }
    if( c_fstluk(work,liste[k],&ni,&nj,&nk) < 0 ){
      printf("(Cvgd) ERROR: in C_get_consistent_pt_e1, with c_fstluk");
    }
    if( k == 0 ){
      *val = work[0];
    } else {
      if( memcmp( &(work[0]), val, sizeof(float)/sizeof(char)) ){
	printf("(Cvgd) ERROR: in C_get_consistent_pt_e1, inconsistent %s, %f v %f\n", nomvar, work[0], *val);
	goto bomb;
      }
    }
  }
  printf("(Cvgd)   All %s consistent\n", nomvar);
  free(work);
  return(VGD_OK);
 bomb:
  free(work);
  return(VGD_ERROR);
}

static int C_get_consistent_hy(int iun, VGD_TFSTD_ext var, VGD_TFSTD_ext *va2, char *nomvar ){
  int error, ni, nj, nk, nmax=1000, infon, ind;
  int liste[nmax];
  VGD_TFSTD_ext va3;

  // Note: HY has dateo not datev
  error = c_fstinl(iun, &ni, &nj, &nk, var.dateo, var.etiket, -1, -1, -1, " ", nomvar, liste, &infon, nmax);
  if (error < 0) {
    printf("(Cvgd) ERROR in C_get_consistent_hy, with fstinl\n");
    return(VGD_ERROR);
  }
  
  if( infon == 0 ){
    printf("(Cvgd)  ERROR in C_get_consistent_hy, no record of nomvar = %s, date = %d, etiket = %s found\n", nomvar, var.dateo, var.etiket);
    return(VGD_ERROR);
  }
  
  for( ind = 0; ind < infon; ind++ ){
    if( ind == 0 ){
      if( my_fstprm(liste[ind], va2) == VGD_ERROR ){
	return(VGD_ERROR);
      }
      printf("(Cvgd)   Found matching HY\n");
    } else {
      printf("(Cvgd)   More than one %s, checking consistency ...\n",nomvar);
      if( my_fstprm(liste[ind], &va3) == VGD_ERROR ){
	return(VGD_ERROR);
      }
      if ( va3.ni != ni && va3.nj != nj && va3.nk != nk ){
	printf("(Cvgd) ERROR: in C_get_consistent_hy, dim misatch for %s, expected (%d,%d,%d), got (%d,%d,%d)\n", nomvar, ni, nj, nk, va3.ni, va3.nj, va3.nk);
	return(VGD_ERROR);
      }
      if ( va3.ig1 != va2->ig1 && va3.ig2 != va2->ig2 && va3.ig3 != va2->ig3 && va3.ig4 != va2->ig4 ){
	printf("(Cvgd) ERROR: in C_get_consistent_hy, igs misatch for %s, expected (%d,%d,%d,%d), got (%d,%d,%d,%d)\n", nomvar, va2->ig1, va2->ig2, va2->ig3, va2->ig4, va3.ig1, va3.ig2, va3.ig3, va3.ig4);
	return(VGD_ERROR);
      } 
    }
  }
  if( infon > 1 )
    printf("(Cvgd)   All %s consistent\n", nomvar);
  return(VGD_OK);
}

static int C_gen_legacy_desc(vgrid_descriptor **self, int unit, int *keylist , int nb ){
  
  int *ip1 = NULL;
  int kind, origkind, k, ni, nj, nk, hy_key, pt_key, e1_key;
  float ptop, rcoef;
  float *hyb = NULL, *hybm = NULL;
  double ptop_8, pref_8;
  double *a_m_8 = NULL, *b_m_8 = NULL;
  VGD_TFSTD_ext var, va2;

  if(my_alloc_float (&hyb  ,nb,"(Cvgd) ERROR: in C_gen_legacy_desc, cannot allocate hyb of size")   == VGD_ERROR)
    return(VGD_ERROR);
  if(my_alloc_float (&hybm ,nb,"(Cvgd) ERROR: in C_gen_legacy_desc, cannot allocate hybm of size")  == VGD_ERROR)
    return(VGD_ERROR);

  if( my_fstprm(keylist[0], &var) == VGD_ERROR ){
    printf("(Cvgd) ERROR: in C_gen_legacy_desc, fstprm 1 on key %d\n", keylist[0]);
    goto bomb;
  }
  hyb[0] = c_convip_IP2Level(var.ip1,&kind);
  if( kind != 1 && kind != 2 && kind != 5 ){
    printf("(Cvgd) ERROR: in C_gen_legacy_desc, kind = %d, has to be 1, 2 or 5\n", kind);
    goto bomb;
  }
  origkind=kind;

  for( k = 1; k < nb; k++ ){
    if( my_fstprm(keylist[k], &va2) == VGD_ERROR ){
      printf("(Cvgd) ERROR: in C_gen_legacy_desc, fstprm 2 on key %d\n", keylist[k]);
      goto bomb;
    }
    if ( va2.ni != var.ni && va2.nj != var.nj && va2.nk != var.nk ){
      printf("(Cvgd) ERROR: in C_gen_legacy_desc, dim misatch expected (%d,%d,%d), got (%d,%d,%d)\n", var.ni, var.nj, var.nk, va2.ni, va2.nj, va2.nk);
      goto bomb;
    }
    hyb[k] = c_convip_IP2Level(va2.ip1,&kind);
    if( kind != origkind ){
      printf("(Cvgd) ERROR: in C_gen_legacy_desc, expecting kind = %d, got kind = %d\n",origkind, kind);
      goto bomb;
    }
  }

  hy_key = c_fstinf (unit,&ni,&nj,&nk,-1," ",-1,  -1,  -1," ","HY  ");
  pt_key = c_fstinf (unit,&ni,&nj,&nk,-1," ",-1,  -1,  -1," ","PT  ");
  e1_key = c_fstinf (unit,&ni,&nj,&nk,-1," ",-1,  -1,  -1," ","E1  ");

  if( kind == 1 ){
    //============================
    // SIGMA ETA HYBRID-NORMALIZED
    //----------------------------
    if( pt_key >= 0){
      //=============================================
      // PT PT PT PT PT PT PT PT PT PT PT PT PT PT PT
      //---------------------------------------------
      if( C_get_consistent_pt_e1(unit, &ptop,"PT  ") == VGD_ERROR ){
	printf("(Cvgd) ERROR in C_gen_legacy_desc, consistency check on PT failed\n");
	goto bomb;
      }
      if(hy_key >= 0){
	// Verify if HY constistant with PT
	if( C_get_consistent_hy(unit, var, &va2, "HY  ") == VGD_ERROR ){
	  printf("(Cvgd) ERROR in C_gen_legacy_record, consistency check on HY failed (1)\n");
	  goto bomb;
	}
	decode_HY(va2, &ptop_8, &pref_8, &rcoef);
	if( fabs(rcoef - 1.0) > 1.e-5){
	  printf("(Cvgd) ERROR in C_gen_legacy_desc, HY rcoef should by 1.0 since PT record is present in file\n");
	  goto bomb;
	}
	if( fabs( ptop - ptop_8/100.) > 1.e-5 ){
	  printf("(Cvgd) ERROR in C_gen_legacy_desc, ptop from HY is %f while it is %f in PT record\n",ptop_8/100., ptop);
	  goto bomb;
	}
	printf("(Cvgd) INFO : in C_gen_legacy_desc HY record consistent with PT\n");
      }
      if( e1_key >= 0){
	printf("(Cvgd) TODO in C_gen_legacy_desc, add support to 1004 etasef coordinate");
	goto bomb;
      } else {
	printf("(Cvgd)   eta coordinate found\n");
	ptop_8 = ptop*100.;
	if( C_genab_1002(hyb, nb, &ptop_8, &a_m_8, &b_m_8, &ip1) == VGD_ERROR ){	  
	  goto bomb;
	}
	if( Cvgd_new_build_vert(self, kind, 2, nb, var.ip1, var.ip2, &ptop_8, NULL, NULL, NULL, a_m_8, b_m_8, NULL, NULL, ip1, NULL, nb, 0) == VGD_ERROR ){
	  goto bomb;
	}
      }
    } else if ( hy_key >= 0){
      //================================================
      // HY HY HY HY HY HY HY HY HY HY HY HY HY HY HY HY
      //------------------------------------------------
      printf("(Cvgd)   hybrid (normalized) coordinate found\n");
      if( C_get_consistent_hy(unit, var, &va2, "HY  ") == VGD_ERROR ){
	printf("(Cvgd) ERROR in C_gen_legacy_record, consistency check on HY failed (2)\n");
	goto bomb;
      }
      printf("C_gen_legacy_desc TO CONTINUE hybrid (normalized) coordinate with HY and no PT\n");
      return(VGD_ERROR);
    } else {
      // SIGMA SIGMA SIGMA SIGMA SIGMA SIGMA SIGMA SIGMA
      if( ! ALLOW_SIGMA ){
	printf("(Cvgd)   C_gen_legacy_desc error: sigma coordinate construction is not ALLOWED.\n(Cvgd)       If your are certain that you want this sigma coordinate, set ALLOW_SIGMA to true e.g.\n(Cvgd)          in fortran stat =  vgd_putopt(\"ALLOW_SIGMA\",.true.)\n(Cvgd)          in C       stat = Cvgd_putopt_int(\"ALLOW_SIGMA\",1)\n");
	goto bomb;
      }
      if( C_genab_1001(hyb, nb, &a_m_8, &b_m_8, &ip1) == VGD_ERROR ){
	goto bomb;
      }
      if( Cvgd_new_build_vert(self, kind, 1, nb, var.ip1, var.ip2, NULL, NULL, NULL, NULL, a_m_8, b_m_8, NULL, NULL, ip1, NULL, nb, 0) == VGD_ERROR ){
	goto bomb;
      }
    }
    
  } else if ( kind == 2 ){
    printf("(Cvgd)   pressure coordinate found\n");
    if( C_genab_2001(hyb, nb, &a_m_8, &b_m_8, &ip1) == VGD_ERROR ){
      goto bomb;
    }
    if( Cvgd_new_build_vert(self, kind, 1, nb, var.ip1, var.ip2, NULL, NULL, NULL, NULL, a_m_8, b_m_8, NULL, NULL, ip1, NULL, nb, 0) == VGD_ERROR ){
      goto bomb;
    }	
  } else if ( kind == 5 ){
    printf("(Cvgd)   Hybrid coordinate found\n");
    if( C_get_consistent_hy(unit, var, &va2, "HY  ") == VGD_ERROR ){
      printf("(Cvgd) ERROR in C_gen_legacy_desc, consistency check on HY failed\n");
      goto bomb;
    }
    decode_HY(va2, &ptop_8, &pref_8, &rcoef);
    if( C_genab_5001(hyb, nb, rcoef, ptop_8, pref_8, &a_m_8, &b_m_8, &ip1) == VGD_ERROR ){
      goto bomb;
    }
    if( Cvgd_new_build_vert(self, kind, 1, nb, var.ip1, var.ip2, &ptop_8, &pref_8, &rcoef, NULL, a_m_8, b_m_8, NULL, NULL, ip1, NULL, nb, 0) == VGD_ERROR ){
      goto bomb;
    }	
  } else {
    printf("(Cvgd ERROR: in C_gen_legacy_desc, kind %d is not supported\n",kind);
    return(VGD_ERROR);
  }
  free(ip1);
  free(hyb);
  free(hybm);
  free(a_m_8);
  free(b_m_8);
  return(VGD_OK);
  
 bomb:
  free(ip1);
  free(hyb);
  free(hybm);
  free(a_m_8);
  free(b_m_8);
  return(VGD_ERROR);

}

static int c_legacy(vgrid_descriptor **self, int unit, int F_kind) {
  // Construct vertical structure from legacy encoding (PT,HY...)

  int error, ni, nj, nk, nip1, i, j, k, kind, nb_kind=100, aa, nb;
  int count, nkeylist = MAX_DESC_REC, valid_kind;
  int keylist[nkeylist], ip1list[nkeylist], num_in_kind[nb_kind];
  float preslist[nkeylist], xx, f_zero=0.f;
  VGD_TFSTD_ext var;

  for( i = 0; i < nb_kind; i++){
    num_in_kind[i] = 0;
  }

  if(F_kind > 0) {
    printf("(Cvgd) Looking for kind = %d\n",F_kind);
  }
  error = c_fstinl(unit, &ni, &nj, &nk, -1, " ", -1, -1, -1, " ", " ", keylist, &count, nkeylist);
  if (error < 0) {
    printf("(Cvgd) ERROR in c_legacy, with fstinl\n");
    return(VGD_ERROR);
  }
  nip1 = 0;
  for( i = 0; i < count; i++){
    error = my_fstprm(keylist[i], &var);
    if (error == VGD_ERROR) {
      printf("(Cvgd) ERROR in c_legacy, error return from fstprm wrapper for fst key = %d",keylist[i]);
      return(VGD_ERROR);
    }
    preslist[i] = c_convip_IP2Level(var.ip1,&kind);
    if( strcmp(var.nomvar, ">>  ") == 0 )
      continue;
    if( strcmp(var.nomvar, "^^  ") == 0 )
      continue;
    if( strcmp(var.nomvar, "^>  ") == 0 )
      continue;
    if( strcmp(var.nomvar, "P0   ") == 0 )
      continue;
    if( strcmp(var.nomvar, "PT  ") == 0 )
      continue;
    if( strcmp(var.nomvar, "HY  ") == 0 )
      continue;
    if( kind == 2 ){
      // Pressure at 0.0 is not part of the vertical structure
      if( memcmp( &(preslist[i]), &f_zero, sizeof(float)/sizeof(char) ) == 0 )
	continue;
    }
    if(F_kind > 0 && kind != F_kind)
      continue;
    if(kind == 1 || kind == 2 || kind == 5) {
      num_in_kind[kind] = num_in_kind[kind]+1;
      ip1list[nip1]=var.ip1;
      keylist[nip1]=keylist[i];
      preslist[nip1]=preslist[i];
      valid_kind=kind;
      nip1++;
    }
  }
  if(max_int(num_in_kind,nb_kind) != nip1){
    printf("(Cvgd) ERROR: more than one pressure/sigma/hyb coordinate in file\n");
    for(i = 0; i < nb_kind; i++){
      if(num_in_kind[i] > 0) {
	printf("(Cvgd)           There are %d records of kind %d\n",num_in_kind[i],i);
      }
    }
    return(VGD_ERROR);
  }
  // Sort levels in ascending order
  for( i = 0; i < nip1 - 1; i++){
    k = i;
    for( j = i + 1; j < nip1; j++){
      if( preslist[j] < preslist[k] )
	k = j;
    }
    if( k != i ){
      // hyb
      xx          = preslist[k];
      preslist[k] = preslist[i];
      preslist[i] = xx;
      // ip1
      aa          = ip1list[k];
      ip1list[k]  = ip1list[i];
      ip1list[i]  = aa;	
      // fstkey
      aa          = keylist[k];
      keylist[k]  = keylist[i];
      keylist[i]  = aa;
    }
  }
  // Remove duplictate (there must be a better way to do this)
  for( i = 0; i < nip1-1; i++ ){
    if( ip1list[i] != -1 ){
      for( j = i+1; j < nip1; j++ ){
	if( ip1list[j] == ip1list[i] ){
	  ip1list[j] = -1;
	}
      }
    }
  }
  nb=0;
  for( i = 0; i < nip1; i++ ){
    if( ip1list[i] != -1 ){
      ip1list[nb]  = ip1list[i];
      keylist[nb]  = keylist[i];
      // pres is not used below but adjusting for consistency and possible future use.
      preslist[nb] = preslist[i];
      nb++;
    }
  }
  if( nb == 0){
    printf("(Cvgd) ERROR: No record of type pressure/sigma/hyb in file");
    return(VGD_ERROR);
  }
  printf("(Cvgd)   Found %d unique ip1 of kind %d among the %d records in file to construct the vertical descriptor\n", nb, valid_kind, count);
  error = C_gen_legacy_desc(self, unit, keylist , nb);
  if( error == VGD_ERROR ){
    printf("(Cvgd) ERROR: problem with C_gen_legacy_desc\n");
    return(VGD_ERROR);
  }  
  printf("(Cvgd)   Vertical descriptor successfully reconstructed\n");
  return(VGD_OK);
}

int Cvgd_new_read(vgrid_descriptor **self, int unit, int ip1, int ip2, int kind, int version) {

  char  match_ipig;
  int error, i, ni, nj, nk;
  int toc_found = 0, count, nkeyList = MAX_DESC_REC;
  int keyList[nkeyList], status;
  VGD_TFSTD_ext var;
  vgrid_descriptor *self2;

  if(*self){
    Cvgd_free(self);
  }
  *self = c_vgd_construct();
  if(! *self){
    printf("(Cvgd) ERROR in Cvgd_new_read, null pointer returned by c_vgd_construct\n");
    return (VGD_ERROR);
  }
  
  if(ip1 >= 0 && ip2 < 0) {
    printf("(Cvgd) ERROR in Cvgd_new_read, expecting optional value ip2\n");      
    return (VGD_ERROR);
  }
  
  if(ip2 >= 0 && ip1 < 0){
    printf("(Cvgd) ERROR in Cvgd_new_read, expecting optional value ip1\n");      
    return (VGD_ERROR);
  }
  match_ipig = 0;
  if(ip1 >= 0){
    match_ipig = 1;
  }
  if(kind == -1 && version != -1) {
    printf("(Cvgd) ERROR in Cvgd_new_read, option kind must be used with option version\n");
    return (VGD_ERROR);
  }
  
  error = c_fstinl(unit, &ni, &nj, &nk, -1, " ", ip1, ip2, -1, " ", ZNAME, keyList, &count, nkeyList);
  if (error < 0) {
    printf("(Cvgd) ERROR in Cvgd_new_read, with fstinl on nomvar !!\n");
    return(VGD_ERROR);
  }
  if(count == 0){
    printf("(Cvgd) Cannot find %s with the following ips: ip1=%d, ip2=%d\n", ZNAME, ip1, ip2);
    if(match_ipig) {
      (*self)->vcode = -1;
      return(VGD_ERROR);
    }
    printf("(Cvgd) Trying to construct vgrid descriptor from legacy encoding (PT,HY ...)\n");
    if(c_legacy(self,unit,kind) == VGD_ERROR){
      printf("(Cvgd) ERROR: failed to construct vgrid descriptor from legacy encoding\n");
      return(VGD_ERROR);
    }
    if(fstd_init(*self) == VGD_ERROR) {
      printf("(Cvgd) ERROR in Cvgd_new_read, problem creating record information\n");
    }
    toc_found = 1;
  } else {
    // Loop on all !! found
    for( i=0; i < count; i++) {     
      // Check if kind and version match, skip the !! if not.
      if( correct_kind_and_version(keyList[i], kind, version, &var, &status) == VGD_ERROR) {
	(*self)->valid = 0;
	return(VGD_ERROR);
      }
      if( status != 1) {
	continue;
      }
      // If we reached this stage then the toc satisfy the selection criteria but it may not be the only one.
      if(! toc_found) {
	toc_found = 1;
	if( C_load_toctoc(*self,var,keyList[i]) == VGD_ERROR ) {
	  printf("(Cvgd) ERROR in Cvgd_new_read, cannot load !!\n");
	  return(VGD_ERROR);
	}
	ni=(*self)->table_ni;
	nj=(*self)->table_nj;
	nk=(*self)->table_nk;
	continue;
      }
      // If we get at this point this means that there are more than one toc satisfying the selection criteria.
      // We load then all to check if they are the same. If not, we return with an error message.
      self2 = c_vgd_construct();
      if( my_fstprm(keyList[i], &var) == VGD_ERROR ) {
	printf("(Cvgd) ERROR in Cvgd_new_read, with my_fstprm on keyList[i] = %d\n",keyList[i]);
	return(VGD_ERROR);
      }
      if( C_load_toctoc(self2,var,keyList[i]) == VGD_ERROR ) {
	printf("(Cvgd) ERROR in Cvgd_new_read, cannot load !!\n");
	return(VGD_ERROR);
      }
      status = Cvgd_vgdcmp(*self,self2);
      if ( status != 0 ){
	printf("(Cvgd) ERROR in Cvgd_new_read, found different entries in vertical descriptors after search on ip1 = %d, ip2 = %d, kind = %d, version = %d, status code is %d\n",ip1,ip2,kind,version,status);
	return(VGD_ERROR);
      }
      // TODO verifier si ce Cvgd_free est correct 
      Cvgd_free(&self2);
    } // Loop in !! 
  } //if(count == 0)

  if(! toc_found) {
    printf("(Cvgd) ERROR in Cvgd_new_read, cannot find !! or it generate from legacy encoding\n");
    return(VGD_ERROR);
  }
  // Fill structure from input table
  if( Cvgd_new_from_table(self, (*self)->table, (*self)->table_ni, (*self)->table_nj, (*self)->table_nk) == VGD_ERROR ) {
    printf("(Cvgd) ERROR in Cvgd_new_read, unable to construct from table\n");
    return(VGD_ERROR);
  }
  (*self)->match_ipig = match_ipig;  
  
  return(VGD_OK);
}

int Cvgd_write_desc (vgrid_descriptor *self, int unit) {
  int ip1, ip2;
  float work[1];

  if(! self){
    printf("(Cvgd) ERROR in Cvgd_write_desc, vgrid descriptor not constructed\n");
    return(VGD_ERROR);
  }  
  if(! self->valid) {
    printf("(Cvgd) ERROR in Cvgd_write_desc, vgrid structure is not valid %d\n", self->valid);    
    return(VGD_ERROR);
  }
  ip1=self->rec.ip1;
  if(self->rec.ip1 < 0) ip1=0;
  ip2=self->rec.ip2;
  if(self->rec.ip2 < 0) ip2=0;
  
  if( c_fstecr( self->table,      work,            -self->rec.nbits, unit, 
		self->rec.dateo,  self->rec.deet,   self->rec.npas, 
		self->table_ni,   self->table_nj,   self->table_nk, 
		ip1,              ip2,              self->rec.ip3,
		self->rec.typvar, self->rec.nomvar, self->rec.etiket, 
		self->rec.grtyp,  self->rec.ig1,    self->rec.ig2,    self->rec.ig3, self->rec.ig4,
		self->rec.datyp, 1) , 0 ) {
    printf("(Cvgd) ERROR in Cvgd_write_desc, problem with fstecr\n");
    return(VGD_ERROR);
  }

  return(VGD_OK);

}
