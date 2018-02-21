HCDECKS= \
	BODY_C_compute_pressure_1001_1002.hc  BODY_C_compute_pressure_1003_5001.hc  BODY_C_compute_pressure_2001.hc  BODY_C_compute_pressure_5002_5003_5004_5005.hc  BODY_Cvgd_diag_withref.hc   \
	vgrid_version.hc  
PTNDECKS=
TMPL90DECKS=
FTNDECKS=
HDECKS= \
	vgrid.h  
INCDECKS=
HFDECKS= \
	vgrid_descriptors.hf  
FDECKS=
CHDECKS=
F90DECKS=
FHDECKS=
CDKDECKS=
CDECKS= \
	vgrid.c  
FTN90DECKS=
CDK90DECKS=
PTN90DECKS=
OBJECTS= \
	utils.o  vgrid.o  vgrid_descriptors.o  
utils.o:	utils.F90 \
	vgrid_descriptors.hf  
vgrid.o:	vgrid.c \
	BODY_C_compute_pressure_1001_1002.hc  BODY_C_compute_pressure_5002_5003_5004_5005.hc  vgrid.h  BODY_C_compute_pressure_2001.hc  BODY_Cvgd_diag_withref.hc   \
	BODY_C_compute_pressure_1003_5001.hc  
vgrid_descriptors.o:	vgrid_descriptors.F90 \
	vgrid_descriptors.hf  utils.o  
