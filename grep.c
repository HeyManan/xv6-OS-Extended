// Simple grep.  Only supports ^ . * $ operators.

#include "types.h"
#include "stat.h"
#include "user.h"

char buf[1024];
int match(char*, char*);

int flag_i = 0;
int flag_n = 0;
int flag_v = 0;

char
tolower(char c)
{
  if (c >= 'A' && c <= 'Z')
    return c + ('a' - 'A');
  return c;
}

void
grep(char *pattern, int fd)
{
  int n, m;
  char *p, *q;
  int line_num = 1;

  m = 0;
  while((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){
    m += n;
    buf[m] = '\0';
    p = buf;
    while((q = strchr(p, '\n')) != 0){
      *q = 0;
      int found = match(pattern, p);
      if((found && !flag_v) || (!found && flag_v)){
        if (flag_n) {
          printf(1, "%d:", line_num);
        }
        printf(1, "%s\n", p);
      }
      p = q+1;
      line_num++;
    }
    if(m > 0){
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

int
main(int argc, char *argv[])
{
  int i;
  char *pattern = 0;

  if(argc <= 1){
    printf(2, "usage: grep pattern [file ...]\n");
    exit();
  }

  for(i = 1; i < argc; i++){
    if(argv[i][0] == '-'){
      for(char *s = argv[i]+1; *s; s++){
        switch(*s){
          case 'i': flag_i = 1; break;
          case 'n': flag_n = 1; break;
          case 'v': flag_v = 1; break;
          default:
            printf(2, "grep: unknown flag %c\n", *s);
            exit();
        }
      }
    } else {
      if(pattern == 0){
        pattern = argv[i];
      } else {
        break;
      }
    }
  }

  if(pattern == 0){
    printf(2, "usage: grep [flags] pattern [file...]\n");
    exit();
  }

  if (i == argc) {
    grep(pattern, 0);
  } else {
    for(; i < argc; i++){
      int fd;
      if((fd = open(argv[i], 0)) < 0){
        printf(1, "grep: cannot open %s\n", argv[i]);
        exit();
      }
      grep(pattern, fd);
      close(fd);
    }
  }
  exit();
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9.

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || (flag_i ? tolower(re[0])==tolower(*text) : re[0]==*text) ))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && ((flag_i ? tolower(*text) : *text) == (flag_i ? tolower(c) : c) || c=='.') && text++);
  return 0;
}

