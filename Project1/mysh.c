#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Simplifed xv6 shell.

#define MAXARGS 10

// All commands have at least a type. Have looked at the type, the code
// typically casts the *cmd to some specific cmd type.
struct cmd {
  int type;          //  ' ' (exec), | (pipe), '<' or '>' for redirection, ';' for list
};

struct listcmd {
  int type;
  struct cmd *left;//first command to be run
  struct cmd *right;//rest of the commands
};

struct execcmd {
  int type;              // ' '
  char *argv[MAXARGS];   // arguments to the command to be exec-ed
};

struct redircmd {
  int type;          // < or > 
  struct cmd *cmd;   // the command to be run (e.g., an execcmd)
  char *file;        // the input/output file
  int flags;         // flags for open() indicating read or write
  int fd;            // the file descriptor number to use for the file
};

struct pipecmd {
  int type;          // |
  struct cmd *left;  // left side of pipe
  struct cmd *right; // right side of pipe
};

int fork1(void);  // Fork but exits on failure.
struct cmd *parsecmd(char*);

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
  int p[2], r;
  struct execcmd *ecmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;
  struct listcmd *lcmd;

  if(cmd == 0)
    _exit(0);
  
  switch(cmd->type){
  default:
    fprintf(stderr, "unknown runcmd\n");
    _exit(-1);

  case ' ':
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      _exit(0);
    execv(ecmd->argv[0], ecmd->argv);
    fprintf(stderr, "exec %s failed\n", ecmd->argv[0]);
    break;

  case '>':
  case '<':
    rcmd = (struct redircmd*)cmd;
    fprintf(stderr, "redir not implemented\n");
    // Your code here ...
    runcmd(rcmd->cmd);
    break;

  case '|':
    pcmd = (struct pipecmd*)cmd;
    fprintf(stderr, "pipe not implemented\n");
    // Your code here ...
    break;

  case ';'://executing listcmd
    lcmd = (struct listcmd*)cmd;//cast cmd to listcmd
    struct execcmd *test = (struct execcmd *)lcmd->left;
    //fprintf(stdout, "listcmd %s \n", test->argv[1]);
    if(fork1() == 0)//fork shell
      runcmd(lcmd->left);//run first command in child
    int r;
    wait(&r);//parent waits on child
    runcmd(lcmd->right);//parent runs the rest of the list
    break;
  }    
  _exit(0);
}

int
getcmd(char *buf, int nbuf)
{
  if (isatty(fileno(stdin)))
    fprintf(stdout, "CS450$ ");
  memset(buf, 0, nbuf);
  if(fgets(buf, nbuf, stdin) == 0)
    return -1; // EOF
  return 0;
}

int
main(void)
{
  static char buf[100];
  int fd, r;

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      // Clumsy but will have to do for now.
      // Chdir has no effect on the parent if run in the child.
      buf[strlen(buf)-1] = 0;  // chop \n
      if(chdir(buf+3) < 0)
        fprintf(stderr, "cannot cd %s\n", buf+3);
      continue;
    }
    if(fork1() == 0)
      runcmd(parsecmd(buf));
    wait(&r);
  }
  exit(0);
}

int
fork1(void)
{
  int pid;
  
  pid = fork();
  if(pid == -1)
    perror("fork");
  return pid;
}

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = ' ';
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, int type)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = type;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->flags = (type == '<') ?  O_RDONLY : O_WRONLY|O_CREAT|O_TRUNC;
  cmd->fd = (type == '<') ? 0 : 1;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = '|';
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}
/*
list constructor following the template of the other constructors
*/
struct cmd*
listcmd(struct cmd *left, struct cmd *right)
{
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = ';';
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

// Parsing

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>;()";//added the symbols

int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
  case 0:
    break;
  case '|':
  case '<':
  case '('://added cases for new symbols
  case ')':
  case ';':
    s++;
    break;
  case '>':
    s++;
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
    *eq = s;
  
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
  char *s;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))//skips over whitespace
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);

