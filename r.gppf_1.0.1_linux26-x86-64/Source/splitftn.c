#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static int chop_bang = 0;
static int f90 = 0;
static int nosplit = 0;

#define MAXCAR 128*1024
#define Out(c) fputc(c,stdout)

/* Functions that are overriden in gpp */
int main_gpp (int argc, char **argv);
int fgetc_gpp (FILE *);


void usage (void);


void splitusage (void)
{
  printf ("This utility preprocesses files and reformats (splits) them.\n\n");
  printf
    ("The arguments are passed in two groups. The first group is for the splitter (derived from splitftn).\n");
  printf
    ("The second group of arguments is for the preprocessor (derived from gpp). \nThe groups are separated by \"-gpp\"\n");
  printf ("General usage : r.gpp -f90 -gpp -C filename\n\n");
  printf ("The options for the splitter and the preprocessor are:\n\n");
  printf
    ("Usage : splitftn [-{f90|lang-f90|lang-f90+}] [-chop_bang] [-tempfile filename]\n\n");
  printf (" -f90 : Fortran 90 source code format\n");
  printf (" -chop_bang : Remove comments when safe to do so\n");
  printf (" -nosplit : do not perform the split\n");
  printf (" -tempfile [filename] : Use this file as a tempfile between the preprocessor and the splitter\n\n");
  usage ();
}



static void splitit (ligne)
     char ligne[MAXCAR];
{
  int nc;
  char *pc;
  char *space;
  int single_quote = 0;
  int double_quote = 0;

  pc = &ligne[0];
  if (*pc == '\t')
    nc = 7;
  else
    nc = 0;
  while ((*pc != '\0') && (*pc != '\n') && (nc < MAXCAR))
  {
    if (*pc == ' ')
    {
      space = pc;
      while (*pc == ' ')
      {
        pc++;
        nc++;
      }
      if (*pc == '\n')
      {
        *space = '\n';
      }
    }
    else
    {
      pc++;
      nc++;
    }

  }
  pc = &ligne[0];
  if (*pc == '\t')
    nc = 7;
  else
    nc = 0;
  while ((*pc != '\0') && (*pc != '\n') && (nc < MAXCAR))
  {
    if (*pc == '"')
      double_quote = !double_quote;
    if (*pc == '\'')
      single_quote = !single_quote;

    if (chop_bang)
    {
      if ((*pc == '!') && !(single_quote || double_quote))
        break;
    }
    nc++;
    if ((*pc == '~') && (*(pc + 1) == '~'))
    {
      Out ('\n');
      Out (' ');
      Out (' ');
      Out (' ');
      Out (' ');
      Out (' ');
      Out (' ');
      pc++;
      nc = 6;
    }
    else
    {
      if ((nc > 72) && (f90 == 0))
      {
        Out ('\n');
        Out (' ');
        Out (' ');
        Out (' ');
        Out (' ');
        Out (' ');
        Out ('%');
        Out (*pc);
        nc = 7;
      }
      else if ((nc > 131) && (f90 != 0))
      {
        /* quote-handling doesn't work across input lines!!! */
        if ((*pc == '&') && (!single_quote && !double_quote)) {
	  /* continuation char */
	  Out('&');
	  /* skip chars after & if whitespace or comment */
	  for (pc++; (*pc == ' ') || (*pc == '\t'); pc++);
	  if ((*pc != '!') && (*pc != '\n')) {
	    fprintf(stderr, "error: unexpected character after & (%d)\n", *pc);
	  }
	  break;
	}
	/* requires split: inside or outside string */
        Out ('&');
        Out ('\n');
        Out (' ');
        Out (' ');
        Out (' ');
        Out (' ');
        Out (' ');
        Out ('&');
        Out (*pc);
        nc = 7;
      }
      else
      {
        Out (*pc);
      }
    }
    pc++;
  }
  Out ('\n');
  fflush (stdout);
}

static void outcharsplit (char c)
{
  static char ligne[MAXCAR];
  static int indx = 0;
  int nc = 0;

  /* In F77 mode, remove trailing & continuation character */
  if ( (c == '\n') && (indx > 0) && (f90 == 0) && (ligne[indx-1] == '&') ) 
  {
    indx--;
  }

  ligne[indx++] = c;

  if (c == '\n' || indx >= MAXCAR - 2)
  {
    ligne[MAXCAR - 1] = '\n';
    while (ligne[nc] == ' ' || ligne[nc] == '\t')
      nc++;
    if (ligne[nc] == '\n')
    {
      indx = 0;
      return;
    }                           /* get rid of blank lines */
    if ((ligne[0] != ' ') && (ligne[0] != '\t') && (!isdigit (ligne[0])))
    {
      indx = 0;
      while (ligne[indx] != '\n')
      {
        Out (ligne[indx]);
        indx++;
      }
      Out ('\n');
    }
    else
    {
      splitit (ligne);
    }
    indx = 0;
  }
}

int main (int argc, char **argv)
{
  int i, c, rmtmp = 1;
  char **newargv = NULL;
  char *tempfile = NULL;
  FILE *stream = NULL;

  tempfile = tmpnam (NULL);

  while (--argc)
  {
    argv++;
    if (strcmp (*argv, "-f90") == 0)
    {
      f90 = 1;
      fprintf (stderr, "setting f90 mode\n");
    }
    if (strcmp (*argv, "-nosplit") == 0)
    {
      nosplit = 1;
      fprintf (stderr, "setting nosplit mode\n");
    }
    if (strcmp (*argv, "-lang-f90") == 0)
    {
      f90 = 1;
      fprintf (stderr, "setting f90 mode\n");
    }
    if (strcmp (*argv, "-lang-f90+") == 0)
    {
      f90 = 1;
      fprintf (stderr, "setting f90 mode\n");
    }
    if (strcmp (*argv, "-chop_bang") == 0)
    {
      chop_bang = 1;
      fprintf (stderr, "setting chop_bang mode\n");
    }
    if (strcmp (*argv, "-tempfile") == 0)
    {
      argv++;
      argc--;
      tempfile = *argv;
      fprintf (stderr, "setting tempfile to %s\n", tempfile);
      rmtmp = 0;
    }
    if (strcmp (*argv, "-h") == 0)
    {
      splitusage ();
      exit (0);
    }
    if (strcmp (*argv, "-gpp") == 0)
    {
      /* Add arguments for outputing to file */
      argc += 2;
      newargv = calloc (argc + 1, sizeof (char *));
      for (i = 0; i < argc - 3; i++)
      {
	if ((strcmp(argv[i],"-F")==0) && (f90==1)) /* Small hack to pass f90 mode to gppf */
	  newargv[i]="-f";
	else
	  newargv[i] = argv[i];
      }
      newargv[argc - 3] = "-o";
      newargv[argc - 2] = tempfile;
      newargv[argc - 1] = argv[argc - 3];
      newargv[argc] = NULL;

      main_gpp (argc, newargv);
      stream = fopen (tempfile, "r");
      break;
    }
  }
  if (stream == NULL)
    stream = stdin;
  while ((c = fgetc (stream)) != EOF)
  {
    if (nosplit) 
      putchar(c);
    else 
      outcharsplit (c);
  }
  if (nosplit) 
    putchar('\n');
  else 
    outcharsplit ('\n');

  fclose (stream);

  if (rmtmp)
    remove (tempfile);
}

int fgetc_gpp (FILE * stream)
/* These changes are brought to make gpp compatible with the behaviour of its predecessor */
/* By making gpp compatible with r.ftnpp, we ensure that legacy code does not need to be modified */
{
  static int lastchar = 0;
  static int do_include = 0;
  static int do_define = 0;
  static char buffer[4096];
  static int position = 0;
  static int len = 0;

  /* These statics only for speed, not for state memory */
  static int currentchar = 0;

  currentchar = fgetc (stream);

  /* If last character was a newline, reset our environment */
  if (lastchar == '\n')
    len = 0;
 
    if ((currentchar == EOF) && (lastchar != '\n'))
      /* If EOF, add a newline before EOF if not present */
      /* otherwise gpp will not display the last line    */
    {
      ungetc (currentchar, stream);
      currentchar = '\n';
    }
    else
    {
      if (currentchar == '#')
      {
        do_include = 1;
        do_define = 1;
      }
      else

      {
        /* Make sure that blank exists after #include statement */
        /* otherwise gpp will reject #include<blah>             */
        if (do_include == 1)
        {
          if ((currentchar == '"') || (currentchar == '\'')
              || (currentchar == '<'))
          {
            for (position = 0; position <= len; position++)
            {
              if ((buffer[position] != ' ') && (buffer[position] != '\t'))
              {
                if (strncmp (buffer + position, "#include", 8) == 0)
                {
                  /* We need to add a space here */
                  ungetc (currentchar, stream);
                  currentchar = ' ';
                  do_include = 0;
                  do_define = 0;
                  break;
                }
              }
            }
          }
        }


        if (do_define == 1)
        {
          /* Ensure that "#define xxxx(yyy)" is followed by a space,  */
          /* that "#define xxxxx~~\" is separated as "#define xxxxx ~~\" */
          /* and that "#define xxxxx\" becomes "#define xxxxx \" */
          if (lastchar == ')')
          {
              /* rewind until parenthesis or less than 8 characters left */
              for (position = len; position > 8; position--)
                if (buffer[position] == '(')
                  break;
              if (currentchar == ' ')
              {
                do_define = 0;
              }
          }
          if ( (lastchar == ')') && (currentchar != ' ') || (currentchar == '~') || (currentchar == '\\') )
          {
            /* rewind any characters of the token */
            for (; position > 7; position--)
              if ((buffer[position] == ' ') || (buffer[position] == '\t'))
                break;
            /* rewind blanks */
            for (; position > 6; position--)
                if ((buffer[position] != ' ') && (buffer[position] != '\t'))
            break;
            /* Check if previous string is #define */
            if (strncmp (buffer + position - 6, "#define", 7) == 0)
            {
              /* We need to add a space here */
              ungetc (currentchar, stream);
              currentchar = ' ';
              do_include = 0;
              do_define = 0;
            }
          }  
        }
      }
    }
  


  lastchar = currentchar;
  buffer[len++] = currentchar;

  return currentchar;
}

#define main main_gpp           /* Do not use the main function in gpp.c in the final executable, use the one in this file instead */
#define fgetc fgetc_gpp         /* Use an overrided fgetc in gpp, to do some pre-processing according to our rules */