// make a copy of the characters in the input buffer, starting from s through es.
// null-terminate the copy to make it a string.
char 
*mkcopy(char *s, char *es)
{
  int n = es - s;
  char *c = malloc(n+1);
  assert(c);
  strncpy(c, s, n);
  c[n] = 0;
  return c;
}

struct cmd*
parsecmd(char *s)//called from main, where this starts, string s is the input
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s); //makes es, end of s, a pointer to the end of the input buffer
  cmd = parseline(&s, es); //send pointer to pointer of input, pointer to end of input
  peek(&s, es, "");
  if(s != es){
    fprintf(stderr, "leftovers: %s\n", s);
    exit(-1);
  }
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;
  cmd = parsepipe(ps, es);//sends the input to parsepipe first
  if(peek(ps, es, ";")){//after parsing the first command, check to see if it's the first in a list
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);//sends the input to parseexec first
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}
//does nothing if there is no redir command
struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a') {
      fprintf(stderr, "missing file for redirection\n");
      exit(-1);
    }
    switch(tok){
    case '<':
      cmd = redircmd(cmd, mkcopy(q, eq), '<');
      break;
    case '>':
      cmd = redircmd(cmd, mkcopy(q, eq), '>');
      break;
    }
  }
  return cmd;
}
//parses the inside of a parentheses block
struct cmd*
parseblock(char **ps, char *es)
{
  struct cmd *cmd;

  gettoken(ps, es, 0, 0);//strip starting parens
  cmd = parseline(ps, es);
  gettoken(ps, es, 0, 0);//strip final parens

  return cmd;
}

/*
Matchparens loads the outermost parenthetical phrase in ps into q and eq
*/
int
matchparens(char **ps, char *es, char **q, char **eq){
  int c = 0;
  char *s = *ps;
  *q = strchr(*ps, '(');
  *eq = es;
  while(s<es){
    if(*s == ')'){
      c--;
      if(c==0){
        *eq = s;
        return eq-q;
      }
    }
    if(*s == '('){
      c++;
    }
    s++;
  }
  return eq - q;
}

/*
Parseparens parses the block of commands inside a pair of parentheses.
Then it replaces the queued commands with whitespace so they are not parsed again.
Finally it adds the parsed blcok to the left side of a listcmd.
*/
struct cmd*
parseparens(char **ps, char *es){
  char *q, *eq, *hq, *pq, *pqh;
  struct cmd *cmd;

  matchparens(ps, es, &q, &eq);//match the outermost parentheses
  if(q)
    hq=q;//hold the front of the parens so it can be deleted later
  pq = mkcopy(q,eq);//make a working copy of the parentheses, terminal null important
  pqh = pq;//hold the start of the working copy so it can be freed
  cmd = parseblock(&pq, pq+strlen(pq));//parseblock on the copy of parenthesized input
  free(pqh);//free the copy
  if(hq){//replace the parentheses just covered with whitespace
    while(hq<=eq){
      *hq=' ';
      hq++;
    }
  }
  cmd = listcmd(cmd, parseline(ps, es));//add the commands to the right side of a new list
  return cmd;//return to parseexec

}

struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;
  
  if((q=strchr(*ps, '(')))//bump any parentheses to the front of the execution order
    return parseparens(ps, es);

  ret = execcmd();
  cmd = (struct execcmd*)ret;//creates an execcmd to return, and a cmd to be manipulated

  argc = 0;
  ret = parseredirs(ret, ps, es);//sends the execcmd to parseredirs, does nothing unless there's a redir
  while(!peek(ps, es, "|);")){//crawls through tokens until terminal character
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a') {
      fprintf(stderr, "syntax error\n");
      exit(-1);
    }
    cmd->argv[argc] = mkcopy(q, eq);//loads argv with a 
    argc++;
    if(argc >= MAXARGS) {
      fprintf(stderr, "too many args\n");
      exit(-1);
    }
    ret = parseredirs(ret, ps, es);//parse redirs again
  }
  cmd->argv[argc] = 0;//null terminate argv array
  return ret;
}
